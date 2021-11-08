#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
	sphere() : radius(0){}
	sphere(point3 cen, double r) : center(cen), radius(r) {};

	virtual bool hit(const ray& r, double t_min, double t_max,
		hit_record& t) const override;//"override" to make sure it's actually overriding something

public:
	point3 center;
	double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {//member function, but definted outside
																				//class using ::

	vec3 oc = r.origin() - center; //recall sphere form is: t^2b (dot) b + 2tb (dot) (A - C) + (A - C) (dot)(A - C) - r^2 = 0
	//since A, C, b, and r are known, it becomes a quadratic eqn with t unknown

	auto a = r.direction().length_squared();//vector dotted with itself is its length^2
	auto half_b = dot(oc, r.direction());	//some simplification of quadratic formula since b has a factor of 2
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant < 0) {
		return false;
	}
	auto sqrtd = sqrt(discriminant);

	//Find nearest root that lies within acceptable range
	auto root = (-half_b - sqrtd) / a;
	if (root <  t_min || root > t_max) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || root > t_max)
			return false;
	}

	rec.t = root;
	rec.p = r.at(rec.t); //point of intersection
	vec3 outward_normal = (rec.p - center) / radius;//returning a unit vector 
	rec.set_face_normal(r, outward_normal);

	return true;
}


#endif