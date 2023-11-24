#pragma once
#include <functional>
#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>


//template<typename T, typename... Others>
//void HashCombineImpl(size_t& seed, const T& v, const Others&... others)
//{
//    (HashCombineImpl(seed, others), ...);
//}

template <typename T, typename... Others>
size_t HashCombine(const T& v, const Others&... others)
{
    size_t hash = boost::hash<T>{}(v);
    //HashCombineImpl(hash, others...);
    (boost::hash_combine(hash, others), ...);
    return hash;
}