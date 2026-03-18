#include "wifi-ax-setup.h"
#include "wifi-ac-setup.h"
#include "traffic.h"
#include "metrics.h"

#include "ns3/flow-monitor-module.h"

int main(int argc, char *argv[])
{
    std::string standard = "ax";
    uint32_t nUsers = 8;
    std::string trafficType = "high";
    uint32_t packetSize = 1500;
    std::string movementType = "Static10m";
    std::string addPropagation = "false";
    std::string runName = "ax_users8_high_pkt1500";

    CommandLine cmd;
    cmd.AddValue("standard", "ax or ac", standard);
    cmd.AddValue("nUsers", "Number of users", nUsers);
    cmd.AddValue("trafficType", "Traffic type", trafficType);
    cmd.AddValue("packetSize", "Packet size in bytes", packetSize);
    cmd.AddValue ("movementType", "Type of STA movement", movementType);
    cmd.AddValue("addPropagation", "Enable propagation", addPropagation);
    cmd.AddValue("runName", "Unique run name for file outputs", runName);
    cmd.Parse(argc, argv);

    bool useAx = (standard == "ax");

    LogComponentEnable("RrMultiUserScheduler", LOG_LEVEL_INFO);
    LogComponentEnable("HeFrameExchangeManager", LOG_LEVEL_INFO);

    NodeContainer staNodes;
    NodeContainer apNode;

    if (useAx)
    {
        WifiAxSetup sim(nUsers);
        sim.Setup(movementType, addPropagation);

        staNodes = sim.GetStaNodes();
        apNode = sim.GetApNode();
    }
    else
    {
        WifiAcSetup sim(nUsers);
        sim.Setup(movementType, addPropagation);

        staNodes = sim.GetStaNodes();
        apNode = sim.GetApNode();
    }

    // generate traffic
    TrafficGenerator::InstallTraffic(staNodes, apNode, trafficType, packetSize);

    // monitor traffic
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Metrics::LogThroughputOverTime(monitor, runName);

    Simulator::Stop(Seconds(12));
    Simulator::Run();

    Metrics::PrintResults(monitor, flowHelper, standard, nUsers, trafficType, packetSize);

    Simulator::Destroy();
    return 0;
}