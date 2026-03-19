#include "wifi-setup.h"
#include "traffic.h"
#include "metrics.h"
#include "movement.h"

#include "ns3/flow-monitor-module.h"

int main(int argc, char *argv[])
{
    std::string standard = "ax";
    uint32_t nUsers = 8;
    std::string trafficType = "high";
    uint32_t packetSize = 1500;
    std::string movementType = "Static10m";
    bool addPropagation = false;
    std::string runName = "ax_users8_high_pkt1500";
    bool enableAmpdu = false;
    bool enableAmsdu = false;
    uint32_t ampduMpdus = 64;
    uint32_t msduLength = 1500;

    CommandLine cmd;
    cmd.AddValue("standard", "ax or ac", standard);
    cmd.AddValue("nUsers", "Number of users", nUsers);
    cmd.AddValue("trafficType", "Traffic type", trafficType);
    cmd.AddValue("packetSize", "Packet size in bytes", packetSize);
    cmd.AddValue ("movementType", "Type of STA movement", movementType);
    cmd.AddValue("addPropagation", "Enable propagation", addPropagation);
    cmd.AddValue("runName", "Unique run name for file outputs", runName);
    cmd.AddValue("enableAmpdu", "Enable A-MPDU aggregation", enableAmpdu);
    cmd.AddValue("enableAmsdu", "Enable A-MSDU aggregation", enableAmsdu);
    cmd.AddValue("ampduMpdus", "Target A-MPDU depth in MPDUs", ampduMpdus);
    cmd.AddValue("msduLength", "MSDU payload length in bytes", msduLength);
    cmd.Parse(argc, argv);

    LogComponentEnable("RrMultiUserScheduler", LOG_LEVEL_INFO);
    LogComponentEnable("HeFrameExchangeManager", LOG_LEVEL_INFO);

    NodeContainer staNodes;
    NodeContainer apNode;

    
    WifiSetup sim(nUsers,
                  standard,
                  enableAmpdu,
                  enableAmsdu,
                  ampduMpdus,
                  msduLength,
                  addPropagation);
    sim.Setup();

    staNodes = sim.GetStaNodes();
    apNode = sim.GetApNode();

    
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
        moveManager.SetToStaticMovement4(mobility, staNodes, apNode.Get(0));
    } else {            //if(movementType == "Dynamic") {
        moveManager.SetToRandomWalkMovement(mobility, staNodes);        
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