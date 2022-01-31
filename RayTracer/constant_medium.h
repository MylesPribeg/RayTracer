#pragma once
#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "raytracer.h"
#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a) 
		: boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(a)) {}

	constant_medium(shared_ptr<hittable> b, double d, color c)
		: boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(c)) {}

	virtual bool hit(const ray& r_in, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
		return boundary->bounding_box(time0, time1, output_box);
	}

private:
	shared_ptr<hittable> boundary;
	shared_ptr<material> phase_function;
	double neg_inv_density;
};

bool constant_medium::hit(const ray& r_in, double t_min, double t_max, hit_record& recOut) const //assumes if ray exits boundary it will never return
{
	//Enabling enableDebug flag prints occasional samples.
	const bool enableDebug = false;
	const bool debugging = enableDebug && random_double() < 0.00001;

	hit_record rec1, rec2;

	if (!boundary->hit(r_in, -infinity, infinity, rec1))
		return false;

	if (!boundary->hit(r_in, rec1.t+0.0001, infinity, rec2))
		return false;

	if (debugging) {
		std::cerr << "\nt_min= " << rec1.t << ", t_max= " << rec2.t << '\n';
	}

	if (rec1.t < t_min) rec1.t = t_min;
	if (rec2.t > t_max) rec2.t = t_max;

	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	const auto ray_length = r_in.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = neg_inv_density * log(random_double());

	if (hit_distance > distance_inside_boundary)
		return false;

	recOut.t = rec1.t + hit_distance / ray_length;
	recOut.p = r_in.at(recOut.t);

	if (debugging) {
		std::cerr << "Hit_distance = " << hit_distance << "\nrecOut.t = " << recOut.t
			<< "\nrecOut.p = " << recOut.p << std::endl;
	}

	recOut.normal = vec3(1, 0, 0);		// doesn't matter
	recOut.front_face = true;			// also doesn't matter
	recOut.mat_ptr = phase_function;

	return true;
}

#endif // !CONSTANT_MEDIUM_H
