#ifndef WIFI_AC_SETUP_H
#define WIFI_AC_SETUP_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class WifiAcSetup
{
public:
    WifiAcSetup(uint32_t nStations);

    void Setup();

    NodeContainer GetStaNodes();
    NodeContainer GetApNode();

    NetDeviceContainer GetStaDevices();
    NetDeviceContainer GetApDevice();

private:
    uint32_t m_nStations;

    NodeContainer staNodes;
    NodeContainer apNode;

    NetDeviceContainer staDevices;
    NetDeviceContainer apDevice;
};

#endif