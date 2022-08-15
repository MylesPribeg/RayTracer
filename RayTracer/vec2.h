#pragma once

#ifndef VEC2_H
#define VEC2_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec2 {

public:
	vec2() : e{ 0,0 } {}//default constructor
	vec2(double e0, double e1) : e{ e0, e1 } {}//non-default constructor
	//getters
	double x() const {
		return e[0];
	}
	double y() const {
		return e[1];
	}


	vec2 operator-() const {
		return vec2(-e[0], -e[1]);
	}


	//There are two variants of this operator so that the elements returned can be changed
	double operator[](int i) const { return e[i]; } 
	double& operator[](int i) { return e[i]; }

	vec2& operator+=(const vec2& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];

		return *this;
	}

	vec2& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;

		return *this;
	}

	vec2& operator/=(const double t) {
		return *this *= 1 / t; //uses previously overloaded *= operator
	}

	//other functions

	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1];
	}

	double length() const {
		return sqrt(length_squared());

	}

	inline static vec2 random(double min, double max) {
		return vec2(random_double(min, max), random_double(min, max));
	}

	bool near_zero() const {
		//Return true if the vector is close to zero in all dimensions
		const auto s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s); //fabs = absolute value
	}

public:
	double e[2];

};

//Type aliases for vec2

using point2 = vec2; //3D point

#endif

//more utility functions/operator overloads
//the operator overloads here are non-member functions because they aren't = (assignment),
//[] (array subcription), -> (member access), or () (function call) operators.

inline std::ostream& operator<<(std::ostream& out, const vec2& v) {
	return out << v.e[0] << ' ' << v.e[1];
}

inline vec2 operator+(const vec2& u, const vec2& v) {
	return vec2(u.e[0] + v.e[0], u.e[1] + v.e[1]);
}

inline vec2 operator-(const vec2& u, const vec2& v) {
	return vec2(u.e[0] - v.e[0], u.e[1] - v.e[1]);
}

inline vec2 operator*(const vec2& u, const vec2& v) {
	return vec2(u.e[0] * v.e[0], u.e[1] * v.e[1]);
	//TRY THIS WITH JUST u[0] * v[0], etc. it should work because that operator is defined
}

inline vec2 operator*(double t, const vec2& v) {
	return vec2(t * v.e[0], t * v.e[1]);
}

inline vec2 operator*(const vec2& v, double t) {
	return t * v; //already defined
}

inline vec2 operator/(const vec2& v, double t) {
	return (1 / t) * v;
}


inline vec2 unit_vector(vec2 v) {
	return v / v.length();
}