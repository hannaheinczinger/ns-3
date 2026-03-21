#include "metrics.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>

void Metrics::PrintResults(Ptr<FlowMonitor> monitor, FlowMonitorHelper &helper, const std::string &standard, uint32_t nUsers, const std::string &trafficType, uint32_t packetSize)
{
    mkdir("results", 0777);
    monitor->CheckForLostPackets();
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    std::string summaryFilename = "results/summary.csv";
    std::string flowFilename = "results/per_flow.csv";

    std::ofstream summaryFile(summaryFilename, std::ios::app);
    std::ofstream flowFile(flowFilename, std::ios::app);

    static bool summaryHeaderChecked = false;
    static bool flowHeaderChecked = false;

    if (!summaryHeaderChecked)
    {
        std::ifstream check(summaryFilename);
        if (check.peek() == std::ifstream::traits_type::eof())
        {
            summaryFile << "standard,nUsers,trafficType,packetSize,totalThroughputMbps,avgDelayMs,jainFairness,packetDropRate\n";
        }
        summaryHeaderChecked = true;
    }

    if (!flowHeaderChecked)
    {
        std::ifstream check(flowFilename);
        if (check.peek() == std::ifstream::traits_type::eof())
        {
            flowFile << "standard,nUsers,trafficType,packetSize,flowId,throughputMbps,avgDelayMs,txPackets,rxPackets,lostPackets\n";
        }
        flowHeaderChecked = true;
    }
 double totalThroughput = 0.0;
    double fairnessNumerator = 0.0;
    double fairnessDenominator = 0.0;
    double totalDelayMsWeighted = 0.0;
    uint64_t totalRxPackets = 0;
    uint64_t totalTxPackets = 0;

    for (auto &flow : stats)
    {
        double throughput = 0.0;
        double duration =
            flow.second.timeLastRxPacket.GetSeconds() -
            flow.second.timeFirstTxPacket.GetSeconds();

        if (duration > 0)
        {
            throughput = flow.second.rxBytes * 8.0 / duration / 1e6;
        }

        double avgDelayMs = 0.0;
        if (flow.second.rxPackets > 0)
        {
            avgDelayMs = (flow.second.delaySum.GetSeconds() / flow.second.rxPackets) * 1000.0;
        }

        uint64_t lostPackets = flow.second.txPackets - flow.second.rxPackets;

        totalThroughput += throughput;
        fairnessNumerator += throughput;
        fairnessDenominator += throughput * throughput;
        totalDelayMsWeighted += avgDelayMs * flow.second.rxPackets;
        totalRxPackets += flow.second.rxPackets;
        totalTxPackets += flow.second.txPackets;

        flowFile << standard << ","
                 << nUsers << ","
                 << trafficType << ","
                 << packetSize << ","
                 << flow.first << ","
                 << throughput << ","
                 << avgDelayMs << ","
                 << flow.second.txPackets << ","
                 << flow.second.rxPackets << ","
                 << lostPackets << "\n";

        std::cout << "Flow " << flow.first << "\n";
        std::cout << "Throughput: " << throughput << " Mbps\n";
        std::cout << "Delay: " << avgDelayMs << " ms\n";
        std::cout << "Packet Loss: " << lostPackets << "\n";
        std::cout << "----------------------\n";
    }

    double avgDelayMs = 0.0;
    if (totalRxPackets > 0)
    {
        avgDelayMs = totalDelayMsWeighted / totalRxPackets;
    }

    double fairness = 0.0;
    if (!stats.empty() && fairnessDenominator > 0.0)
    {
        fairness = (fairnessNumerator * fairnessNumerator) /
                   (stats.size() * fairnessDenominator);
    }

    double packetDropRate = 0.0;
    if (totalTxPackets > 0)
    {
        packetDropRate = static_cast<double>(totalTxPackets - totalRxPackets) / totalTxPackets;
    }

    summaryFile << standard << ","
                << nUsers << ","
                << trafficType << ","
                << packetSize << ","
                << totalThroughput << ","
                << avgDelayMs << ","
                << fairness << ","
                << packetDropRate << "\n";

    std::cout << "Total Throughput: " << totalThroughput << " Mbps\n";
    std::cout << "Average Delay: " << avgDelayMs << " ms\n";
    std::cout << "Jain's Fairness Index: " << fairness << "\n";
    std::cout << "Packet Drop Rate: " << packetDropRate << "\n";
}

void Metrics::LogThroughputOverTime(Ptr<FlowMonitor> monitor, const std::string &runName)
{
    std::string dir = "results/throughput_time";
    mkdir("results", 0777);
    mkdir(dir.c_str(), 0777);

    std::string filename = dir + "/" + runName + ".csv";

    static std::map<std::string, bool> headerWritten;

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

     std::ofstream timeFile(filename, std::ios::app);

    if (!headerWritten[filename])
    {
        std::ifstream checkFile(filename);
        if (checkFile.peek() == std::ifstream::traits_type::eof())
        {
            timeFile << "time_s,totalThroughputMbps\n";
        }
        headerWritten[filename] = true;
    }

    timeFile << now << "," << totalThroughput << "\n";

    Simulator::Schedule(Seconds(0.1), &Metrics::LogThroughputOverTime, monitor, runName);
}