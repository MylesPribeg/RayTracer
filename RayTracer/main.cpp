#include "raytracer.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

// A NEW MAIN PROGRAM TO EXPLORE MONTE CARLO ALOGRITHMS IN MORE DEPTH

int main() {
	int N = 100000;
	auto sum = 0.0;

	for (int i = 0; i < N; i++) {

		auto x = random_double(0, 2);
		sum += x * x;

	}

	std::cout << std::fixed << std::setprecision(12);

	std::cout << "I = " << 2 * sum/N << std::endl;


	return 0;
}

