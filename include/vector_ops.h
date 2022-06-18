#pragma once
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <stdexcept>

/****************************
*   Vector initialization   *
****************************/

// Source: https://stackoverrun.com/es/q/3539074#30312659
template <typename IntType> std::vector<IntType> range(IntType start, IntType stop, IntType step)
{
    if (step == IntType(0))
    {
        throw std::invalid_argument("step for range must be non-zero");
    }

    std::vector<IntType> result;
    IntType i = start;
    while ((step > 0) ? (i < stop) : (i > stop))
    {
        result.push_back(i);
        i += step;
    }

    return result;
}

template <typename IntType> std::vector<IntType> range(IntType start, IntType stop)
{
    return range(start, stop, IntType(1));
}

template <typename IntType> std::vector<IntType> range(IntType stop)
{
    return range(IntType(0), stop, IntType(1));
}

/****************************
*  Vector sets operations   *
****************************/

template<typename Iter, typename RandomGenerator> Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter> Iter select_randomly(Iter start, Iter end)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

// Source: https://stackoverflow.com/a/3487814
template<typename T> void delete_indexes(std::vector<T>& data, const std::vector<unsigned>& deleteIndices)
{
    std::vector<bool> markedElements(data.size(), false);
    std::vector<T> tempBuffer;
    tempBuffer.reserve(data.size() - deleteIndices.size());

    for (unsigned int deleteIndice : deleteIndices)
        markedElements[deleteIndice] = true;

    for (size_t i = 0; i < data.size(); i++)
    {
        if (!markedElements[i])
            tempBuffer.push_back(data[i]);
    }
    data = tempBuffer;
}

// Source: https://www.techiedelight.com/check-vector-contains-given-element-cpp/
template<typename T> bool vector_contains_q(const std::vector<T>& v, const T& key)
{
    return std::find(v.begin(), v.end(), key) != v.end();
}

template<typename T> int find_index_of(const std::vector<T>& v, const T& key)
{
    auto it = std::find(v.begin(), v.end(), key);

    if (it != v.end())
    {
        int index = static_cast<int>(std::distance(v.begin(), it));
        return index;
    }
    else
        return -1;
}

template<typename T> T* find_first_of(const std::vector<T>& v, const T& key)
{
    auto it = std::find(v.begin(), v.end(), key);
    if (it != v.end())
        return it;
    else
        return nullptr;
}