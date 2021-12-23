#include "raytracer.h"

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "plane.h"
#include "camera.h"
#include "material.h"

#include <iostream>


color ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	///TODO
	if(world.hit(r, 0.0001, infinity, rec)){//hit from 0.0001 (to prevent shadow acne) to inf
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
		
		//point3 target = rec.p + rec.normal + random_unit_vector();
		//color shade = unit_vector(vec3(54, 201, 50));
		//return 0.5 * shade * ray_color(ray(rec.p, target - rec.p), world, depth - 1);

		//return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);

	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
	//					^WHITE COLOUR              ^BLUEISH COLOUR

}

hittable_list random_scene() {
	hittable_list world;

	auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
	std::cerr << "Ground material created \n";
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

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



int main() {

	//Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 400;//400
	const int image_height= static_cast<int>(image_width/aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	//World (+y-up, +x-right, +z-toward)
	hittable_list world = random_scene();
	//hittable_list world;

	/*auto mat_diffuse = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	world.add(make_shared<plane>(point3(0, 0, 0), vec3(0,1,1), 2, 3, mat_diffuse));


	auto material_sphere = make_shared<lambertian>(color(0.3, 0.5, 0.9));
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material_sphere));
	*/

	//Camera
	point3 lookfrom(13, 2, 3);
	point3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0); // world up vector
	auto dist_to_focus = 10.0;
	auto aperture = 0;

	camera cam(lookfrom, lookat, vup, 60, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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
				pixel_color += ray_color(r, world, max_depth);

			}

			write_color(std::cout, pixel_color, samples_per_pixel);
		}

	}
	std::cerr << "\nDone.\n";
}