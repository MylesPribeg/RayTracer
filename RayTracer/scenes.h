#pragma once

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "triangle.h"
#include "bvh.h"
#include "box.h"
#include "camera.h"
#include "material.h"
#include "constant_medium.h"
#include "model.h"

#if 0
hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));

	auto checker = make_shared<checkered_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	std::cerr << "Ground material created \n";
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++) {//-11 to 11
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

#if 1

			shared_ptr<material> sphere_material;

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {

				if (choose_mat < 0.8) {					//diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					//world.add(make_shared<sphere>(center, 0.2, sphere_material));

					auto center2 = center + vec3(0, random_double(0, 0.5), 0);
					world.add(make_shared<moving_sphere>(center, center2, 0, 1.0, 0.2, sphere_material));


					//std::cerr << "created diffuse sphere\n";

				}
				else if (choose_mat < 0.95) {			//metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					//std::cerr << "created metal sphere\n";

				}
				else {									//glass
					sphere_material = make_shared<dielectic>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					//std::cerr << "created glass sphere\n";

				}
			}
#endif
		}
	}


	//auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	//auto material_right = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto material_left = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto material_centre = make_shared<dielectic>(1.5);
	//auto material_left = make_shared<metal>(color(0.3, 0.5, 0.9), 0.3);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0);

	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material_centre));
	//world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material_right));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material_left));

	return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

hittable_list two_spheres() {
	hittable_list objects;

	auto checker = make_shared<checkered_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	//auto rand = make_shared<random_grey_blocks>();
	auto pertext = make_shared<noise_texture>();
	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

	return hittable_list(make_shared<bvh_node>(objects, 0.0, 1.0));

}

hittable_list two_perlin_sphere() {
	hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);

	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	return objects;
}

hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("images/earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	return hittable_list(globe);
}

hittable_list simple_light()
{
	hittable_list objects;

	auto light_texture = make_shared<diffuse_light>(color(2, 2, 2));

	//auto light_texture = make_shared<lambertian>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, light_texture));
	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
	//objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<diffuse_light>(color(4,4,4))));
	return objects;

}

hittable_list cornell_box()
{
	hittable_list objects;

	auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
	auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
	auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
	auto light = make_shared<diffuse_light>(color(15, 15, 15));

	//left, right, back walls
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
	//floor and ceiling
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	//light
	objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));

	//boxes
	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295)); // the vectors for the translations are inverted
															// ex. x = 265 will move it 265 units left
	objects.add(box1);

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

	//objects.add(make_shared<box>(point3(130, 0, 65), point3(295, 165, 230), white));
	//objects.add(make_shared<box>(point3(265, 0, 295), point3(430, 330, 460), white));



	return objects;
}

hittable_list cornell_box_smoke()
{
	hittable_list objects;

	auto red = make_shared<lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared<lambertian>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(7, 7, 7));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));

	objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
	objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

	return objects;
}

hittable_list all_features()
{
	hittable_list boxes1;
	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
		for (int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}
	hittable_list objects;

	objects.add(make_shared<bvh_node>(boxes1, 0, 1));

	auto light = make_shared<diffuse_light>(color(7, 7, 7));
	objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);
	auto moving_sphere_mat = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_mat));

	objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectic>(1.5)));
	objects.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectic>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectic>(1.5));
	objects.add(make_shared<constant_medium>(boundary, 0.0001, color(1, 1, 1)));

	auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	auto pertex = make_shared<noise_texture>(0.1);
	objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertex)));

	hittable_list boxes2;
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
	}

	objects.add(make_shared<translate>(
		make_shared<rotate_y>(
			make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
		vec3(-100, 270, 395)
		)
	);

	return objects;
}

//hittable_list loadPolyMeshFromGeoFile(const char* file, shared_ptr<material> m) {
//	std::ifstream ifs;
//	hittable_list mesh;
//
//
//	try {
//		ifs.open(file);
//		if (ifs.fail()) throw;
//		std::stringstream ss;
//		ss << ifs.rdbuf();
//		uint32_t numFaces;
//		ss >> numFaces;
//		std::unique_ptr<uint32_t[]> faceIndex(new uint32_t[numFaces]);
//		uint32_t vertsIndexArraySize = 0;
//		// reading face index array
//		for (uint32_t i = 0; i < numFaces; ++i) {
//			ss >> faceIndex[i];
//			vertsIndexArraySize += faceIndex[i];
//		}
//		std::unique_ptr<uint32_t[]> vertsIndex(new uint32_t[vertsIndexArraySize]);
//		uint32_t vertsArraySize = 0;
//		// reading vertex index array
//		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
//			ss >> vertsIndex[i];
//			if (vertsIndex[i] > vertsArraySize) vertsArraySize = vertsIndex[i];
//		}
//		vertsArraySize += 1;
//		// reading vertices
//		std::unique_ptr<vec3[]> verts(new vec3[vertsArraySize]);
//		for (uint32_t i = 0; i < vertsArraySize; ++i) {
//			ss >> verts[i][0] >> verts[i][1] >> verts[i][2]; //0, 1, 2 are to access x, y, z of the vector
//		}
//		// reading normals
//		std::unique_ptr<vec3[]> normals(new vec3[vertsIndexArraySize]);
//		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
//			ss >> normals[i][0] >> normals[i][1] >> normals[i][2];
//		}
//		// reading st coordinates
//		std::unique_ptr<vec2[]> st(new vec2[vertsIndexArraySize]);
//		for (uint32_t i = 0; i < vertsIndexArraySize; ++i) {
//			ss >> st[i][0] >> st[i][1];
//		}
//		int i = 10;
//		//return new TriangleMesh(numFaces, faceIndex, vertsIndex, verts, normals, st);
//
//			//CREATING TRIANGLES
//		int k = 0;
//		int maxVertIndex = 0, numTris = 0;
//		// find out how many triangles we need to create for this mesh
//		for (int i = 0; i < numFaces; ++i) {
//			numTris += faceIndex[i] - 2;
//			for (int j = 0; j < faceIndex[i]; ++j)
//				if (vertsIndex[k + j] > maxVertIndex)
//					maxVertIndex = vertsIndex[k + j];
//			k += faceIndex[i];
//		}
//		maxVertIndex += 1;
//
//		//// allocate memory to store the position of the mesh vertices
//		//P = std::unique_ptr<Vec3f[]>(new Vec3f[maxVertIndex]);
//		//for (int i = 0; i < maxVertIndex; ++i) {
//		//	P[i] = verts[i];
//		//}
//
//		// allocate memory to store triangle indices
//		std::unique_ptr<int[]> trisIndex(new int[numTris * 3]);
//
//		int l = 0;
//		// generate the triangle index array
//		for (int i = 0, k = 0; i < numFaces; ++i) {  //for each  face 
//			for (int j = 0; j < faceIndex[i] - 2; ++j) {  //for each triangle in the face 
//
//				trisIndex[l] = vertsIndex[k];
//				trisIndex[l + 1] = vertsIndex[k + j + 1];
//				trisIndex[l + 2] = vertsIndex[k + j + 2];
//
//				l += 3;
//			}
//			k += faceIndex[i];
//		}
//		// store normals and st coordinates...
//
//
//		for (int i = 0, j = 0; i < numTris; i++, j += 3) {
//			mesh.add(make_shared<triangle>(	//CCW currently
//				verts[trisIndex[j]],
//				verts[trisIndex[j + 2]],
//				verts[trisIndex[j + 1]],
//				m));
//		}
//	}
//	catch (...) {
//		ifs.close();
//	}
//	ifs.close();
//
//	return mesh;
//}

#endif