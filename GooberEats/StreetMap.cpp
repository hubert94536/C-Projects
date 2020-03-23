#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <functional>
#include "ExpandableHashMap.h"


unsigned int hash(const GeoCoord& g)
{
    return std::hash<std::string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(std::string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, std::vector<StreetSegment>>* m_map;

};

StreetMapImpl::StreetMapImpl()
{
    m_map = new ExpandableHashMap<GeoCoord, std::vector<StreetSegment>>;
}

StreetMapImpl::~StreetMapImpl()
{
    delete m_map;
}

bool StreetMapImpl::load(std::string mapFile)
{
    m_map->reset();
    std::ifstream infile(mapFile);
    if (!infile) {
        return false;
    }

    
    std::string line;
    std::string streetName;
    std::string lat1;
    std::string lon1; 
    std::string lat2;
    std::string lon2;
    std::string extra;
    while (getline(infile, line)) {
        std::vector<StreetSegment>* vec;
        std::istringstream iss(line);   
        //check for geocoord
        if (iss >> lat1 >> lon1 >> lat2 >> lon2 && !(iss >> extra) && isdigit(lat1[1]) && isdigit(lat2[1])) {
            GeoCoord start(lat1, lon1);
            GeoCoord end(lat2, lon2);
            StreetSegment street(start, end, streetName);
            StreetSegment revStreet(end, start, streetName);
            vec = m_map->find(start);
            //inserting street segment
            if (vec != nullptr) {
                vec->push_back(street);
                m_map->associate(start, *vec);
            }

            else {
                vec = new std::vector<StreetSegment>;
                vec->push_back(street); 
                m_map->associate(start, *vec);
                
            }
            //insert reversed street segment
            vec = m_map->find(end);
            if (vec != nullptr) {
                vec->push_back(revStreet);
                m_map->associate(end, *vec);
            }

            else {
                vec = new std::vector<StreetSegment>;
                vec->push_back(revStreet);
                m_map->associate(end, *vec);
                
            }
           
            
        }
        //check for streetname
        else {
            if (line.size() <= 3) {
                continue;
            }
            streetName = line;
        }
        
    }
    
    return true;  
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const
{
    std::vector<StreetSegment>* vec = m_map->find(gc);
    if (vec != nullptr) 
    {
        segs.clear();
        segs = *vec;
        return true;

    }
    return false;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(std::string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, std::vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
