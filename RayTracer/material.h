#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "raytracer.h"
#include "hittable.h" //added by me
#include "texture.h"

//struct hit_record;

class material {
public:
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const = 0;
	
	virtual color emitted(double u, double v, const point3& p) const {
		return color(0.0, 0.0, 0.0);
	}
};

class lambertian : public material {
public:
	//lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
	lambertian(shared_ptr<image_texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
		ray& scattered) const override {
		auto scatter_direction = rec.normal + random_unit_vector();

		//catch scatter directions near 0 (if unit vec is opposite of normal)
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;

		scattered = ray(rec.p, scatter_direction, r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		
		double out[4] = { 0, 0, 0, 0 };

		albedo->valueA(rec.u, rec.v, rec.p, out);

		//TODO remove
		if (out[3] < 0.5)
		{
			scattered = ray(rec.p, r_in.direction(), r_in.time());
			//attenuation *= out[3];
			attenuation = color(1, 1, 1); //attenuation.x() - out[3]
		}
		return true;
	}

public:
	shared_ptr<image_texture> albedo;
};


class metal : public material {
public:
	metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation,
		ray& scattered) const override {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz* random_in_unit_sphere(), r_in.time());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);//if ray is facing outwards return true
	}

public:
	color albedo;
	double fuzz;
};

class dielectic : public material { //ex. glass
public:
	dielectic(double ior) : ir(ior), fuzz(0) {} //ior - index of refaction
	dielectic(double ior, double frost) : ir(ior), fuzz(frost){}

	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
		attenuation = color(1, 1, 1);
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;//if not hitting from the front then IOR ratio is reciprocal

		vec3 unit_direction = unit_vector(r_in.direction());
		//vec3 refracted = refract(unit_direction, rec.normal, refraction_ratio);

		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);//from dot product, cannot be greater than 1
		double sin_theta = sqrt(1 - cos_theta * cos_theta);//from trig equality sin = sqrt(1 - cos^2(theta))

		bool cannot_refract = refraction_ratio * sin_theta > 1.0; //for Total Internal Reflection
		vec3 direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
			direction = reflect(unit_direction, rec.normal);
		else
			direction = refract(unit_direction, rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction + fuzz * random_in_unit_sphere(), r_in.time());
		return true;
	}
public:
	double ir;
	double fuzz;

private:
	static double reflectance(double cosine, double ref_idx) {//for Fresnel
		//Schlick's approximation for reflectance
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow(1 - cosine, 5);
	}

};

class diffuse_light : public material{
public:
	diffuse_light(shared_ptr<texture> a) : emit(a) {}
	diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}
	
	virtual bool scatter(
		const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		return false;
	}

	virtual color emitted(double u, double v, const point3& p) const override {
		return emit->value(u, v, p);
	}

public:
	shared_ptr<texture> emit;
};

class isotropic : public material { // for volumes
public:
	isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
	isotropic(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
	) const override {
		scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}

private:
	shared_ptr<texture> albedo;
};

#endif // !MATERIAL_H
