#pragma once
#include <numeric>
#include <random>

class RandomGen
{
    static std::random_device rd;
    static std::mt19937 gen;

public:
    static void Seed()
    {
        gen.seed(rd());
    }
    static void Seed(int n)
    {
        gen.seed(n);
    }
    template<typename Iter> static Iter SelectRandomly(Iter start, Iter end)
    {
        // Source: https://stackoverflow.com/a/16421677
        const int distance = static_cast<int>(std::distance(start, end));
        std::uniform_int_distribution<> dis(0, distance - 1);
        std::advance(start, dis(gen));
        return start;
    }
};

std::random_device RandomGen::rd;
std::mt19937 RandomGen::gen(rd());