#pragma once
#include <functional>
#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>


template<typename T, typename... Others>
void HashCombineImpl(size_t& seed, const T& v, const Others&... others)
{
    seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
    (HashCombineImpl(seed, others), ...);
}

namespace std
{
    template<>
    struct hash<boost::uuids::uuid>
    {
        size_t operator() (const boost::uuids::uuid& uid) noexcept
        {
            return boost::hash<boost::uuids::uuid>()(uid);
        }
    };
}

namespace std
{
    template<typename T>
    struct hash<std::vector<T>>
    {
        size_t operator() (const std::vector<T>& v) noexcept
        {
            size_t hash = 0;
            for (auto& item : v)
                HashCombineImpl(hash, item);
            return hash;
        }
    };
}

template <typename T, typename... Others>
size_t HashCombine(const T& v, const Others&... others)
{
    size_t hash = std::hash<T>{}(v);
    HashCombineImpl(hash, others...);
    return hash;
}