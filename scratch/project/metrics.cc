#include "metrics.h"
#include <iostream>

void Metrics::PrintResults(Ptr<FlowMonitor> monitor, FlowMonitorHelper &helper)
{
    monitor->CheckForLostPackets();

    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalThroughput = 0;
    double fairnessNumerator = 0;
    double fairnessDenominator = 0;

    for (auto &flow : stats)
    {
        double throughput =
            flow.second.rxBytes * 8.0 /
            (flow.second.timeLastRxPacket.GetSeconds() -
             flow.second.timeFirstTxPacket.GetSeconds()) / 1e6;

        totalThroughput += throughput;

        // calculate for Jain's fairness index
        fairnessNumerator += throughput;
        fairnessDenominator += throughput * throughput;

        // print information per flow
        std::cout << "Flow " << flow.first << std::endl;
        std::cout << "Throughput: " << throughput << " Mbps" << std::endl;
        std::cout << "Delay: "
                  << flow.second.delaySum.GetSeconds() / flow.second.rxPackets
                  << " s" << std::endl;
        std::cout << "Packet Loss: "
                  << (flow.second.txPackets - flow.second.rxPackets)
                  << std::endl;

        std::cout << "----------------------" << std::endl;
    }

    uint32_t n = stats.size();

    double fairness = (fairnessNumerator * fairnessNumerator) / (n * fairnessDenominator);

    // print final throughput and jain's fairness
    std::cout << "Total Throughput: " << totalThroughput << " Mbps" << std::endl;
    std::cout << "Jain's Fairness Index: " << fairness << std::endl;
}