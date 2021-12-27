#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "texture.h"

class sphere : public hittable {
public:
	sphere() : radius(0){}
	sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, double t_min, double t_max,
		hit_record& t) const override;//"override" to make sure it's actually overriding something
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

private:
	static void get_sphere_uv(const point3& p, double& u, double& v) {
		// NOTE FROM RT NEXT WEEK:
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
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
	get_sphere_uv(outward_normal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;// telling what type of material was hit

	return true;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const {
	output_box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

#endif