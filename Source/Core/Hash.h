#pragma once
#include <type_traits>
#include <functional>

struct Hash
{
    Hash()
        : hash_(0)
    {
    }

    Hash(const Hash& other)
        : hash_(other.GetValue())
    {
    }

    template<class T>
    typename std::enable_if<std::is_same<T, Hash>::value || std::is_base_of<Hash, T>::value>::type
    Add(const T& other_hash)
    {
        Hash::HashCombine(hash_, other_hash.GetValue());
    }

    template<class T>
    typename std::enable_if<std::is_fundamental<T>::value || std::is_same<T, std::string>::value>::type
        Add(const T& value)
    {
        Hash::HashCombine(hash_, value);
    }

    size_t GetValue() const
    {
        return hash_;
    }

    // Convenient hash enable - See https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    inline static void HashCombine(std::size_t& seed) {}

    template <typename T, typename... Rest>
    inline static void HashCombine(std::size_t& seed, const T& v, Rest... rest)
    {
        seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        HashCombine(seed, rest...);
    }

private:
    std::size_t hash_;
 };

#define MAKE_HASHABLE(Type, ...) \
    namespace std {\
        template<> struct hash<Type> {\
            std::size_t operator()(const Type& t) const {\
                std::size_t ret = 0;\
                Hash::HashCombine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }
