#include <catch2/catch_all.hpp>

#include "lib/Distance.h"
#include <random>
#include <array>
#include <tuple>

using namespace math;

constexpr const double Prec = 0.00003;

#define DEQ(a,b) a

struct Rand {
    constexpr static const double min = -100;
    constexpr static const double max = 100;
    inline static std::uniform_real_distribution<double> unif{min, max};
    inline static std::default_random_engine re{};

    static double RandomDouble()
    {
        double random_double = unif(re);
        return random_double;
    }
};

template<size_t Size>
std::array<std::tuple<structures::circle<double>, DPackedVector512>, Size> GenRandData() {
    std::array<std::tuple<structures::circle<double>, DPackedVector512>, Size> res;
    for(int i = 0; i < Size; i++)
    {
        std::get<0>(res[i]).x = Rand::RandomDouble();
        std::get<0>(res[i]).y = Rand::RandomDouble();
        std::get<0>(res[i]).r = Rand::RandomDouble();
        for(int j = 0; j < 8; j++) {
            std::get<1>(res[i])[j] = Rand::RandomDouble();
        }
        return res;
    }
}

constexpr const size_t sz = 10000;

TEST_CASE("cpp")
{
    structures::circle<double> c1{0, 0, 1};
    DPackedVector512 testarray {0, 0, 3, 4, 12, 5, 4, 0};

    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[0] == -1);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[1] == 4);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[2] == 12);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[3] == 3);

    testarray = {0, 1, 12, 16, 6, 8, 11, 0};
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[0] == 0);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[1] == 19);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[2] == 9);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[3] == 10);

    c1.x = 3;
    c1.y = 1;
    c1.r = 5;
    testarray = {3, 9, 9, -7, 0, -3, -27, -39};
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[0] == 3);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[1] == 5);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[2] == 0);
    CHECK(cpp::DistFromPointToCirclePack(c1, testarray)[3] == 45);
}

TEST_CASE("avx") {
    structures::circle<double> c1{0, 0, 1};
    DPackedVector512 testarray {0, 0, 3, 4, 12, 5, 4, 0};

    CHECK(avx::DistFromPointToCirclePack(c1, testarray)[0] == -1);
    CHECK(avx::DistFromPointToCirclePack(c1, testarray)[1] == 4);
    CHECK(avx::DistFromPointToCirclePack(c1, testarray)[2] == 12);
    CHECK(avx::DistFromPointToCirclePack(c1, testarray)[3] == 3);
}

TEST_CASE("avx random")
{
    auto array = GenRandData<sz>();
    for (size_t i = 0; i < sz; i++) {
        for (size_t j = 0; j < 4; j++) {
        CHECK(cpp::DistFromPointToCirclePack(std::get<0>(array[i % sz]), std::get<1>(array[i % sz]))[j] ==
            avx::DistFromPointToCirclePack(std::get<0>(array[i % sz]), std::get<1>(array[i % sz]))[j]);
        }
    }
}