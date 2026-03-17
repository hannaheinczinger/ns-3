#include "metrics.h"
#include <fstream>
#include <iostream>

void Metrics::PrintResults(Ptr<FlowMonitor> monitor, FlowMonitorHelper &helper)
{
    monitor->CheckForLostPackets();

    std::ofstream flowFile("results/flow_results.csv");
    std::ofstream summaryFile("results/summary.csv");

    // CSV headers
    flowFile << "FlowId,Throughput_Mbps,Delay_s,PacketLoss\n";

    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalThroughput = 0;
    double fairnessNumerator = 0;
    double fairnessDenominator = 0;

    for (auto &flow : stats)
    {
        double duration =
            flow.second.timeLastRxPacket.GetSeconds() -
            flow.second.timeFirstTxPacket.GetSeconds();

        double throughput = 0.0;
        if (duration > 0)
        {
            throughput = flow.second.rxBytes * 8.0 / duration / 1e6;
        }

        totalThroughput += throughput;

        fairnessNumerator += throughput;
        fairnessDenominator += throughput * throughput;

        double delay = 0.0;
        if (flow.second.rxPackets > 0)
        {
            delay = flow.second.delaySum.GetSeconds() / flow.second.rxPackets;
        }

        uint32_t packetLoss = flow.second.txPackets - flow.second.rxPackets;

        // write to file instead of only printing
        flowFile << flow.first << ","
                 << throughput << ","
                 << delay << ","
                 << packetLoss << "\n";

        // still print (optional)
        std::cout << "Flow " << flow.first << std::endl;
        std::cout << "Throughput: " << throughput << " Mbps" << std::endl;
        std::cout << "----------------------" << std::endl;
    }

    uint32_t n = stats.size();
    double fairness = 0.0;

    if (fairnessDenominator > 0)
    {
        fairness = (fairnessNumerator * fairnessNumerator) /
                   (n * fairnessDenominator);
    }

    // write summary
    summaryFile << "TotalThroughput_Mbps,JainFairness\n";
    summaryFile << totalThroughput << "," << fairness << "\n";

    flowFile.close();
    summaryFile.close();
}

void Metrics::LogThroughputOverTime(Ptr<FlowMonitor> monitor)
{
    static std::ofstream timeFile("results/throughput_time.csv", std::ios::app);

    double now = Simulator::Now().GetSeconds();

    double totalThroughput = 0;

    auto stats = monitor->GetFlowStats();

    for (auto &flow : stats)
    {
        double duration =
            flow.second.timeLastRxPacket.GetSeconds() -
            flow.second.timeFirstTxPacket.GetSeconds();

        if (duration > 0)
        {
            double throughput = flow.second.rxBytes * 8.0 / duration / 1e6;
            totalThroughput += throughput;
        }
    }

    timeFile << now << "," << totalThroughput << "\n";

    Simulator::Schedule(Seconds(0.5), &Metrics::LogThroughputOverTime, monitor);
}