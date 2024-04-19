#include"Distance.h"
#include<iostream>
#include<array>

using namespace math;

int main() {
    std::cout << ReleazeInfo::TargetName() << std::endl;
	std::array<double, 8> circles;
	std::array<double, 4> distances;
	double r, x, y;
	std::cin >> r >> x >> y;
	for (auto& i : circles) {
		std::cin >> i;
	}
	distances = avx::DistFromPointToCirclePack(r, circles, x, y);
	for (double d : distances) {
		std::cout << d << " ";
	}
}

/*
3
0
0
5
4
12
5
5
0
0
5

3.40312 10 2 2
*/