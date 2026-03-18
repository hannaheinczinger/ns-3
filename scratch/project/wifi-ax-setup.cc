#include "wifi-ax-setup.h"
#include "movement.h"

WifiAxSetup::WifiAxSetup(uint32_t nStations)
{
    m_nStations = nStations;
}

void WifiAxSetup::Setup(std::string movementType, std::string addPropagation)
{
    // setup nodes, extendable to multi-clients
    staNodes.Create(m_nStations);
    apNode.Create(1);

    // setup the 802.11ax standard
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    if(addPropagation == "true") {
        channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
        channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    }
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

    // Set movement of STA's
    Movement moveManager;
    MobilityHelper mobility = moveManager.InitMovement();
    moveManager.SetAPToStaticMovement(mobility, apNode);
    if(movementType == "Static1m") {
        moveManager.SetToStaticMovement(mobility, staNodes, apNode.Get(0), 1, 1);
    } else if(movementType == "Static10m") {
        moveManager.SetToStaticMovement(mobility, staNodes, apNode.Get(0), 10, 10);        
    } else if(movementType == "Static30m") {
        moveManager.SetToStaticMovement(mobility, staNodes, apNode.Get(0), 30, 30);
    } else if(movementType == "StaticRandom_1m-30m") {
        moveManager.SetToStaticMovement(mobility, staNodes, apNode.Get(0), 1, 30);
    } else {            //if(movementType == "Dynamic") {
        moveManager.SetToRandomWalkMovement(mobility, staNodes);        
    }

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