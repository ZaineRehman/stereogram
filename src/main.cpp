#include <iostream>

#include <SDL3/SDL.h>

#include "render.hpp"


// https://blog.demofox.org/2023/10/22/how-to-make-your-own-spooky-magic-eye-pictures-autostereograms/
// https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-41-real-time-stereograms
// https://kapandaria.wordpress.com/stereograma/
// https://github.com/ssloy/tinyraytracer/wiki/Part-2:-low-budget-stereo-rendering

// magick mogrify -resize 128x128! -format bmp *.*


size_t WIDTH = 1600, HEIGHT = 1000;
int OFFSET = 20;


int main() {
	srand(time(0));

	bool RUNNING = true;
	std::vector<RGBA_t> result(WIDTH*HEIGHT, RGBA_t{0,0,0,255});

	Renderer renderer {WIDTH, HEIGHT, "Stereogram"};
	
	// read color map bmp into texture
	size_t colorMap_width, colorMap_height;
	std::vector<RGBA_t> colorMap = loadBMP("assets/tile/tiles.bmp", colorMap_width, colorMap_height);
	
	auto randRGB = []() {
		return static_cast<uint8_t>(rand() % 255);
	};

	if (0) for (size_t i = 0; i < colorMap_width*colorMap_height; ++i) {
		colorMap[i] = RGBA_t{randRGB(), randRGB(), randRGB(), 255};
	}
	
	// read depth map bmp into texture
	size_t depthMap_width, depthMap_height;
	std::vector<RGBA_t> depthMap = loadBMP("assets/depth/grave.bmp", depthMap_width, depthMap_height);


	// fill in top and left of depth map to align center of screen
	size_t leftPadding = (WIDTH - depthMap_width)/2;
	if (depthMap_width > WIDTH) leftPadding = 0;
	size_t topPadding = (HEIGHT - depthMap_height)/2;
	if (depthMap_height > HEIGHT) topPadding = 0;

	// ok now fill in
	std::vector<RGBA_t> newvec {};

	for (size_t i = 0; i < topPadding; ++i) {
		newvec.insert(newvec.begin(), depthMap_width + leftPadding, RGBA_t{0,0,0,255});
	}
	
	for (size_t i = 0; i < depthMap_height; ++i) {
		for (size_t n = 0; n < leftPadding; ++n) {
			newvec.push_back(RGBA_t{0,0,0,255});
		}
		auto start = depthMap.begin() + (i * depthMap_width);
		newvec.insert(newvec.end(), start, start + depthMap_width);
	}

	depthMap = std::move(newvec);
	depthMap_width += leftPadding;
	depthMap_height += topPadding;
	

	bool presented = false;
	while (RUNNING) {

		// poll events

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				RUNNING = false;
			}
		}

		// render

		if (!presented) {
			auto getDepthmapPixel = [depthMap_width, depthMap_height, &depthMap](size_t x, size_t y) {
				if (x >= depthMap_width || y >= depthMap_height) return RGBA_t{0,0,0,255};
				return depthMap[y*depthMap_width + x];
			};

			for (size_t x = 0; x < WIDTH; ++x) {
				for (size_t y = 0; y < HEIGHT; ++y) {
					//RGBA_t built = {0,0,0,255};

					// put extra empty slice on left (x = x - colormap_width?)
					
					// offset = normalized from depthmap
					int offset = (static_cast<float>(getDepthmapPixel(x, y).r) / 255.0) * OFFSET;

					// each pixel val, taken from colormap: 
					// x < colormap_width?
					//		 ((x + offset) % colormap_width, y % colormap_height)
					if (x < colorMap_width) {
						result[y*WIDTH + x] = colorMap[(y % colorMap_height)*colorMap_width + ((x + offset) % colorMap_width)];
					}
					// else, take from already done image
					//		 ((x + offset – colormap_width), y)
					else {
						result[y*WIDTH + x] = result[y*WIDTH + (x + offset - colorMap_width)];
					}
				}
			}

			renderer.render(result);
			presented = true;
		}
	}
	
	return 0;
}