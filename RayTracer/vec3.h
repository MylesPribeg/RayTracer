#pragma once //for VS
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {

public:
	vec3() : e{ 0,0,0 } {}//default constructor
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}//non-default constructor
	//getters
	double x() const {
		return e[0];
	}
	double y() const {
		return e[1];
	}
	double z() const {
		return e[2];
	}

	//operator definitions for class
	//these operators are member functions because things like /= or += modify
	//the left operand (i.e. add to it) so they need access to the private parts
	vec3 operator-() const {
		return vec3(-e[0], -e[1], -e[2]);
	}
	

	//There are two variants of this operator so that the elements returned can be changed
	double operator[](int i) const { return e[i]; } //This one returns a copy of the double 
													//(because it is a built in type, otherwise 
													//these two should be written differently)
	double& operator[](int i) { return e[i]; }		

	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];

		return *this; //returns reference of itself
	}

	vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
	}

	vec3& operator/=(const double t) {
		return *this *= 1 / t; //uses previously overloaded *= operator
	}
	
	//other functions

	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	double length() const {
		return sqrt(length_squared());

	}

	//for choosing random point in unit sphere
	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	bool near_zero() const {
		//Return true if the vector is close to zero in all dimensions
		const auto s = 1e-8;
		return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s); //fabs = absolute value
	}

public: //Could be removed, access label can appear mutiple times, doesn't do anything here since last one was "public"
	double e[3];

};

//Type aliases for vec3

using point3 = vec3; //3D point
using color = vec3;  //RGB colour

#endif

//more utility functions/operator overloads
//the operator overloads here are non-member functions because they aren't = (assignment),
//[] (array subcription), -> (member access), or () (function call) operators.

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
	//TRY THIS WITH JUST u[0] * v[0], etc. it should work because that operator is defined
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v; //already defined
}

inline vec3 operator/(const vec3& v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u[0] * v[0] 
		+ u[1] * v[1] 
		+ u[2] * v[2];
}

inline vec3 cross(const vec3& u, vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

point3 random_in_unit_sphere() {
	while (1) {
		auto p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

inline vec3 random_unit_vector() {
	return unit_vector(random_in_unit_sphere());
}

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, double rior) {//ratio of index of refractions "roir"
	auto cos_theta = fmin(dot(-uv, n), 1.0);
	vec3 r_out_perp = rior * (uv + cos_theta * n);
	vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
	return r_out_perp + r_out_parallel;
}