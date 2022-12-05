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
template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop, IntType step)
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

template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop)
{
    return range(start, stop, IntType(1));
}

template <typename IntType>
std::vector<IntType> range(IntType stop)
{
    return range(IntType(0), stop, IntType(1));
}

/****************************
*  Vector sets operations   *
****************************/

template<typename Iter, typename RandomGenerator>
Iter random_choice(Iter start, Iter end, RandomGenerator& g)
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter random_choice(Iter start, Iter end)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return random_choice(start, end, gen);
}

template<typename T>
void shuffle(std::vector<T>& data)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(std::begin(data), std::end(data), gen);
}

// Source: https://stackoverflow.com/a/57616877
inline int pow2(int n)
{
    return 1 << (n);
}
template<typename RandomGenerator>
std::vector<size_t> random_weighted_sample_indexes(const std::vector<int>& weights, size_t ntake, RandomGenerator& g)
{
    size_t rnd_max = weights.size();

    /* determine the smallest power of two that is larger than N */
    int tree_levels = static_cast<int>(ceil(log2((double)rnd_max)));

    /* initialize vector with place-holders for perfectly-balanced tree */
    std::vector<double> tree_weights(pow2(tree_levels + 1));

    /* compute sums for the tree leaves at each node */
    int offset = pow2(tree_levels) - 1;
    for (size_t ix = 0; ix < rnd_max; ix++)
    {
        tree_weights[ix + offset] = weights[ix];
    }
    for (size_t ix = pow2(tree_levels + 1) - 1; ix > 0; ix--)
    {
        tree_weights[(ix - 1) / 2] += tree_weights[ix];
    }

    /* sample according to uniform distribution */
    double rnd_subrange, w_left;
    double curr_subrange;
    size_t curr_ix;
    std::vector<size_t> sampled(ntake);
    for (size_t el = 0; el < ntake; el++)
    {
        /* go down the tree by drawing a random number and
           checking if it falls in the left or right sub-ranges */
        curr_ix = 0;
        curr_subrange = tree_weights[0];

        for (int lev = 0; lev < tree_levels; lev++)
        {
            rnd_subrange = std::uniform_real_distribution<double>(0, curr_subrange)(g);
            w_left = tree_weights[2 * curr_ix + 1];
            curr_ix = 2 * curr_ix + 1 + (rnd_subrange >= w_left);
            curr_subrange = tree_weights[curr_ix];
        }

        /* finally, add element from this iteration */
        sampled[el] = curr_ix - offset;

        /* now remove the weight of the chosen element */
        tree_weights[curr_ix] = 0;
        for (int lev = 0; lev < tree_levels; lev++)
        {
            curr_ix = (curr_ix - 1) / 2;
            tree_weights[curr_ix] = tree_weights[2 * curr_ix + 1] + tree_weights[2 * curr_ix + 2];
        }
    }

    return sampled;
}

inline std::vector<size_t> random_weighted_sample_indexes(const std::vector<int>& weights, int ntake)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return random_weighted_sample_indexes(weights, ntake, gen);
}

template<typename T>
std::vector<T> extract_elements_at_indexes(const std::vector<T>& data, const std::vector<size_t>& extractIndexes)
{
    std::vector<T> extracted;
    for (size_t extractIndex : extractIndexes)
        extracted.push_back(data[extractIndex]);

    return extracted;
}

// Source: https://stackoverflow.com/a/3487814
template<typename T>
void delete_elements_at_indexes(std::vector<T>& data, const std::vector<size_t>& deleteIndexes)
{
    std::vector<bool> markedElements(data.size(), false);
    std::vector<T> tempBuffer;
    tempBuffer.reserve(data.size() - deleteIndexes.size());

    for (size_t deleteIndex : deleteIndexes)
        markedElements[deleteIndex] = true;

    for (size_t i = 0; i < data.size(); i++)
    {
        if (!markedElements[i])
            tempBuffer.push_back(data[i]);
    }
    data = tempBuffer;
}

// Source: https://www.techiedelight.com/check-vector-contains-given-element-cpp/
template<typename T>
bool vector_contains_q(const std::vector<T>& v, const T& key)
{
    return std::find(v.begin(), v.end(), key) != v.end();
}

template<typename T, class UnaryPredicate>
std::vector<size_t> find_indexes_if(const std::vector<T>& v, UnaryPredicate pred)
{
    std::vector<size_t> indexes;
    auto it = v.begin();

    while ((it = std::find_if(it, v.end(), pred)) != v.end())
    {
        indexes.push_back(std::distance(v.begin(), it));
        it++;
    }

    return indexes;
}

/****************************
*     Vector conversions    *
****************************/

template<typename T>
T vector_to_string(const std::vector<T>& v)
{
    std::string s = "[";
    for (auto& elem : v)
    {
        size_t i = &elem - &v[0];
        s += "'" + elem + "'";
        if (i != v.size() - 1)
            s += ",";
    }
    s += "]";
    return s;
}