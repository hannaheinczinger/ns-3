#ifndef TRAFFIC_H
#define TRAFFIC_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class TrafficGenerator
{
public:
    static void InstallTraffic(NodeContainer staNodes, NodeContainer apNode, std::string trafficType, uint32_t packetSize);
};

#endif