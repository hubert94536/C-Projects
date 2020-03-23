#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_sm;
    string getDirection(StreetSegment seg1) const;
    void generateDeliveryCommand(vector<DeliveryCommand>& commands, list<StreetSegment> path, string request) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_sm = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    DeliveryOptimizer op(m_sm);
    double oldCrowDist;
    double newCrowDist;
    vector<DeliveryRequest> opDeliveries;
    for (int i = 0; i < deliveries.size(); i++) {
        DeliveryRequest d = deliveries[i];
        opDeliveries.push_back(d);
    }
    op.optimizeDeliveryOrder(depot, opDeliveries, oldCrowDist, newCrowDist);
    PointToPointRouter pathFinder(m_sm);
    list<StreetSegment> path;
    double distance = 0;
    double totalDistance = 0;
    
    //create delivery command from depot to first delivery
    DeliveryResult result = pathFinder.generatePointToPointRoute(depot, opDeliveries[0].location, path, distance);
    if (result == NO_ROUTE || result == BAD_COORD) {
        return result;
    }

    totalDistance += distance;
    generateDeliveryCommand(commands, path, opDeliveries[0].item);

    //create delivery command for all deliveries
    for (int i = 1; i < opDeliveries.size(); i++) {
        result = pathFinder.generatePointToPointRoute(opDeliveries[i - 1].location, opDeliveries[i].location, path, distance);
        totalDistance+= distance;
        generateDeliveryCommand(commands, path, opDeliveries[i].item);
        if (result == NO_ROUTE || result == BAD_COORD) {
            return result;
        }
    }

    //create delivery command from last delivery back to depot
    result = pathFinder.generatePointToPointRoute(opDeliveries[opDeliveries.size() - 1].location, depot, path, distance);
    totalDistance += distance;
    if (result == NO_ROUTE || result == BAD_COORD) {
        return result;
    }
    generateDeliveryCommand(commands, path, "");
    totalDistanceTravelled = totalDistance;
    return DELIVERY_SUCCESS;
}

void DeliveryPlannerImpl::generateDeliveryCommand(vector<DeliveryCommand>& commands, list<StreetSegment> path, string request) const {
    list<StreetSegment>::iterator it = path.begin();
    StreetSegment firstSeg = *it;
    StreetSegment lastSeg;
    double coordDistance = 0;
    for (; it != path.end(); it++) {
        if (it->name != firstSeg.name ) {
            //initialize proceed command
            DeliveryCommand proceed;
            string direction = getDirection(firstSeg);
            proceed.initAsProceedCommand(direction, firstSeg.name, coordDistance); 
            coordDistance = 0;
            commands.push_back(proceed);
            //initialize turn command
            DeliveryCommand turn;
            double dir = angleBetween2Lines(lastSeg, *it);
            if (dir >= 1 && dir < 180) {
                turn.initAsTurnCommand("left", it->name);
                commands.push_back(turn);
            }

            else if (dir >= 180 && dir <= 359) {
                turn.initAsTurnCommand("right", it->name);
                commands.push_back(turn);
            }
            firstSeg = *it;
            
        }
        coordDistance += distanceEarthMiles(it->start, it->end);
        lastSeg = *it;
    }
    //initialize last proceed command
    it--;
    DeliveryCommand proceed;
    string direction = getDirection(*it);
    proceed.initAsProceedCommand(direction, firstSeg.name, coordDistance);
    commands.push_back(proceed);
    if (request != "") {
        DeliveryCommand delivered;
        delivered.initAsDeliverCommand(request);
        commands.push_back(delivered);
    }
}

string  DeliveryPlannerImpl::getDirection(StreetSegment seg1) const {
    double dir = angleOfLine(seg1);
    //get direction
    if (dir >= 0 && dir < 22.5) {
        return "east";
    }

    else if (dir >= 22.5 && dir < 67.5) {
        return "northeast";
    }

    else if (dir >= 67.5 && dir < 112.5) {
        return "north";
    }

    else if (dir >= 112.5 && dir < 157.5) {
        return "northwest";
    }

    else if (dir >= 157.5 && dir < 202.5) {
        return "west";
    }

    else if (dir >= 202.5 && dir < 247.5) {
        return "southwest";
    }

    else if (dir >= 247.5 && dir < 292.5) {
        return "south";
    }

    else if (dir >= 292.5 && dir < 337.5) {
        return "southeast";
    }

    else {
        return "east";
    }
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
