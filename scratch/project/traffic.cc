#include "traffic.h"

void TrafficGenerator::InstallTraffic(NodeContainer staNodes,
                                       NodeContainer apNode,
                                       std::string trafficType,
                                       uint32_t packetSize)
{
    uint16_t port = 5000;
    double dataRate;

    // TODO: extend traffic types for experiments
    if (trafficType == "low")
    {
        dataRate = 64000;
    }
    else if(trafficType == "medium") 
    {
        dataRate = 5000000;
    } 
    else
    {
        dataRate = 50000000;
    }

    // AP IP address
    Ipv4Address apAddress = apNode.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    double intervalSeconds = (packetSize * 8.0) / dataRate;
    Time interval = Seconds(intervalSeconds);

    for (uint32_t i = 0; i < staNodes.GetN(); i++)
    {
        Address sinkAddress(InetSocketAddress(Ipv4Address::GetAny(), port));

        // send UDP packets
        PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", sinkAddress);
        ApplicationContainer sinkApp = sinkHelper.Install(apNode.Get(0));

        sinkApp.Start(Seconds(0.0));
        sinkApp.Stop(Seconds(12.0));

        UdpClientHelper client(apAddress, port);
        client.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        client.SetAttribute("Interval", TimeValue(interval));
        client.SetAttribute("PacketSize", UintegerValue(packetSize));

        ApplicationContainer clientApp = client.Install(staNodes.Get(i));
        clientApp.Start(Seconds(2.0));
        clientApp.Stop(Seconds(12.0));

        port++;
    }
}