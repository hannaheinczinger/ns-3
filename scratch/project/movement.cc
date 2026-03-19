#include "movement.h"

MobilityHelper Movement::InitMovement() {
    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(10.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    return mobility;
}

void Movement::SetToRandomWalkMovement(MobilityHelper &mobility, NodeContainer &nodes) {
    Ptr<RandomRectanglePositionAllocator> pos = CreateObject<RandomRectanglePositionAllocator>();
    
    // Configure X random variable
    Ptr<UniformRandomVariable> xVar = CreateObject<UniformRandomVariable>();
    xVar->SetAttribute("Min", DoubleValue(-50.0));
    xVar->SetAttribute("Max", DoubleValue(50.0));
    pos->SetX(xVar);

    // Configure Y random variable
    Ptr<UniformRandomVariable> yVar = CreateObject<UniformRandomVariable>();
    yVar->SetAttribute("Min", DoubleValue(-50.0));
    yVar->SetAttribute("Max", DoubleValue(50.0));
    pos->SetY(yVar);

    // 2. Tell the helper to use this allocator instead of the Grid one
    mobility.SetPositionAllocator(pos);

    // 3. Set the model with matching bounds
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, 50, -50, 50)));
    
    mobility.Install(nodes);
}

void Movement::SetAPToStaticMovement(MobilityHelper &mobility, NodeContainer &ap) {
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ap);
}

void Movement::SetToStaticMovement(MobilityHelper &mobility, NodeContainer &nodes, Ptr<Node> apNode, double min_distance, double max_distance) {
    // Get AP position
    Ptr<MobilityModel> apMobility = apNode->GetObject<MobilityModel>();
    Vector apPos = apMobility->GetPosition();

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // Place nodes at fixed distance (circle around AP)
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {   
        // Set distance between STA and AP based on min_distance and max_distance
        Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
        uv->SetAttribute("Min", DoubleValue(min_distance));
        uv->SetAttribute("Max", DoubleValue(max_distance));
        double distance = uv->GetValue();
        
        Ptr<Node> node = nodes.Get(i);
        Ptr<MobilityModel> mob = node->GetObject<MobilityModel>();

        double angle = 2 * M_PI * i / nodes.GetN();

        double x = apPos.x + distance * std::cos(angle);
        double y = apPos.y + distance * std::sin(angle);

        mob->SetPosition(Vector(x, y, apPos.z));
    }
}

void Movement::SetToStaticMovement4(MobilityHelper &mobility, NodeContainer &nodes, Ptr<Node> apNode) {
    // Get AP position
    Ptr<MobilityModel> apMobility = apNode->GetObject<MobilityModel>();
    Vector apPos = apMobility->GetPosition();

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    double distances[4] = {1.0, 5.0, 15.0, 30.0};

    // Place nodes at fixed distance (circle around AP)
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {   
        double distance = distances[i];
        
        Ptr<Node> node = nodes.Get(i);
        Ptr<MobilityModel> mob = node->GetObject<MobilityModel>();

        double angle = 2 * M_PI * i / nodes.GetN();

        double x = apPos.x + distance * std::cos(angle);
        double y = apPos.y + distance * std::sin(angle);

        mob->SetPosition(Vector(x, y, apPos.z));
    }
}