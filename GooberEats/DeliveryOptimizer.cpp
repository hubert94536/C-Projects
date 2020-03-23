#include "provided.h"
#include <vector>
#include <random>
#include<algorithm>
#include <cmath> 
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    double getDistance(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = getDistance(depot, deliveries);
    vector<DeliveryRequest> tempDeliveries;
    double maxDistance = distanceEarthMiles(depot, deliveries[0].location);
    //get old crow distance
    for (int i = 0; i < deliveries.size(); i++) {
        if (distanceEarthMiles(depot, deliveries[i].location) > maxDistance) {
            maxDistance = distanceEarthMiles(depot, deliveries[i].location);
        }
        tempDeliveries.push_back(deliveries[i]);
    }
    
    newCrowDistance = oldCrowDistance;
    double tempCrowDist = 0;
    double temp = 1;
    double finalTemp = 0.0001;
    double coolRate = 0.98;
    int numIt = 100;
    double changeE = 0;
    while (temp > finalTemp) {
        //randomly order the deliveries
        for (int i = 0; i < 100; i++) {
            random_shuffle(tempDeliveries.begin(), tempDeliveries.end());
        }
        tempCrowDist = getDistance(depot, tempDeliveries);
        changeE = newCrowDistance -  tempCrowDist;
        //update deliveries if new order is better
        if (changeE > 0) {
            newCrowDistance = tempCrowDist;
            for (int i = 0; i < deliveries.size(); i++) {
                deliveries[i] = tempDeliveries[i];
            }
        }
        //check probability if should update deliveries
        else {
            if (exp(changeE / temp) > rand()) {
                newCrowDistance = tempCrowDist;
                for (int i = 0; i < deliveries.size(); i++) {
                    deliveries[i] = tempDeliveries[i];
                }
            }
        }
        temp *= coolRate;

    }
}

double DeliveryOptimizerImpl::getDistance(const GeoCoord& depot, vector<DeliveryRequest>& deliveries) const {
    double totalDistance = 0;
    totalDistance += distanceEarthMiles(depot, deliveries[0].location);
    for (int i = 1; i < deliveries.size(); i++) {
        totalDistance += distanceEarthMiles(deliveries[i - 1].location, deliveries[i].location);
    }
    totalDistance += distanceEarthMiles(deliveries[deliveries.size()-1].location, depot);
    return totalDistance;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
