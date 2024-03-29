#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) { //recall: color is an alias of vec3
	//write the translated [0,255] value of each colour component

	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//Divide color by number of samples and gamma correct for gamma=2.0
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);
	
	//Write the translated [0,255] value of each color component
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';

}

void write_color_to_buf(color pixel_color, int samples_per_pixel, unsigned char* image_buffer_ptr)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//Divide color by number of samples and gamma correct for gamma=2.0
	auto scale = 1.0 / samples_per_pixel;
	r = scale * r;
	g = scale * g;
	b = scale * b;

	*image_buffer_ptr = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
	*(image_buffer_ptr + 1) = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
	*(image_buffer_ptr + 2) = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}

#endif