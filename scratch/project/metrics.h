#ifndef METRICS_H
#define METRICS_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

class Metrics
{
public:
    static void PrintResults(Ptr<FlowMonitor> monitor, FlowMonitorHelper &helper);
    static void LogThroughputOverTime(Ptr<FlowMonitor> monitor);
};

#endif