#ifndef NAMA_CONTAINER_H
#define NAMA_CONTAINER_H

#include "container/tsl/robin_hash.h"
#include "container/tsl/robin_map.h"
#include "container/tsl/robin_set.h"

template<class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<Key, T>>,
    bool StoreHash = false,
    class GrowthPolicy = tsl::rh::power_of_two_growth_policy<2>>
using RobinMap = tsl::robin_map<Key, T, Hash, KeyEqual, Allocator, StoreHash, GrowthPolicy>;

template<class Key,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<Key>,
    bool StoreHash = false,
    class GrowthPolicy = tsl::rh::power_of_two_growth_policy<2>>
using RobinSet = tsl::robin_set<Key, Hash, KeyEqual, Allocator, StoreHash, GrowthPolicy>;


#endif 
