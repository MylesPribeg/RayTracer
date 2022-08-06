#pragma once

#include "hittable.h"
#include "vec3.h"


class triangle : public hittable 
{
public:
	triangle() : v0(0,0,0), v1(0,0,0), v2(0,0,0) {}
	triangle(point3 _v0, point3 _v1, point3 _v2, shared_ptr<material> m) : v0(_v0), v1(_v1), v2(_v2), mat_ptr(m) {
		// find normal of triangle		
		vec3 v0v1 = v1 - v0;
		vec3 v0v2 = v2 - v0;

		normal = cross(v0v1, v0v2);
		denom = dot(normal, normal);
		unit_normal = unit_vector(normal); 
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& t) const override;
	virtual bool bounding_box(double  time0, double time1, aabb& output_box) const override;


private:
	point3 v0, v1, v2;
	vec3 normal;
	vec3 unit_normal;
	shared_ptr<material> mat_ptr;
	double denom;
};
#if 1
// MT (fast) ray-triangle intersection
bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	vec3 v0v1 = v1 - v0;
	vec3 v0v2 = v2 - v0;
	vec3 pvec = cross(r.direction(), v0v2);

	double det = dot(v0v1, pvec);

	if (fabs(det) < DBL_EPSILON) { //near 0
		return false;
	}

	double invDet = 1 / det;

	vec3 tvec = r.origin() - v0;
	double u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) 
		return false;

	vec3 qvec = cross(tvec, v0v1);
	double v = dot(r.direction(), qvec) * invDet;
	if (v < 0 || u + v > 1) 
		return false;

	double t = dot(v0v2, qvec) * invDet;

	if (t < t_min || t > t_max) //TODO put this before maybe
		return false;

	rec.normal = unit_normal;
	rec.t = t;
	rec.p = r.at(t);
	rec.set_face_normal(r, unit_normal);
	//barycentric coordinates
	rec.u = u;
	rec.v = v;

	rec.mat_ptr = mat_ptr;
	return true;

}
#endif

bool triangle::bounding_box(double time0, double time1, aabb& output_box) const 
{
	output_box = aabb(vec3(10, 10, 10), vec3(-10, -10, -10));
	return true;
}
// simple ray-triangle intersection 
#if 0
bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const 
{	
	// finding ray-plane intersection
	double NdotRayDir = dot(normal, r.direction());
	if (fabs(NdotRayDir) < 0.0000001) // almost 0
		return false; //parallel so don't intersect

	double d = -(dot(normal, v0));
	double t = ( -dot(normal, r.origin()) + d) / NdotRayDir;

	if (t < t_min || t > t_max)
		return false;

	vec3 p = r.at(t);

	//Inside-outside test

	vec3 C; //vector perpendicular to triangle's plane

	//edge 0
	vec3 edge0 = v1 - v0;
	vec3 vp0 = p - v0;
	C = cross(edge0, vp0);
	if (dot(normal, C) < 0)
		return false; // P is outside of triangle

	//edge 1
	vec3 edge1 = v2 - v1;
	vec3 vp1 = p - v1;
	C = cross(edge1, vp1);
	double u = dot(normal, C); // for barycentric coords 
	if (u < 0)				  // (also used for inside-outside test)
		return false; 

	//edge 2
	vec3 edge2 = v0 - v2;
	vec3 vp2 = p - v2;
	C = cross(edge2, vp2);
	double v = dot(normal, C);
	if (v < 0)
		return false;
	
	
	rec.normal = unit_normal;
	rec.t = t;
	rec.p = p;
;	rec.set_face_normal(r, unit_normal);
	//barycentric coordinates
	rec.u = u / denom;
	rec.v = v / denom;

	rec.mat_ptr = mat_ptr;
	return true;
}
#endif 


