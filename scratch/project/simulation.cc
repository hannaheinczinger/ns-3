#include "wifi-ax-setup.h"
#include "wifi-ac-setup.h"
#include "traffic.h"
#include "metrics.h"

#include "ns3/flow-monitor-module.h"

int main()
{
    bool useAx = true;
    uint32_t nUsers = 1;

    NodeContainer staNodes;
    NodeContainer apNode;

    if (useAx)
    {
        WifiAxSetup sim(nUsers);
        sim.Setup();

        staNodes = sim.GetStaNodes();
        apNode = sim.GetApNode();
    }
    else
    {
        WifiAcSetup sim(nUsers);
        sim.Setup();

        staNodes = sim.GetStaNodes();
        apNode = sim.GetApNode();
    }

    // generate traffic
    TrafficGenerator::InstallTraffic(staNodes, apNode, "low");

    // monitor traffic
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(20));
    Simulator::Run();

    Metrics::PrintResults(monitor, flowHelper);

    Simulator::Destroy();
}