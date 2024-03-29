#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "raytracer.h"

class camera {
public:
	camera(
		point3 lookfrom,
		point3 lookat,
		point3 vup,
		double vfov,	// vertical fov in degrees
		double aspect_ratio,
		double aperture,
		double focus_dist,
		double _time0,
		double _time1
	) { 
		time0 = _time0;
		time1 = _time1;
		auto theta = deg_to_rad(vfov);
		auto h = tan(theta / 2);	
		auto viewport_height = 2.0 * h;
		auto viewport_width = viewport_height * aspect_ratio;
		
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist*w;
		lens_radius = aperture / 2;
	}

	ray get_ray(double s, double t) const {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, 
			random_double(time0, time1));

	}


private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double lens_radius;
	double time0, time1; //shutter open/close times

};

#endif

class orthoCamera {

public:
	orthoCamera(
		point3 lookfrom,
		point3 lookat,
		point3 vup,
		double height,
		double width,
		double _time0,
		double _time1
	) {
		time0 = _time0;
		time1 = _time1;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = u * width;
		vertical = v * height;

		lower_left_corner = origin - horizontal / 2 - vertical / 2;
	}

	ray getRay(double s, double t) {

		vec3 f = lower_left_corner + s * horizontal + t * vertical;

		return ray(f, -w, random_double(time0, time1));
	}

private:

	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double time0, time1;

};
