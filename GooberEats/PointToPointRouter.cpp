#include "provided.h"
#include "ExpandableHashMap.h"
#include<unordered_map>
#include <list>
#include <cmath>
#include <memory>
#include <vector>


class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        std::list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct Node {
        double f;
        double g;
        double h;
        GeoCoord coord;
        StreetSegment street;
        std::shared_ptr<Node> prev;
    }; 
    const StreetMap* m_sm;
    /*double getDistance(GeoCoord startCoord, GeoCoord endCoord) const;*/
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_sm = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        std::list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{


    std::vector<StreetSegment> segs;
    //check for bad coord
    if (!m_sm->getSegmentsThatStartWith(start, segs) || !m_sm->getSegmentsThatStartWith(end, segs)) {
        return BAD_COORD;
    }
    //check if start and end coordinates are the same
    if (start == end) {
        route.clear();
        totalDistanceTravelled = 0;
        return DELIVERY_SUCCESS;
    }
    route.clear();
    ExpandableHashMap<GeoCoord, StreetSegment> m_closed;
    std::vector<std::shared_ptr<Node>> m_open;
    //push starting coordinate
    auto startNode = std::make_shared<Node>();
    startNode->prev = nullptr;
    startNode->coord = start;
    startNode->f = 0;
    startNode->g = 0;
    startNode->h = distanceEarthMiles(start, end);
    m_open.push_back(startNode);
    bool found = false;
    auto prevNode = startNode;
    std::shared_ptr<Node> current = nullptr;
    while (m_open.size() != 0 && !found) {
        current = m_open[0]; 
        //find Node with lowest f value
        int index = 0;
        std::vector<std::shared_ptr<Node>>::iterator it = m_open.begin();
        for (int i = 0; i < m_open.size(); i++) {
            if (m_open[i]->f < current->f && m_closed.find(m_open[i]->coord) == nullptr) {
                current = m_open[i];
                index = i;
            }
        }
        for (int j = 0; j < index; j++) {
            it++;
        }
        //move node from open to closed
        m_open.erase(it);
        m_closed.associate(current->coord, current->street);
        m_sm->getSegmentsThatStartWith(current->coord, segs);
        //push all new nodes into open list and calculate f
        for (int j = 0; j < segs.size(); j++) {
            if (m_closed.find(segs[j].end) == nullptr) {
                std::shared_ptr<Node> newNode = std::make_shared<Node>();
                newNode->street = segs[j];
                newNode->coord = segs[j].end;
                newNode->prev = current;
                m_open.push_back(newNode);
            }
        }
        //calculate g, h, f values for every node in open
        for (int j = 0; j < m_open.size(); j++) {
            if (m_open[j]->g == NULL) {
                m_open[j]->g = current->g + distanceEarthMiles(current->coord, m_open[j]->coord);
            }
            else {
                double tempg = current->g + distanceEarthMiles(current->coord, m_open[j]->coord);
                if (tempg < m_open[j]->g) {
                    m_open[j]->g = current->g + distanceEarthMiles(current->coord, m_open[j]->coord);
                }
            }
            m_open[j]->h = distanceEarthMiles(m_open[j]->coord, end);
            m_open[j]->f = m_open[j]->g + m_open[j]->h;
        }
        prevNode = current;
        //check if reached end
        if (current->coord == end) {
            found = true;
        }
    }
    totalDistanceTravelled = 0;
    if (found) {
        //totalDistanceTravelled = current->g;
        while (current->f != 0 ) {
            route.push_front(current->street);
            totalDistanceTravelled += distanceEarthMiles(current->street.start, current->street.end);
            current = current->prev;
        }
        return DELIVERY_SUCCESS;
    }
    else {
        return NO_ROUTE;
    }
     
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        std::list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
