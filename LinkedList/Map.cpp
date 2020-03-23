// Map.cpp

#include "Map.h"

Map::Map()
    : m_size(0)
{
    //set head pointer to a dummy node
    m_head = &m_dummy;
    m_dummy.m_next = m_head;
    m_dummy.m_prev = m_head;
}

Map::Map(const Map& m) {
    m_size = m.m_size;
    //set head pointer to dummy node
    m_head = &m_dummy;
    m_dummy.m_next = m_head;
    m_dummy.m_prev = m_head;
    Pair* thisTemp = m_head;
    Pair* otherTemp = m.m_head->m_next;
    //add all the nodes of m to this map
    for (int i = 0; i < m_size; i++) {
        Pair* newPair = new Pair;
        newPair->m_key = otherTemp->m_key;
        newPair->m_value = otherTemp->m_value;
        newPair->m_prev = thisTemp;
        newPair->m_next = m_head;
        thisTemp->m_next = newPair;
        m_head->m_prev = newPair;
        
        thisTemp = thisTemp->m_next;
        otherTemp = otherTemp->m_next;

    }
}

Map& Map::operator=(const Map& m) {
    if (this == &m) {
        return *this;
    }
    KeyType key;
    ValueType val;
    //erase all nodes in current map
    while (m_size != 0) {
        get(0, key, val);
        erase(key);
    }
    
    m_size = m.m_size;
    //add all the nodes of m to this map
    Pair* thisTemp = m_head;
    Pair* otherTemp = m.m_head->m_next;
    for (int i = 0; i < m_size; i++) {
        Pair* newPair = new Pair;
        newPair->m_key = otherTemp->m_key;
        newPair->m_value = otherTemp->m_value;
        newPair->m_prev = thisTemp;
        newPair->m_next = m_head;
        thisTemp->m_next = newPair;
        m_head->m_prev = newPair;

        thisTemp = thisTemp->m_next;
        otherTemp = otherTemp->m_next;
     
    }
    

    return *this;
}

Map::~Map() {
    Pair* del = m_head->m_next;
    //delete each node 1 at a time starting from the first node
    while (del != m_head) {
        m_head->m_next = del->m_next;
        delete del;
        del = m_head->m_next;  
    }

    
} 

bool Map::erase(const KeyType& key)
{
    int pos = find(key);
    if (pos == -1)  // not found
        return false;

    Pair* killMe = m_head->m_next;
    for (int i = 0; i < pos; i++) { //iterate killme pointer until position of key
        killMe = killMe->m_next;
    }
    //delete key and reconnect appropriate nodes
    Pair* p1 = killMe->m_next;
    p1->m_prev = killMe->m_prev;
    Pair* p2 = killMe->m_prev;
    p2->m_next = killMe->m_next;
    delete killMe;

    m_size--;
    
    return true;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
    int pos = find(key);
    if (pos == -1)  // not found
        return false;
    Pair* p = m_head->m_next;
    for (int i = 0; i < pos; i++) { //iterate until the position where key is found
        p = p->m_next;
    }
    //update value with the value at key
    value = p->m_value;
    return true;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i < 0 || i >= m_size)
        return false;
    Pair* p = m_head->m_next;
    for (int j = 0; j < i ; j++) { //iterate until position i
        p = p->m_next;
    }
    //set value and key located at the node
    value = p->m_value;
    key = p->m_key;
    return true;
}

void Map::swap(Map& other)
{
    //swap m_head pointers
    Pair* temp = m_head; 
    m_head = other.m_head;
    other.m_head = temp;

    // Swap sizes

    int t = m_size; 
    m_size = other.m_size;
    other.m_size = t;
}

int Map::find(const KeyType& key) const
{
    
    if (m_size == 0) {
        return -1;
    }

    int pos = 0;
    //iterate until the key is found or return -1
    for (Pair* p = m_head->m_next; p != m_head; p = p->m_next) { 
        if (p->m_key == key) {
            return pos;
        }
        pos++;
    }

    return -1;

}

bool Map::doInsertOrUpdate(const KeyType& key, const ValueType& value,
    bool mayInsert, bool mayUpdate)
{
    int pos = find(key);
    if (pos != -1)  // found
    {
        if (mayUpdate) {
            Pair* p = m_head->m_next;
            for (int j = 0; j < pos; j++) { //iterate to the right position
                p = p->m_next;
            }
            p->m_value = value; //update value
        }
        return mayUpdate;
    }   
    if (!mayInsert)  // not found, and not allowed to insert
        return false;
    //create new pair and insert to the end of linked list, while reconnecting necessary nodes
    Pair* newPair = new Pair; 
    newPair->m_key = key;
    newPair->m_value = value;
    newPair->m_next = m_head;
    Pair* p = m_head->m_prev;
    p->m_next = newPair;
    m_head->m_prev = newPair;
    newPair->m_prev = p;

    m_size++;
    return true;
}

bool combine(const Map& m1, const Map& m2, Map& result) {
    Map mtemp1 = m1;
    Map mtemp2 = m2;
    while (result.size() != 0) { //erase result
        KeyType key;
        ValueType val;
        result.get(0, key, val);
        result.erase(key);
    }
    
    bool end = true;
    for (int i = 0; i < mtemp1.size(); i++) { //add all items of m1 to result
        KeyType key;
        ValueType val;
        mtemp1.get(i, key, val);
        result.insert(key, val);
    }

    for (int i = 0; i < mtemp2.size(); i++) { //add all items of m2 to result
        KeyType key;
        ValueType val;
        mtemp2.get(i, key, val);
        if (result.contains(key)) { //check if a key in m2 appears in result and does not insert
            ValueType val2;
            result.get(key, val2);
            if (val != val2) {
                end = false;
                result.erase(key);
            }
            continue;
        }
        result.insert(key,val);
    }

    return end;
}

void reassign(const Map& m, Map& result) { 
    Map mtemp = m;
    result = mtemp;
     ValueType first;
    KeyType key;
    KeyType garbage;
    ValueType val;
    result.get(0, key, first);
    for (int i = 0; i < result.size() - 1; i++) { //shift values of keys to the right
        result.get(i, key, val);
        result.get(i + 1, garbage, val);
        result.update(key, val);
    }
    result.get(result.size() - 1, key, val); //replace the value of the last key to the value of the first key
    result.update(key, first); 
    
}