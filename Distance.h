#pragma once

#include <array>
#include <iostream>
#include <cmath>
#include <immintrin.h>
#include <emmintrin.h>
#include "ARCH.h"

namespace math
{
	template<typename T, size_t size, size_t _align = alignof(T) * 8ULL> 
        requires (_align >= alignof(T) / 8 && _align / 8 <= size * sizeof(T)) 
            struct alignas(_align / 8) align_array : public std::array<T, size> {};

	using DPackedVector512 = align_array<double, 8, 512>;
	using DPackedVector256 = align_array<double, 4, 256>;
	using DPackedVector128 = align_array<double, 2, 128>;
	using DPackedScalar512 = align_array<double, 8, 512>;
	using DPackedScalar256 = align_array<double, 4, 256>;
	using DPackedScalar128 = align_array<double, 2, 128>;
	using DScalar = double;

	template<typename T, size_t size, size_t _align = alignof(T) * 8ULL> 
        requires (_align >= alignof(T) / 8 && _align / 8 <= size * sizeof(T)) 
            struct alignas(_align / 8) align_array : public std::array<T, size> {};

	using __DFPTCP_ptr = DPackedScalar256 (*)(double, DPackedVector512, double, double);

	namespace cpp
	{
		DPackedScalar256 DistFromPointToCirclePack(double radius, DPackedVector512 circles, double x, double y)
		{
			DPackedScalar256 distances;
			for (int i = 0; i < 4; i++)
			{
				int ci = 2 * i;
				distances[i] = sqrt((circles[ci] - x) * (circles[ci] - x) + (circles[ci + 1] - y) * (circles[ci + 1] - y)) - radius;
			}
			return distances;
		}
	}
	namespace avx512
	{
		template <bool __MAVX = arch::ARCH_INFO::AVX512F>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(double radius, DPackedVector512 circles, double x, double y)
		{
			__m512d quadropoint = _mm512_set4_pd(x, y, x, y);
			__m512d centralpoints = _mm512_load_pd(&circles[0]);
			centralpoints = _mm512_sub_pd(centralpoints, quadropoint);
			centralpoints = _mm512_mul_pd(centralpoints, centralpoints); // по квадратам
			__m512d ukcb1 = _mm512_movedup_pd(centralpoints);
			__mmask8 mask = 170;
			centralpoints = _mm512_maskz_add_pd(mask, centralpoints, ukcb1);
			__m512d radiuses = _mm512_set4_pd(0, radius, 0, radius);
			centralpoints = _mm512_maskz_sqrt_pd(mask, centralpoints);
			centralpoints = _mm512_maskz_sub_pd(mask, centralpoints, radiuses);
			_mm512_storeu_pd(&circles[0], centralpoints);
			return {circles[1], circles[3], circles[5], circles[7]};
		}
	}
	namespace avx
	{
		DPackedScalar128 __DistFromPointToCirclePack(double radius, DPackedVector256 circles, double x, double y)
		{
			__m256d dabloopoint = _mm256_set_pd(y, x, y, x);
			__m256d centralpoints = _mm256_load_pd(&circles[0]);
			centralpoints = _mm256_sub_pd(centralpoints, dabloopoint);
			centralpoints = _mm256_mul_pd(centralpoints, centralpoints); // по квадратам
			__m256d nuli = _mm256_set_pd(0, 0, 0, 0);
			centralpoints = _mm256_hadd_pd(centralpoints, nuli); // 2 square distances
			//__m128d distances = _mm256_castpd256_pd128(centralpoints);
			// distances = _mm_sqrt_pd(distances);
			__m256d radiuses = _mm256_set_pd(0, radius, 0, radius);
			centralpoints = _mm256_sqrt_pd(centralpoints);
			centralpoints = _mm256_sub_pd(centralpoints, radiuses);
			_mm256_storeu_pd(&circles[0], centralpoints);
			return {circles[0], circles[2]};
		}

		template <bool __MAVX = arch::ARCH_INFO::AVX>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(double radius, DPackedVector512 circles, double x, double y)
		{
			DPackedScalar256 res;
			std::array<double, 2> tmp = __DistFromPointToCirclePack(radius, {circles[0], circles[1], circles[2], circles[3]}, x, y);
			res[0] = tmp[0];
			res[1] = tmp[1];
			tmp = __DistFromPointToCirclePack(radius, {circles[4], circles[5], circles[6], circles[7]}, x, y);
			res[2] = tmp[0];
			res[3] = tmp[1];
			return res;
		}
	}
	namespace sse3 {
		template <bool __MAVX = arch::ARCH_INFO::SSE2 && arch::ARCH_INFO::SSE3>
			requires __MAVX
		DPackedScalar256 DistFromPointToCirclePack(double radius, DPackedVector512 circles, double x, double y) {
			DPackedScalar256 res;
			__m128d xy = _mm_set_pd(y, x);
			__m128d rr = _mm_set1_pd(radius);
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

	template<size_t AVXFlags = arch::ARCH_INFO::SIMDFlag>
	struct __Releaze {
		public:
		constexpr static const char* TargetName() {
			if constexpr ((AVXFlags & 32ULL) == 32ULL)
				return "AVX512F-Rel";
			else if constexpr ((AVXFlags & 8ULL) == 8ULL)
				return "AVX-Rel";
			else if constexpr ((AVXFlags & 6ULL) == 6ULL)
				return "SSE3-Rel";
			return "CPP-Rel";
		}
		constexpr static __DFPTCP_ptr TargetDistanceFunction() {
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