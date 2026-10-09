#include <iostream>
#include <string>

#include <SDL3/SDL.h>

#include "render.hpp"


// https://blog.demofox.org/2023/10/22/how-to-make-your-own-spooky-magic-eye-pictures-autostereograms/
// https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-41-real-time-stereograms
// https://kapandaria.wordpress.com/stereograma/
// https://github.com/ssloy/tinyraytracer/wiki/Part-2:-low-budget-stereo-rendering

// magick mogrify -resize 128x128! -format bmp *.*


size_t WIDTH = 1200, HEIGHT = 800;
int OFFSET = 40;


int main() {
	srand(time(0));

	bool RUNNING = true;
	std::vector<RGBA_t> result(WIDTH*HEIGHT, RGBA_t{0,0,0,255});

	Renderer renderer {WIDTH, HEIGHT, "Stereogram"};
	
	// read color map bmp into texture

	std::vector<size_t> colorMap_widths {};
	std::vector<size_t> colorMap_heights {};
	std::vector<std::vector<RGBA_t>> colorMaps {};
	size_t colorMap_index = 0;

	std::vector<std::string> colorFiles {
		"colors.bmp", 
		"flores.bmp", 
		"gravel.bmp", 
		"pumpkins.bmp", 
		"satin.bmp", 
		"squares.bmp", 
		"tiles.bmp", 
		"wood.bmp"
	};
	for (const std::string& c : colorFiles) {
		colorMap_widths.push_back(0);
		colorMap_heights.push_back(0);

		colorMaps.push_back(loadBMP(
			"assets/tile/" + c, 
			colorMap_widths[colorMap_index], 
			colorMap_heights[colorMap_index]
		));

		colorMap_index++;
	}
	colorMap_index = 0;
	
	auto randRGB = []() {
		return static_cast<uint8_t>(rand() % 255);
	};

	//if (0) for (size_t i = 0; i < colorMap_width*colorMap_height; ++i) {
	//	colorMap[i] = RGBA_t{randRGB(), randRGB(), randRGB(), 255};
	//}
	
	// read depth map bmp into texture

	std::vector<size_t> depthMap_widths {};
	std::vector<size_t> depthMap_heights {};
	std::vector<std::vector<RGBA_t>> depthMaps {};
	size_t depthMap_index = 0;

	std::vector<std::string> depthFiles {
		"grave.bmp", 
		"lebron.bmp", 
		"mario.bmp", 
		"rings.bmp", 
		"eagle.bmp"
	};
	for (const std::string& c : depthFiles) {
		depthMap_widths.push_back(0);
		depthMap_heights.push_back(0);

		depthMaps.push_back(loadBMP(
			"assets/depth/" + c, 
			depthMap_widths[depthMap_index], 
			depthMap_heights[depthMap_index]
		));

		depthMap_index++;
	}
	depthMap_index = 0;


	for (size_t v = 0; v < depthMaps.size(); ++v) {
		// fill in top and left of depth map to align center of screen
		size_t leftPadding = (WIDTH - depthMap_widths[v])/2;
		if (depthMap_widths[v] > WIDTH) leftPadding = 0;
		size_t topPadding = (HEIGHT - depthMap_heights[v])/2;
		if (depthMap_heights[v] > HEIGHT) topPadding = 0;
	
		// ok now fill in
		std::vector<RGBA_t> newvec {};
	
		for (size_t i = 0; i < topPadding; ++i) {
			newvec.insert(newvec.begin(), depthMap_widths[v] + leftPadding, RGBA_t{0,0,0,255});
		}
		
		for (size_t i = 0; i < depthMap_heights[v]; ++i) {
			for (size_t n = 0; n < leftPadding; ++n) {
				newvec.push_back(RGBA_t{0,0,0,255});
			}
			auto start = depthMaps[v].begin() + (i * depthMap_widths[v]);
			newvec.insert(newvec.end(), start, start + depthMap_widths[v]);
		}
	
		depthMaps[v] = std::move(newvec);
		depthMap_widths[v] += leftPadding;
		depthMap_heights[v] += topPadding;
	}
	

	bool presented = false;
	while (RUNNING) {

		// poll events

		// key buffer, not key states
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				RUNNING = false;
			}
			else if (event.type == SDL_EVENT_KEY_DOWN) {
				switch (event.key.key) {
					case SDLK_ESCAPE: 
						RUNNING = false;
						break;
					case SDLK_EQUALS: 
						OFFSET++;
						std::cout << "Offset: " << OFFSET << std::endl;
						break;
					case SDLK_MINUS: 
						OFFSET--;
						if (OFFSET < 0) OFFSET = 0;
						std::cout << "Offset: " << OFFSET << std::endl;
						break;
					
					case SDLK_LEFT: 
						if (depthMap_index == 0) depthMap_index = depthMaps.size();
						depthMap_index--;
						break;
					case SDLK_RIGHT: 
						depthMap_index++;
						if (depthMap_index == depthMaps.size()) depthMap_index = 0;
						break;
					case SDLK_UP: 
						colorMap_index++;
						if (colorMap_index == colorMaps.size()) colorMap_index = 0;
						break;
					case SDLK_DOWN: 
						if (colorMap_index == 0) colorMap_index = colorMaps.size();
						colorMap_index--;
						break;
				}
			}
		}


		// render

		if (!presented) {
			auto getDepthmapPixel = [depthMap_widths, depthMap_heights, depthMap_index, &depthMaps](size_t x, size_t y) {
				if (x >= depthMap_widths[depthMap_index] || y >= depthMap_heights[depthMap_index]) return RGBA_t{0,0,0,255};
				return depthMaps[depthMap_index][y*depthMap_widths[depthMap_index] + x];
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
					if (x < colorMap_widths[colorMap_index]) {
						result[y*WIDTH + x] = colorMaps[colorMap_index][(y % colorMap_heights[colorMap_index])*colorMap_widths[colorMap_index] + ((x + offset) % colorMap_widths[colorMap_index])];
					}
					// else, take from already done image
					//		 ((x + offset – colormap_width), y)
					else {
						result[y*WIDTH + x] = result[y*WIDTH + (x + offset - colorMap_widths[colorMap_index])];
					}
				}
			}

			renderer.render(result);
			//presented = true;
		}
	}
	
	return 0;
}