#pragma once

#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "raytracer.h"
#include "aabb.h"

class material;

struct hit_record {//struct in C++ is same as class, except they are public by default (also diff for templates)
	point3 p;
	vec3 normal;
	shared_ptr<material> mat_ptr;
	double t;
	bool front_face; //normal is always against direction of ray

	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0; //comparision evaluates to true or false
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {//this is an abstract class
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0; // = 0 means pure virtual function
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
};

#endif