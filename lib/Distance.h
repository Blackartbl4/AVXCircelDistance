#pragma once

#include <array>
#include <iostream>
#include <cmath>
#include <immintrin.h>
#include <emmintrin.h>
#include "ARCH.h"

namespace math
{
	namespace structures
	{
		template <typename T, size_t size, size_t _align = alignof(T) * 8ULL>
			requires(_align >= alignof(T) / 8 && _align / 8 <= size * sizeof(T))
		struct alignas(_align / 8) align_array : public std::array<T, size> {};

		template <typename T>
        struct circle {
        public:
            T x{0}, y{0}, r{0};

            constexpr circle() = default;
            constexpr circle(T&& x, T&& y, T&& r) : x(std::forward(x)), y(std::forward(y)), r(std::forward(r)) {}

            constexpr circle(const circle& other) : x(other.x), y(other.y), r(other.r) {}
            constexpr circle(circle&& other) : x(std::move(x)), y(std::move(y)), r(std::move(r)) {}

            constexpr circle& operator=(const circle& other) {
                x = other.x; y = other.y; r = other.r;
            }
            constexpr circle& operator=(circle&& other) {
                x = std::move(x);
                y = std::move(y);
                r = std::move(r);
            }
        };
	}

	using DPackedVector512 = structures::align_array<double, 8, 512>;
	using DPackedVector256 = structures::align_array<double, 4, 256>;
	using DPackedVector128 = structures::align_array<double, 2, 128>;
	using DPackedScalar512 = structures::align_array<double, 8, 512>;
	using DPackedScalar256 = structures::align_array<double, 4, 256>;
	using DPackedScalar128 = structures::align_array<double, 2, 128>;
	using DVector = structures::align_array<double, 2, 128>;
	using DScalar = double;

	using DCircle = structures::circle<DScalar>;

	using FPackedVector256 = structures::align_array<float, 8, 256>;
	using FPackedVector128 = structures::align_array<float, 8, 128>;
	using FPackedScalar128 = structures::align_array<float, 4, 128>;
	using FVector = structures::align_array<float, 2, 64>;
	using FScalar = float;

	using FCircle = structures::circle<FScalar>;

	using __DFPTCP_ptr = DPackedScalar256 (*)(DCircle, DPackedVector512);

	namespace cpp
	{
		DPackedScalar256 DistFromPointToCirclePack(DCircle c, DPackedVector512 circles)
		{
			DPackedScalar256 distances;
			for (int i = 0; i < 4; i++)
			{
				int ci = 2 * i;
				distances[i] = sqrt((circles[ci] - c.x) * (circles[ci] - c.x) + (circles[ci + 1] - c.y) * (circles[ci + 1] - c.y)) - c.r;
			}
			return distances;
		}
	}
	namespace avx512
	{
		template <bool __MAVX = arch::ARCH_INFO::AVX512F>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(DCircle c, DPackedVector512 circles)
		{
			__m512d quadropoint = _mm512_set4_pd(c.x, c.y, c.x, c.y);
			__m512d centralpoints = _mm512_load_pd(&circles[0]);
			centralpoints = _mm512_sub_pd(centralpoints, quadropoint);
			centralpoints = _mm512_mul_pd(centralpoints, centralpoints); // по квадратам
			__m512d ukcb1 = _mm512_movedup_pd(centralpoints);
			__mmask8 mask = 170;
			centralpoints = _mm512_maskz_add_pd(mask, centralpoints, ukcb1);
			__m512d res = _mm512_set4_pd(0, c.r, 0, c.r);
			centralpoints = _mm512_maskz_sqrt_pd(mask, centralpoints);
			centralpoints = _mm512_maskz_sub_pd(mask, centralpoints, res);
			_mm512_storeu_pd(&circles[0], centralpoints);
			return {circles[1], circles[3], circles[5], circles[7]};
		}
	}
	namespace avx
	{
		DPackedScalar128 __DistFromPointToCirclePack(DCircle c, DPackedVector256 circles)
		{
			__m256d dabloopoint = _mm256_set_pd(c.y, c.x, c.y, c.x);
			__m256d centralpoints = _mm256_load_pd(&circles[0]);
			centralpoints = _mm256_sub_pd(centralpoints, dabloopoint);
			centralpoints = _mm256_mul_pd(centralpoints, centralpoints); // по квадратам
			__m256d nuli = _mm256_set_pd(0, 0, 0, 0);
			centralpoints = _mm256_hadd_pd(centralpoints, nuli); // 2 square distances
			//__m128d distances = _mm256_castpd256_pd128(centralpoints);
			// distances = _mm_sqrt_pd(distances);
			__m256d res = _mm256_set_pd(0, c.r, 0, c.r);
			centralpoints = _mm256_sqrt_pd(centralpoints);
			centralpoints = _mm256_sub_pd(centralpoints, res);
			_mm256_storeu_pd(&circles[0], centralpoints);
			return {circles[0], circles[2]};
		}

		template <bool __MAVX = arch::ARCH_INFO::AVX>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(DCircle c, DPackedVector512 circles)
		{
			DPackedScalar256 res;
			std::array<double, 2> tmp = __DistFromPointToCirclePack(c, {circles[0], circles[1], circles[2], circles[3]});
			res[0] = tmp[0];
			res[1] = tmp[1];
			tmp = __DistFromPointToCirclePack(c, {circles[4], circles[5], circles[6], circles[7]});
			res[2] = tmp[0];
			res[3] = tmp[1];
			return res;
		}

		FPackedScalar128 DistancesToCircle(FCircle c, FPackedVector256 points)
		{
			__m256 pts = _mm256_load_ps(points.data());
			__m256 xy = _mm256_set_ps(c.y, c.x, c.y, c.x, c.y, c.x, c.y, c.x);
			pts = _mm256_sub_ps(pts, xy);
			pts = _mm256_mul_ps(pts, pts);
			pts = _mm256_hadd_ps(pts, pts);
			pts = _mm256_sqrt_ps(pts);
			__m256 rs = _mm256_set1_ps(c.r);
			pts = _mm256_sub_ps(pts, rs);
			_mm256_store_ps(points.data(), pts);
			return {points[0], points[1], points[4], points[5]};
		}

		FPackedScalar128 SquareDistancesToCircle(FCircle c, FPackedVector256 points)
		{
			__m256 pts = _mm256_load_ps(points.data());
			__m256 xy = _mm256_set_ps(c.y, c.x, c.y, c.x, c.y, c.x, c.y, c.x);
			pts = _mm256_sub_ps(pts, xy);
			pts = _mm256_mul_ps(pts, pts);
			pts = _mm256_hadd_ps(pts, pts);
			pts = _mm256_sqrt_ps(pts);
			__m256 rs = _mm256_set1_ps(c.r);
			pts = _mm256_sub_ps(pts, rs);
			pts = _mm256_mul_ps(pts, pts);
			_mm256_store_ps(points.data(), pts);
			return {points[0], points[2], points[4], points[6]};
		}
	}
	namespace sse3
	{
		template <bool __MAVX = arch::ARCH_INFO::SSE2 && arch::ARCH_INFO::SSE3>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(DCircle c, DPackedVector512 circles)
		{
			DPackedScalar256 res;
			__m128d xy = _mm_set_pd(c.y, c.x);
			__m128d rr = _mm_set1_pd(c.r);
			__m128d circla = _mm_load_pd(&circles[0]);
			__m128d circlb = _mm_load_pd(&circles[2]);
			circla = _mm_sub_pd(circla, xy);
			circlb = _mm_sub_pd(circlb, xy);
			circla = _mm_mul_pd(circla, circla);
			circlb = _mm_mul_pd(circlb, circlb);
			circla = _mm_hadd_pd(circla, circlb); // SSE3
			circla = _mm_sqrt_pd(circla);
			circla = _mm_sub_pd(circla, rr);
			_mm_store_pd(&res[0], circla);
			circla = _mm_load_pd(&circles[4]);
			circlb = _mm_load_pd(&circles[6]);
			circla = _mm_sub_pd(circla, xy);
			circlb = _mm_sub_pd(circlb, xy);
			circla = _mm_mul_pd(circla, circla);
			circlb = _mm_mul_pd(circlb, circlb);
			circla = _mm_hadd_pd(circla, circlb); // SSE3
			circla = _mm_sqrt_pd(circla);
			circla = _mm_sub_pd(circla, rr);
			_mm_store_pd(&res[4], circla);
			return res;
		}
	}

	template <size_t AVXFlags = arch::ARCH_INFO::SIMDFlag>
	struct __Releaze
	{
	public:
		constexpr static const char *TargetName()
		{
			if constexpr ((AVXFlags & 32ULL) == 32ULL)
				return "AVX512F-Rel";
			else if constexpr ((AVXFlags & 8ULL) == 8ULL)
				return "AVX-Rel";
			else if constexpr ((AVXFlags & 6ULL) == 6ULL)
				return "SSE3-Rel";
			return "CPP-Rel";
		}
		constexpr static __DFPTCP_ptr TargetDistanceFunction()
		{
			if constexpr ((AVXFlags & 32ULL) == 32ULL)
				return (__DFPTCP_ptr)avx512::DistFromPointToCirclePack;
			else if constexpr ((AVXFlags & 8ULL) == 8ULL)
				return (__DFPTCP_ptr)avx::DistFromPointToCirclePack;
			else if constexpr ((AVXFlags & 6ULL) == 6ULL)
				return (__DFPTCP_ptr)sse3::DistFromPointToCirclePack;
			return (__DFPTCP_ptr)cpp::DistFromPointToCirclePack;
		}
	};

	using ReleazeInfo = __Releaze<>;

	constexpr const __DFPTCP_ptr DistFromPointToCirclePack = ReleazeInfo::TargetDistanceFunction();
}