#include "raytracer.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>


// A NEW MAIN PROGRAM TO EXPLORE MONTE CARLO ALOGRITHMS IN MORE DEPTH

double pdf(double x) {
	return 0.5 * x;
}

int main() {
	int N = 100000;
	auto sum = 0.0;
	auto upper = 4;
	auto lower = 0.000001;

	for (int i = 0; i < N; i++) {

		auto x = sqrt(random_double(lower, upper));
		sum += x*x/pdf(x);  //sum += 2 * x^2 / x

	}

	auto range = upper - lower;

	std::cout << std::fixed << std::setprecision(12);

	std::cout << "I = " << /*range * */ sum / N << std::endl; // from Mean Value Theorem
	// integral(x) from a to b = f(c) * (b - a) where f(c) is the average value


	return 0;
}
