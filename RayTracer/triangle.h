#pragma once

#include "hittable.h"
#include "vec3.h"

struct Vertex {
	vec3 Position;
	//vec3 Normal;
	vec2 TexCoords;
};

class triangle : public hittable 
{
public:
	triangle() 
	{
		Vertex v;
		v.Position = vec3(0, 0, 0);
		v.TexCoords = vec2(0, 0);
		denom = 0;
		p0 = p1 = p2 = v;
	}
	triangle(Vertex _p0, Vertex _p1, Vertex _p2, shared_ptr<material> m) : p0(_p0), p1(_p1), p2(_p2), mat_ptr(m) {
		// find normal of triangle		
		vec3 p0p1 = p1.Position - p0.Position;
		vec3 p0p2 = p2.Position - p0.Position;

		normal = cross(p0p1, p0p2);
		denom = dot(normal, normal);
		unit_normal = unit_vector(normal); 
		//Vertex* t = new Vertex();
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& t) const override;
	virtual bool bounding_box(double  time0, double time1, aabb& output_box) const override;

	// triangleMesh(const int numFaces, int* faceIndex, int* vertsIndex, int* verts);

private:
	
	Vertex p0, p1, p2;
	vec3 normal;
	vec3 unit_normal;

	shared_ptr<material> mat_ptr;
	double denom;
};
#if 1
// MT (fast) ray-triangle intersection
bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	vec3 p0p1 = p1.Position - p0.Position;
	vec3 p0p2 = p2.Position - p0.Position;
	vec3 pvec = cross(r.direction(), p0p2);

	double det = dot(p0p1, pvec);

	if (fabs(det) < DBL_EPSILON) { //near 0
		return false;
	}

	double invDet = 1 / det;

	vec3 tvec = r.origin() - p0.Position;
	double u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) 
		return false;

	vec3 qvec = cross(tvec, p0p1);
	double v = dot(r.direction(), qvec) * invDet;
	if (v < 0 || u + v > 1) 
		return false;

	double t = dot(p0p2, qvec) * invDet;

	if (t < t_min || t > t_max) //TODO put this before maybe
		return false;

	//rec.normal = unit_normal;
	rec.t = t;
	rec.p = r.at(t);
	rec.set_face_normal(r, unit_normal);
	//TODO barycentric coordinates are "u" and "v"

	//u = p0
	//v = p1
	double w = 1.0 - u - v;
	rec.u = w * p0.TexCoords.x() + u * p1.TexCoords.x() + v * p2.TexCoords.x();
	rec.v = w * p0.TexCoords.y() + u * p1.TexCoords.y() + v * p2.TexCoords.y();

	rec.mat_ptr = mat_ptr;
	return true;

}
#endif

bool triangle::bounding_box(double time0, double time1, aabb& output_box) const // TODO what about triangles that are vertical/horizontal?
{
	double boxMinX = p0.Position.x(), boxMinY = p0.Position.y(), boxMinZ = p0.Position.z();
	double boxMaxX = p0.Position.x(), boxMaxY = p0.Position.y(), boxMaxZ = p0.Position.z();
	
	// BOX MINS
	if (p1.Position.x() < boxMinX) boxMinX = p1.Position.x();
	if (p2.Position.x() < boxMinX) boxMinX = p2.Position.x();

	if (p1.Position.y() < boxMinY) boxMinY = p1.Position.y();
	if (p2.Position.y() < boxMinY) boxMinY = p2.Position.y();

	if (p1.Position.z() < boxMinZ) boxMinZ = p1.Position.z();
	if (p2.Position.z() < boxMinZ) boxMinZ = p2.Position.z();
	
	// BOX MAXS

	if (p1.Position.x() > boxMaxX) boxMaxX = p1.Position.x();
	if (p2.Position.x() > boxMaxX) boxMaxX = p2.Position.x();

	if (p1.Position.y() > boxMaxY) boxMaxY = p1.Position.y();
	if (p2.Position.y() > boxMaxY) boxMaxY = p2.Position.y();

	if (p1.Position.z() > boxMaxZ) boxMaxZ = p1.Position.z();
	if (p2.Position.z() > boxMaxZ) boxMaxZ = p2.Position.z();


	//boxMaxX = boxMaxX < p1.x() ? p1.x() : boxMaxX;


	output_box = aabb(vec3(boxMinX, boxMinY, boxMinZ), vec3(boxMaxX, boxMaxY, boxMaxZ));
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

	double d = -(dot(normal, p0));
	double t = ( -dot(normal, r.origin()) + d) / NdotRayDir;

	if (t < t_min || t > t_max)
		return false;

	vec3 p = r.at(t);

	//Inside-outside test

	vec3 C; //vector perpendicular to triangle's plane

	//edge 0
	vec3 edge0 = p1 - p0;
	vec3 vp0 = p - p0;
	C = cross(edge0, vp0);
	if (dot(normal, C) < 0)
		return false; // P is outside of triangle

	//edge 1
	vec3 edge1 = p2 - p1;
	vec3 vp1 = p - p1;
	C = cross(edge1, vp1);
	double u = dot(normal, C); // for barycentric coords 
	if (u < 0)				  // (also used for inside-outside test)
		return false; 

	//edge 2
	vec3 edge2 = p0 - p2;
	vec3 vp2 = p - p2;
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


