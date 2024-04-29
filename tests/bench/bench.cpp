#include <catch2/catch_all.hpp>

#include "lib/Distance.h"
#include <random>
#include <array>
#include <tuple>

double min = -100;
double max = 100;

std::uniform_real_distribution<double> unif(min, max);

std::default_random_engine re{};

double Rand()
{
    double random_double = unif(re);
    return random_double;
}

template<size_t Size>
std::array<std::tuple<math::structures::circle<double>, math::DPackedVector512>, Size> GenRandData() {
    std::array<std::tuple<math::structures::circle<double>, math::DPackedVector512>, Size> res;
    for(int i = 0; i < Size; i++)
    {
        std::get<0>(res[i]).x = Rand();
        std::get<0>(res[i]).y = Rand();
        std::get<0>(res[i]).r = Rand();
        for(int j = 0; j < 8; j++) {
            std::get<1>(res[i])[j] = Rand();
        }
    }
}

math::structures::circle<double> c1{0, 0, 1};
math::DPackedVector512 testarray {0, 0, 3, 4, 12, 5, 4, 0};

constexpr const size_t sz = 10000;

TEST_CASE("cpp")
{
    CHECK(math::cpp::DistFromPointToCirclePack(c1, testarray)[0] == -1);
    CHECK(math::cpp::DistFromPointToCirclePack(c1, testarray)[1] == 4);
    CHECK(math::cpp::DistFromPointToCirclePack(c1, testarray)[2] == 12);
    CHECK(math::cpp::DistFromPointToCirclePack(c1, testarray)[3] == 3);

    BENCHMARK("not random")
    {
        return math::cpp::DistFromPointToCirclePack(c1, testarray);
    };

    BENCHMARK_ADVANCED("random")
    (Catch::Benchmark::Chronometer meter)
    {
        auto data = GenRandData<sz>();
        meter.measure([&](int i)
                      { return math::cpp::DistFromPointToCirclePack(std::get<0>(data[i % sz]), std::get<1>(data[i % sz])); });
    };
}

TEST_CASE("avx")
{
    CHECK(math::avx::DistFromPointToCirclePack(c1, testarray)[0] == 
        math::cpp::DistFromPointToCirclePack(c1, testarray)[0]);
    CHECK(math::avx::DistFromPointToCirclePack(c1, testarray)[1] == 
        math::cpp::DistFromPointToCirclePack(c1, testarray)[1]);
    CHECK(math::avx::DistFromPointToCirclePack(c1, testarray)[2] == 
        math::cpp::DistFromPointToCirclePack(c1, testarray)[2]);
    CHECK(math::avx::DistFromPointToCirclePack(c1, testarray)[3] == 
        math::cpp::DistFromPointToCirclePack(c1, testarray)[3]);
    auto array = GenRandData<sz>();
    for (size_t i = 0; i < sz; i++) {
        for (size_t j = 0; j < 4; j++) {
        CHECK(math::cpp::DistFromPointToCirclePack(std::get<0>(array[i % sz]), std::get<1>(array[i % sz]))[j] ==
            math::avx::DistFromPointToCirclePack(std::get<0>(array[i % sz]), std::get<1>(array[i % sz]))[j]);
        }
    }

    BENCHMARK("not random")
    {
        return math::avx::DistFromPointToCirclePack(c1, testarray);
    };

    BENCHMARK_ADVANCED("random")
    (Catch::Benchmark::Chronometer meter)
    {
        auto data = GenRandData<sz>();
        meter.measure([&](int i)
                      { return math::avx::DistFromPointToCirclePack(std::get<0>(data[i % sz]), std::get<1>(data[i % sz])); });
    };
}