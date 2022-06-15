#pragma once
#ifndef RT_STB_IMAGE_H
#define RT_STB_IMAGE_H

//disables some warnings for external library
#ifdef _MSC_VER //(for MSVC)
	#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"


//restore warnings
#ifdef _MSC_VER
	#pragma warning (pop)
#endif
#endif //!RT_STB_IMAGE_H