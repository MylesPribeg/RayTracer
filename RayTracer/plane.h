#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "hittable.h"
#include "vec3.h"

class plane : public hittable {
public:
	plane() : p1(0, 0, 0), n(0, 0, 1), xEdge(0), zEdge(0) {}
	plane(point3 pt1, vec3 normal, double width, double length, shared_ptr<material> m) : p1(pt1), n(normal), mat_ptr(m) {
		xEdge = width;//dot((p1 - point3(0, 0, 0)), vec3(1, 0, 0));
		zEdge = length;// dot((p1 - point3(0, 0, 0)), vec3(0, 0, 1));
	}
	/*plane(point3 pt1, point3 pt2, point3 pt3, shared_ptr<material> m) : mat_ptr(m) {//constructor for 3 points
		vec3 v1 = (pt2 - pt1);
		vec3 v2 = (pt3 - pt1);

		n = cross(v1, v2);
		p1 = pt3;

		xEdge = dot((pt2 - point3(0, 0, 0)), vec3(1, 0, 0)) - pt1.x();
		if (xEdge < 0)
			xEdge = -xEdge;
		std::cerr << "edge 1 is: " << xEdge << std::endl;
		zEdge = dot((pt3 - point3(0, 0, 0)), vec3(0, 0, 1)) - pt1.z();
		if (zEdge < 0)
			zEdge = -zEdge;
		std::cerr << "edge 2 is: " << zEdge << std::endl;
	}*/

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& t) const override;

public:
	point3 p1;
	vec3 n;
	shared_ptr<material> mat_ptr;
	double xEdge;
	double zEdge;
};

bool plane::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 l = r.direction();
	point3 l0 = r.origin();

	//parallel test
	if (dot(l, n) == 0)
		return false;

	double d = dot((p1 - l0), n) / dot(l, n);
	if (d < t_min || d > t_max)
		return false;

	point3 intersection = l0 + l * d;
	if (intersection.x() > xEdge || intersection.x() < -xEdge
		|| intersection.z() > zEdge || intersection.z() < -zEdge) {
		return false;
	}
		

	rec.t = d;
	rec.p = intersection;
	rec.set_face_normal(r, n);
	rec.mat_ptr = mat_ptr;

	return true;
}

#endif // PLANE_H
