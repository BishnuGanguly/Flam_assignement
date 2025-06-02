
#include<bits/stdc++.h>

using namespace std;

class LRUCache {
private:
    int capacity;
    std::list<std::pair<int, int>> items_list; 
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> items_map;

public:
    LRUCache(int capacity) {
        this->capacity = capacity;
    }

    int get(int key) {
        auto it = items_map.find(key);
        
        if (it == items_map.end()) {
            return -1; 
        }
        
        // Move accessed item to the front of the list
        items_list.splice(items_list.begin(), items_list, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = items_map.find(key);
        
        if (it != items_map.end()) {
            // Key exists, update value and move to front
            it->second->second = value;
            items_list.splice(items_list.begin(), items_list, it->second);
        } else {
            // Key does not exist, insert new item
            if (items_list.size() >= capacity) {
                // Cache is full, evict LRU item
                if (!items_list.empty()) {
                    int lru_key = items_list.back().first;
                    items_map.erase(lru_key);
                    items_list.pop_back();
                }
            }
            
            items_list.push_front({key, value});
            items_map[key] = items_list.begin();
        }
    }
};
 

int main() {
    LRUCache lru(2);
    cout << "LRUCache lru(2);" << endl;

    lru.put(1, 10); 
    cout << "lru.put(1, 10);" << endl;

    lru.put(2, 20); 
    cout << "lru.put(2, 20);" << endl;

    cout << "lru.get(1): " << lru.get(1) <<endl; 

    lru.put(3, 30); 
    cout << "lru.put(3, 30);" <<endl;

    cout << "lru.get(2): " << lru.get(2) << endl; 

    lru.put(4, 40); 
    cout << "lru.put(4, 40);" << endl;

 cout << "lru.get(1): " << lru.get(1) <<endl; 
    cout << "lru.get(3): " << lru.get(3) << endl; 
    cout << "lru.get(4): " << lru.get(4) <<endl; 


    return 0;
}