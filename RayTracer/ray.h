#pragma once

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
	ray() {} //default constructor
	ray(const point3& origin, const vec3& direction, const double time = 0.0) { //point 3 is alias of vec3
		orig = origin;
		dir = direction;
		tm = time;
	}

	point3 origin() const { return orig; }
	vec3 direction() const { return dir; }
	double time() const { return tm; }

	point3 at(double t) const { //defining function at()
		return orig + t * dir;
	}

public:
	point3 orig;
	vec3 dir;
	double tm;

};

#endif