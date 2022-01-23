#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include "raytracer.h"
#include "perlin.h"
#include "rt_stb_image.h"

class texture {
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
	solid_color() {}
	solid_color(color c) : color_value(c) {}
	solid_color(double r, double g, double b) : color_value(color(r, g, b)) {}

	virtual color value(double u, double v, const point3& p) const override {
		return color_value;
	}

public:
	color color_value;

};

class checkered_texture : public texture {
public:
	checkered_texture() {}
	checkered_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd) : even(_even), odd(_odd) {}
	checkered_texture(color _even, color _odd) : even(make_shared<solid_color>(_even)), odd(make_shared<solid_color>(_odd)) {}

	virtual color value(double u, double v, const point3& p) const override {
		auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());//creates a 3D checker texture based on the polarity of sine
		if (sines < 0) {
			return odd->value(u, v, p);
		}
		else {
			return even->value(u, v, p);
		}
	}

public:
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

class noise_texture : public texture {
public:
	noise_texture() : scale(1) {}
	noise_texture(double sc) : scale(sc) {}
	virtual color value(double u, double v, const point3& p) const override {
		//return color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
		//return color(1, 1, 1) * noise.turb(scale * p);
		return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
	}
public:
	perlin noise;
	double scale;
};

class image_texture : public texture {
public:
	const static int bytes_per_pixel = 3; //r, g, and b values
	image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0){}

	image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

		if (!data) {
			std::cerr << "Could not load image texture file '" << filename << "'.\n";
			width = height = 0;
		}
		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() {
		delete data;
	}

	virtual color value(double u, double v, const vec3& p) const override {
		if (data == nullptr)
			return color(0, 1, 1);//returns solid cyan if image missing

		//clamp texture coordinates to [0,1] x [0,1]
		u = clamp(u, 0.0, 1.0);
		v = 1.0 - clamp(v, 0.0, 1.0); //flip V to image coordinates

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);

		//clamp integer mapping, since actual coordinates should be less than 1.0
		if (i >= width)
			i = width - 1;
		if (j >= height)
			j = height - 1;

		const auto color_scale = 1.0 / 255;
		unsigned char* pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height;
	int bytes_per_scanline;
};

//class random_grey_blocks : public texture {
//public:
//	random_grey_blocks() {
//		double rand = 0;
//		for (int i = 0; i < 100; i++) {
//			rand = random_double(0.1, 1);
//			blocks[i] = color(rand, rand, rand);
//		}
//	}
//	
//	virtual color value(double u, double v, const point3& p) const override {///TODO
//		/*static int prev_floor_x = 0;
//		static int prev_floor_y = 0;
//		static int prev_floor_z = 0;
//		static color prev_color(0.1,0.1,0.1);
//		
//		int floor_x = floor(p.x());
//		int floor_y = floor(p.y());
//		int floor_z = floor(p.z());
//
//		if (floor_x == prev_floor_x && floor_y == prev_floor_y && floor_z == prev_floor_z) {
//			return prev_color;
//		}
//		else {
//			prev_floor_x = floor_x;
//			prev_floor_y = floor_y;
//			prev_floor_z = floor_z;
//			double rand = random_double(0.1, 1);
//			prev_color = color(rand, rand, rand);
//			return prev_color;
//		}*/
//		return color(0, 0, 0);
//		
//	}
//public:
//	color blocks[100];
//
//};

#endif // !TEXTURE_H
