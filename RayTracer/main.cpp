#include "raytracer.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>


// A NEW MAIN PROGRAM TO EXPLORE MONTE CARLO ALOGRITHMS IN MORE DEPTH

double pdf(vec3& x) {
	return 1/(4*pi);
}

int main() {
	int N = 1000000;
	auto sum = 0.0;

	for (int i = 0; i < N; i++) {
		vec3 d = random_unit_vector();
		auto cosine_squared = d.z() * d.z();
		sum += cosine_squared / pdf(d);  //pdf is constant 

	}

	std::cout << std::fixed << std::setprecision(12);

	std::cout << "I = " << sum / N << std::endl; // from Mean Value Theorem
	// integral(x) from a to b = f(c) * (b - a) where f(c) is the average value


	return 0;
}
