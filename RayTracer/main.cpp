#include "raytracer.h"

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "plane.h"
#include "camera.h"
#include "material.h"

#include <iostream>


color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (!world.hit(r, 0.0001, infinity, rec)) { //hit from 0.0001 (to prevent shadow acne) to inf
		return background;
	}

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));

	auto checker = make_shared<checkered_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	std::cerr << "Ground material created \n";
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -4; a < 4; a++) {//-11 to 11
		for (int b = -4; b < 4; b++) {
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


					std::cerr << "created diffuse sphere\n";

				}
				else if (choose_mat < 0.95) {			//metal
					auto albedo = color::random(0.5,1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					std::cerr << "created metal sphere\n";

				}
				else {									//glass
					sphere_material = make_shared<dielectic>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
					std::cerr << "created glass sphere\n";

				}
			}
#endif
		}
	}

	
	//auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_right = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	//auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8));
	auto material_centre = make_shared<dielectic>(1.5);
	auto material_left = make_shared<metal>(color(0.3, 0.5, 0.9), 0.3);
	//auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0);

	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material_centre));
	//world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material_right));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material_left));

	return world;
}

hittable_list two_spheres() {
	hittable_list objects;

	auto checker = make_shared<checkered_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	//auto rand = make_shared<random_grey_blocks>();
	auto pertext = make_shared<noise_texture>();
	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

	return objects;

}

hittable_list two_perlin_sphere() {
	hittable_list objects;

	auto pertext = make_shared<noise_texture>(4);

	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	return objects;
}

hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
	
	return hittable_list(globe);
}

hittable_list simple_light() {
	hittable_list objects;

	auto light_texture = make_shared<diffuse_light>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, light_texture));
	auto pertext = make_shared<noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));
	
	return objects;

}

int main() {

	//Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;//400
	const int image_height= static_cast<int>(image_width/aspect_ratio);
	int samples_per_pixel = 100;
	const int max_depth = 50;


	//Camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0); // world up vector
	auto dist_to_focus = 10.0;
	auto aperture = 0;

	//World (+y-up, +x-right, +z-toward)
	color background(0.7, 0.7, 1.0);
	hittable_list world;// = random_scene();
	switch(5) {
	case 1:
		world = random_scene();
		aperture = 0.1;
		break;
	case 2:
		world = two_spheres();
		break;
	case 3:
		world = two_perlin_sphere();
		break;
	case 4:
		world = earth();
		break;
	case 5:
		background = color(0, 0, 0);
		world = simple_light();
		samples_per_pixel = 400;
		lookfrom = point3(26, 3, 6);
		lookat = point3(0, 2, 0);
		break;
	}

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

	//Render

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << "    " << std::flush;
		for (int i = 0; i < image_width; ++i) {

			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; s++) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);

				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, background, world, max_depth);

			}

			write_color(std::cout, pixel_color, samples_per_pixel);
		}

	}
	std::cerr << "\nDone.\n";
}