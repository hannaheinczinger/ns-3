#include "wifi-ax-setup.h"

WifiAxSetup::WifiAxSetup(uint32_t nStations)
{
    m_nStations = nStations;
}

void WifiAxSetup::Setup()
{
    // setup nodes, extendable to multi-clients
    staNodes.Create(m_nStations);
    apNode.Create(1);

    // setup the 802.11ax standard
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211ax);

    WifiMacHelper mac;

    Ssid ssid = Ssid("wifi-ax-network");

    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid));

    staDevices = wifi.Install(phy, mac, staNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    apDevice = wifi.Install(phy, mac, apNode);

    // setup static postions of nodes
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(staNodes);
    mobility.Install(apNode);

    InternetStackHelper stack;
    stack.Install(staNodes);
    stack.Install(apNode);

    // setup addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    address.Assign(staDevices);
    address.Assign(apDevice);
}

NodeContainer WifiAxSetup::GetStaNodes()
{
    return staNodes;
}

NodeContainer WifiAxSetup::GetApNode()
{
    return apNode;
}

NetDeviceContainer WifiAxSetup::GetStaDevices()
{
    return staDevices;
}

NetDeviceContainer WifiAxSetup::GetApDevice()
{
    return apDevice;
}