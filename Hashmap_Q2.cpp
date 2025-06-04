#include <bits/stdc++.h> 

using namespace std; 

class MyHashMap {
private:
    
    static const int INITIAL_CAPACITY = 10007; 

    // Each bucket is a list (chain) of key-value pairs.
    vector<list<pair<int, int>>> buckets;
    
    // Simple modulo hash function.
    int _calculate_hash(int key) {
        return key % INITIAL_CAPACITY;
    }

public:
    MyHashMap() {
        buckets.resize(INITIAL_CAPACITY);
    }

    void put(int key, int value) {
        int bucket_index = _calculate_hash(key);
        list<pair<int, int>>& chain = buckets[bucket_index];

        // Update if key exists
        for (auto& entry : chain) {
            if (entry.first == key) {
                entry.second = value; 
                return;
            }
        }
        // Add new key-value pair
        chain.push_back({key, value});
    }

    int get(int key) {
        int bucket_index = _calculate_hash(key);
        const list<pair<int, int>>& chain = buckets[bucket_index];

        for (const auto& entry : chain) {
            if (entry.first == key) {
                return entry.second; 
            }
        }
        return -1; // Key not found
    }

    void remove(int key) {
        int bucket_index = _calculate_hash(key);
        list<pair<int, int>>& chain = buckets[bucket_index];

        // Remove key if it exists in the chain.
        chain.remove_if([key](const pair<int, int>& entry) {
            return entry.first == key;
        });
    }
};


int main() {
    cout << "MyHashMap Example (from problem description):" << endl;
    MyHashMap obj; 

    obj.put(1, 10);
    cout << "obj.put(1, 10);" << endl;

    obj.put(2, 20);
    cout << "obj.put(2, 20);" << endl;

    cout << "obj.get(1): " << obj.get(1) << endl;   

    cout << "obj.get(3): " << obj.get(3) << endl;   

    obj.put(2, 30); 
    cout << "obj.put(2, 30);" << endl;

    cout << "obj.get(2): " << obj.get(2) << endl;   

    obj.remove(2);
    cout << "obj.remove(2);" << endl;

    cout << "obj.get(2) after remove: " << obj.get(2) << endl; 
    
    return 0;
}

