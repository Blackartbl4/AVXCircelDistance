#include "lib/Distance.h"
#include <iostream>
#include <array>

using namespace math;

int main() {
    std::cout << ReleazeInfo::TargetName() << std::endl;
	DPackedVector512 circles;
	DPackedScalar256 distances;
	DCircle c;
	std::cin >> c.x >> c.y >> c.r;
	for (auto& i : circles) {
		std::cin >> i;
	}
	distances = DistFromPointToCirclePack(c, circles);
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