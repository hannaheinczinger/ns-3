#include "traffic.h"

void TrafficGenerator::InstallTraffic(NodeContainer staNodes,
                                       NodeContainer apNode,
                                       std::string trafficType)
{
    uint16_t port = 5000;

    uint32_t packetSize;
    std::string dataRate;

    // TODO: extend traffic types for experiments
    if (trafficType == "low")
    {
        packetSize = 160;
        dataRate = "64kbps";
    }
    else
    {
        packetSize = 1500;
        dataRate = "10Mbps";
    }

    for (uint32_t i = 0; i < staNodes.GetN(); i++)
    {
        Address sinkAddress(InetSocketAddress(Ipv4Address::GetAny(), port));

        // send UDP packets
        PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", sinkAddress);
        ApplicationContainer sinkApp = sinkHelper.Install(apNode.Get(0));

        sinkApp.Start(Seconds(0.0));
        sinkApp.Stop(Seconds(20.0));

        OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(apNode.Get(0)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal(), port));

        onoff.SetConstantRate(DataRate(dataRate), packetSize);

        ApplicationContainer clientApp = onoff.Install(staNodes.Get(i));
        clientApp.Start(Seconds(1.0 + i * 0.1));
        clientApp.Stop(Seconds(20.0));

        port++;
    }
}