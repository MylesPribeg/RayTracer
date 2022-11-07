#include "raytracer.h"
#include "rt_stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
#include "scenes.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <algorithm>


color ray_color(const ray& r,texture& background, const hittable& world, int depth) { //color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (!world.hit(r, 0.0001, infinity, rec)) { //hit from 0.0001 (to prevent shadow acne) to inf
		vec3 unit = unit_vector(r.direction());
		
		auto theta = acos(-unit.y());
		auto phi = atan2(-unit.z(), unit.x()) + pi;

		auto u = phi / (2 * pi);
		auto v = theta / pi;

		return background.value(u, v, point3(0,0, 0));
		
	}

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);//emitted can be removed here
}

int main() {

	//Image
	auto aspect_ratio = 16.0/9.0;//16.0 / 9.0;
	const int image_width = 1920;//400
	const int image_height= static_cast<int>(image_width/aspect_ratio);
	int samples_per_pixel = 100;
	const int max_depth = 50;


	//Camera
	point3 lookfrom(13, 2, 6); //13, 2, 3
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0); // world up vector
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 20.0;

	//World (+x-right, +y-up, +z-toward)
	
	texture* background;
	solid_color bck(0, 0.2, 0.5);
	background = &bck;
	background = new image_texture("images/Playa_Sunrise_8k.jpg"); // TODO: dynamically allocate this, shared pointer?
	hittable_list world;

	//model test
	samples_per_pixel = 10;
	vfov = 60;
	lookfrom = point3(15, 20, 35);
	lookat = point3(0, 0, 0);
	Model backpack("models/autumnhouse/source/House_scene.fbx");// , make_shared<lambertian>(color(0.0, 0.32, 0.78)));

	world = backpack.getHitList();

	//lookfrom = point3(0, 0, 20);
	//lookat = point3(0, 0, 0);
	//auto checker = make_shared<checkered_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	//world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<dielectic>(1.5, 0.5)));
	//world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(make_shared<image_texture>("images/Alpha Test2.png"))));

#if 0
	switch(10) {
	case 1:
		background = new image_texture("images/Playa_Sunrise_8k.jpg");
		world = random_scene();
		aperture = 0.1;
		break;
	case 2:
		background = new solid_color(0.70, 0.80, 1.00);
		world = two_spheres();
		break;
	case 3:
	{
		solid_color sky(0.70, 0.80, 1.00);
		background = &sky;
	}
		world = two_perlin_sphere();
		break;
	case 4:
	{
		solid_color sky(0.70, 0.80, 1.00);
		background = &sky;
	}
		world = earth();
		break;
	case 5:
		
		background = new solid_color(0, 0, 0);
		world = simple_light();
		samples_per_pixel = 400;
		lookfrom = point3(26, 3, 6);
		lookat = point3(0, 2, 0);
		break;
	case 6:
		world = cornell_box();
		samples_per_pixel = 200;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 7:
		world = cornell_box_smoke();
		samples_per_pixel = 200;
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 8:
		world = all_features();
		//aspect_ratio = 1.0;
		//image_width = 800;
		samples_per_pixel = 10000;
		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 9:
		//world = sphere::generatePolySphere(6, 6, make_shared<lambertian>(color(0, 0.5, 0.72)));
		//world.add(make_shared<xz_rect>(-10, 10, -10, 10, 0, make_shared<lambertian>(color(0, 0.5, 0.72))));
		//world.add(make_shared<sphere>(vec3(0, 0, 0), 8, make_shared<dielectic>(1.7)));
		samples_per_pixel = 50;
		vfov = 60;
		lookfrom = point3(0, 5, 25);
		lookat = point3(5, 5, 0);
	case 10:
		samples_per_pixel = 1;
		
		//world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<metal>(color(0.5, 0.5, 0.5), 1.0)));
	//case 10:
	//	world = loadPolyMeshFromGeoFile("models/cow.geo", make_shared<lambertian>(color(0, 0.5, 0.72)));
	//	world.add(make_shared<xz_rect>(-10, 10, -10, 10, 0, make_shared<lambertian>(color(0.2, 0.7, 0.1))));
	//	samples_per_pixel = 25;
	//	vfov = 60;
	//	lookfrom = point3(0, 5, 25);
	//	lookat = point3(5, 5, 0);
	}
#endif
	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
	orthoCamera oc(lookfrom, lookat, vup, 9, 16, 0.0, 1.0);
	//Render

	int buf_index = 0;
	unsigned char* image_buffer = new unsigned char[image_width * image_height * 3];
	
#define MULTI_THREAD

#ifndef MULTI_THREAD 
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << "    " << std::flush;
		for (int i = 0; i < image_width; ++i) {

			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);

				ray r = cam.get_ray(u, v);
				//ray r = oc.getRay(u, v);
				pixel_color += ray_color(r, *background, world, max_depth);

			}

			write_color_to_buf(pixel_color, samples_per_pixel, &image_buffer[buf_index]);
			buf_index += 3;
		}

	}
#endif

#ifdef MULTI_THREAD
	int thread_count = 10;

	std::cout << "\n\nMultithreaded\n";

	std::vector<color*> bufs;
	for (int i = 0; i < thread_count; i++) {
		bufs.push_back(new color[image_width * image_height]);
	}

	/*int* threadID = new int[4];
	for (int i = 0; i < 4; i++) {
		threadID[i] = i + 1;
	}*/

	std::vector<std::thread> workers;
	for (int t_num = 0; t_num < thread_count; t_num++) {
		workers.push_back(std::move(std::thread([&, t_num]() {
			//const int thread_number = t_num;
			int t = 0;
			for (int j = image_height - 1; j >= 0; --j) {
				std::cerr << "\rScanlines remaining: " << j << std::flush;//<< "    " << "THREAD NUMBER: " << t_num << std::flush;
				
				for (int i = 0; i < image_width; ++i) {

					color pixel_color(0, 0, 0);
					for (int s = 0; s < samples_per_pixel / thread_count; s++) {
						auto u = (i + random_double()) / (image_width - 1);
						auto v = (j + random_double()) / (image_height - 1);

						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, *background, world, max_depth);

					}

					if (i == 500 && j == 700) {
						std::cerr << "\nTHREAD #" << t_num << " reporting color: " << pixel_color[0]
							<< ", " << pixel_color[1] << ", " << pixel_color[2] << std::endl;
					}
					bufs.at(t_num)[t] = pixel_color; //t_num stays up lol
					t++;
				}

			}
		})));
	}


	for (std::thread& thr : workers) {
		thr.join();
		std::cerr << "Joined a thread\n";
	}


	for (int h = 0; h < image_height * image_width - 1; h++) {
		int i = 0;

		color output = color(0, 0, 0);
		//std::for_each(bufs.begin(), bufs.end(), [&output, h](color* currBuf) {
		//	output += currBuf[h];
		//	});

		for (color* currBuf : bufs) {
			output += currBuf[h];
		}
		write_color_to_buf(output, (samples_per_pixel / thread_count) * thread_count, &image_buffer[buf_index]);
		buf_index += 3;
	}
#endif

	stbi_write_jpg("output.jpg", image_width, image_height, 3, image_buffer, 100);

	delete[] image_buffer;
	//delete background; //TODO make safe

	std::cerr << "\nDone.\n";

}