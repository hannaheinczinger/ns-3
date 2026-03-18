#include "wifi-setup.h"

WifiSetup::WifiSetup(uint32_t nStations,
                     const std::string& standard,
                     bool enableAmpdu,
                     bool enableAmsdu,
                     uint32_t ampduMpdus,
                     uint32_t msduLength,
                     bool addPropagation)
    : m_nStations(nStations),
      m_standard(standard),
      m_enableAmpdu(enableAmpdu),
      m_enableAmsdu(enableAmsdu),
      m_ampduMpdus(ampduMpdus),
      m_msduLength(msduLength),
      m_addPropagation(addPropagation)
{
}

void WifiSetup::Setup()
{
    // setup station nodes, extendable to multi-clients
    staNodes.Create(m_nStations);
    apNode.Create(1);

    // setup the 802.11ac standard
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    if(m_addPropagation) {
        channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
        channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    }
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    if (m_standard == "ax") {
        wifi.SetStandard(WIFI_STANDARD_80211ax);
    } else {
        wifi.SetStandard(WIFI_STANDARD_80211ac);
    }

    WifiMacHelper mac;

    Ssid ssid = Ssid(m_standard == "ax" ? "wifi-ax-network" : "wifi-ac-network");

    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid));

    staDevices = wifi.Install(phy, mac, staNodes);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    apDevice = wifi.Install(phy, mac, apNode);

    if (m_enableAmpdu && m_ampduMpdus > 1) {
        
        // will only work for single user scenario
        Ptr<NetDevice> staDev = staDevices.Get(0);
        Ptr<NetDevice> apDev = apDevice.Get(0);

        Ptr<WifiNetDevice> wifiStaDev = DynamicCast<WifiNetDevice>(staDev);
        Ptr<WifiNetDevice> wifiApDev = DynamicCast<WifiNetDevice>(apDev);

        wifiStaDev->GetMac()->SetAttribute("BE_MaxAmpduSize", UintegerValue(m_ampduMpdus));
        wifiApDev->GetMac()->SetAttribute("BE_MaxAmpduSize", UintegerValue(m_ampduMpdus));
        if (m_enableAmsdu)
        {
            wifiStaDev->GetMac()->SetAttribute("BE_MaxAmsduSize", UintegerValue(m_msduLength));
            wifiApDev->GetMac()->SetAttribute("BE_MaxAmsduSize", UintegerValue(m_msduLength));
        }
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

NodeContainer WifiSetup::GetStaNodes()
{
    return staNodes;
}

NodeContainer WifiSetup::GetApNode()
{
    return apNode;
}

NetDeviceContainer WifiSetup::GetStaDevices()
{
    return staDevices;
}

NetDeviceContainer WifiSetup::GetApDevice()
{
    return apDevice;
}