#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "texture.h"
#include "triangle.h"

class sphere : public hittable {
public:
	sphere() : radius(0){}
	sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, double t_min, double t_max,
		hit_record& t) const override;//"override" to make sure it's actually overriding something
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
	static hittable_list generatePolySphere(float rad, int divs, shared_ptr<material> m);

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

hittable_list sphere::generatePolySphere(float rad, int divs, shared_ptr<material> m)
{
	int numVerts = (divs - 1) * divs + 2;

	hittable_list polySphere;

	int numVertices = (divs - 1) * divs + 2;

	std::unique_ptr<vec3[]> P(new vec3[numVertices]);
	std::unique_ptr<vec3[]> N(new vec3[numVertices]);
	//std::unique_ptr<vec2[] = ""> st(new vec2f[numVertices]);


	double u = -pi / 2;
	double v = pi;
	double du = pi / divs;
	double dv = 2 * pi / divs;
	P[0] = N[0] = vec3(0, -rad, 0);

	int k = 1;

	for (int i = 0; i < divs - 1; i++) {
		u += du;
		v = -pi;
		for (int j = 0; j < divs; j++) {
			float x = rad * cos(u) * cos(v);
			float y = rad * sin(u);
			float z = rad * cos(u) * sin(v);
			P[k] = N[k] = vec3(x, y, z);
			//st[k].x = u / M_PI + 0.5;
			//st[k].y = v * 0.5 / M_PI + 0.5;
			v += dv, k++;
		}
	}
	P[k] = N[k] = vec3(0, rad, 0);

	int npolys = divs * divs;
	std::unique_ptr<int[]> faceIndex(new int[npolys]);
	std::unique_ptr<int[]> vertsIndex(new int[(6 + (divs - 1) * 4) * divs]);

	// create the connectivity lists                                                                                                                                                                        
	int vid = 1, numV = 0, l = 0;
	k = 0;
	for (int i = 0; i < divs; i++) {
		for (int j = 0; j < divs; j++) {
			if (i == 0) {
				faceIndex[k++] = 3;
				vertsIndex[l] = 0;
				vertsIndex[l + 1] = j + vid;
				vertsIndex[l + 2] = (j == (divs - 1)) ? vid : j + vid + 1;
				l += 3;
			}
			else if (i == (divs - 1)) {
				faceIndex[k++] = 3;
				vertsIndex[l] = j + vid + 1 - divs;
				vertsIndex[l + 1] = vid + 1;
				vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
				l += 3;
			}
			else {
				faceIndex[k++] = 4;
				vertsIndex[l] = j + vid + 1 - divs;
				vertsIndex[l + 1] = j + vid + 1;
				vertsIndex[l + 2] = (j == (divs - 1)) ? vid + 1 : j + vid + 2;
				vertsIndex[l + 3] = (j == (divs - 1)) ? vid + 1 - divs : j + vid + 2 - divs;
				l += 4;
			}
			numV++;
		}
		vid = numV;
	}

	
	//CREATING TRIANGLES
	k = 0;
	int maxVertIndex = 0, numTris = 0;
	// find out how many triangles we need to create for this mesh
	for (int i = 0; i < npolys; ++i) {
		numTris += faceIndex[i] - 2;
		for (int j = 0; j < faceIndex[i]; ++j)
			if (vertsIndex[k + j] > maxVertIndex)
				maxVertIndex = vertsIndex[k + j];
		k += faceIndex[i];
	}
	maxVertIndex += 1;

	//// allocate memory to store the position of the mesh vertices
	//P = std::unique_ptr<Vec3f[]>(new Vec3f[maxVertIndex]);
	//for (int i = 0; i < maxVertIndex; ++i) {
	//	P[i] = verts[i];
	//}

	// allocate memory to store triangle indices
	std::unique_ptr<int[]> trisIndex(new int[numTris * 3]);
	
	l = 0;
	// generate the triangle index array
	for (int i = 0, k = 0; i < npolys; ++i) {  //for each  face 
		for (int j = 0; j < faceIndex[i] - 2; ++j) {  //for each triangle in the face 
			
			trisIndex[l] = vertsIndex[k];
			trisIndex[l + 1] = vertsIndex[k + j + 1];
			trisIndex[l + 2] = vertsIndex[k + j + 2];

			l += 3;
		}
		k += faceIndex[i];
	}
	// store normals and st coordinates...

	for (int i = 0, j = 0; i < numTris; i++, j += 3) {
		Vertex v1, v2, v3;
		v1.Position = vec3(P[trisIndex[j]]);
		v2.Position = vec3(P[trisIndex[j + 2]]);
		v3.Position = vec3(P[trisIndex[j + 1]]);

		//CCW currently
		polySphere.add(make_shared<triangle>(v1, v2, v3, m));
	}

	return polySphere;
}

#endif