#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

class WifiSetup
{
public:
    WifiSetup(uint32_t nStations,
                const std::string& standard,
                bool enableAmpdu,
                bool enableAmsdu,
                uint32_t ampduMpdus,
                uint32_t msduLength, 
                bool addPropagation);

    void Setup();

    NodeContainer GetStaNodes();
    NodeContainer GetApNode();

    NetDeviceContainer GetStaDevices();
    NetDeviceContainer GetApDevice();

private:
    uint32_t m_nStations;
    std::string m_standard;
    bool m_enableAmpdu;
    bool m_enableAmsdu;
    uint32_t m_ampduMpdus;
    uint32_t m_msduLength;
    bool m_addPropagation;

    NodeContainer staNodes;
    NodeContainer apNode;

    NetDeviceContainer staDevices;
    NetDeviceContainer apDevice;
};

#endif