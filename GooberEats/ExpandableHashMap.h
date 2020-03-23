// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Bucket {
		KeyType m_key;
		ValueType m_value;
		Bucket* m_next;
	};
	void clearMap();
	unsigned int getBucketNumber(const KeyType& key, int numBuckets) const;
	Bucket** m_buckets;
	int m_count;
	int m_numBuckets;
	double m_loadFactor;
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_loadFactor = maximumLoadFactor;
	m_numBuckets = 8;
	m_count = 0;
	m_buckets = new Bucket * [m_numBuckets];
	for (int i = 0; i < m_numBuckets; i++) {
		m_buckets[i] = nullptr;
	}
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	clearMap();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	clearMap();
	m_numBuckets = 8;
	m_buckets = new Bucket * [m_numBuckets];
	for (int i = 0; i < m_numBuckets; i++) {
		m_buckets[i] = nullptr;
	}
	m_count = 0;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_count;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	unsigned int bucketNum = getBucketNumber(key, m_numBuckets);
	ValueType* val = find(key);
	if (find(key)) {
		*val =  value;
		return;
	}

	double loadFactor = (double)m_count / m_numBuckets;
	if (loadFactor >= m_loadFactor) { //check if need to resize hashmap
		Bucket** temp = new Bucket * [m_numBuckets * 2];
		for (int i = 0; i < m_numBuckets * 2; i++) {
			temp[i] = nullptr;
		}		
		for (int i = 0; i < m_numBuckets; i++) { //copying current hashmap into new hashmap
			while (m_buckets[i] != nullptr) {
				//bucketNum = hash(current->m_key);
				bucketNum = getBucketNumber(m_buckets[i]->m_key, m_numBuckets * 2);
				//delete above
				if (temp[bucketNum] == nullptr) { //check for no collision
					temp[bucketNum] = new Bucket;
					temp[bucketNum]->m_key = m_buckets[i]->m_key;
					temp[bucketNum]->m_value = m_buckets[i]->m_value;
					temp[bucketNum]->m_next = nullptr;
				}
				else { //otherwise link bucket to the last bucket of list
					Bucket* iterator = temp[bucketNum];
					while (iterator->m_next != nullptr) {
						iterator = iterator->m_next;
					}
					iterator->m_next = new Bucket;
					iterator->m_next->m_key = m_buckets[i]->m_key;
					iterator->m_next->m_value = m_buckets[i]->m_value;
					iterator->m_next->m_next = nullptr;
				}
				Bucket* next = m_buckets[i]->m_next;
				delete m_buckets[i];
				m_buckets[i] = next;
			}
		}
		m_numBuckets = m_numBuckets * 2;
		delete m_buckets;
		m_buckets = temp;
	}
	bucketNum = getBucketNumber(key, m_numBuckets);
	Bucket* current = m_buckets[bucketNum];
	//inserting new bucket
	if (m_buckets[bucketNum] == nullptr) {
		m_buckets[bucketNum] = new Bucket;
		m_buckets[bucketNum]->m_key = key;
		m_buckets[bucketNum]->m_value = value;
		m_buckets[bucketNum]->m_next = nullptr;
	}

	else {	
		while (current->m_next != nullptr) {
			current = current->m_next;
		}
		current->m_next = new Bucket;
		current->m_next->m_key = key;
		current->m_next->m_value = value;
		current->m_next->m_next = nullptr;
	}
	m_count++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	unsigned int bucketNum = getBucketNumber(key, m_numBuckets);
	Bucket* current = m_buckets[bucketNum];
	while (current != nullptr) {
		if (current->m_key == key) {
			return &current->m_value;
		}
		current = current->m_next;
	}
	return nullptr;  
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::clearMap() {
	Bucket* head;
	Bucket* temp;
	for (int i = 0; i < m_numBuckets; i++) {
		head = m_buckets[i];
		while (head != nullptr) {
			temp = head;
			head = head->m_next;
			delete temp;
		}
	}
	delete[] m_buckets;
}
template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::getBucketNumber(const KeyType& key, int numBuckets) const {
	unsigned int hash(const KeyType &key);
	unsigned int h = hash(key);
	return h % numBuckets;
}
