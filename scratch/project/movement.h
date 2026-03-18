#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/node-container.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"

using namespace ns3;

class Movement {
    public: 
    MobilityHelper InitMovement();
    void SetToRandomWalkMovement(MobilityHelper &moblilty, NodeContainer &nodes);
    void SetAPToStaticMovement(MobilityHelper &mobility, NodeContainer &ap);
    void SetToStaticMovement(MobilityHelper &mobility, NodeContainer &nodes, Ptr<Node> apNode, double min_distance, double max_distance);

};

#endif