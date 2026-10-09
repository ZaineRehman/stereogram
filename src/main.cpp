#include <iostream>
#include <string>

#include <SDL3/SDL.h>

#include "render.hpp"
#include "textureHandler.hpp"
#include "stereoRender.hpp"


// https://blog.demofox.org/2023/10/22/how-to-make-your-own-spooky-magic-eye-pictures-autostereograms/
// https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-41-real-time-stereograms
// https://kapandaria.wordpress.com/stereograma/
// https://github.com/ssloy/tinyraytracer/wiki/Part-2:-low-budget-stereo-rendering

// magick mogrify -resize 128x128! -format bmp *.*


size_t WIDTH = 1200, HEIGHT = 800;
int OFFSET = 30;


int main() {
	srand(time(0));

	bool RUNNING = true;
	std::vector<RGBA_t> result(WIDTH*HEIGHT, RGBA_t{0,0,0,255});

	Renderer renderer {WIDTH, HEIGHT, "Stereogram"};
	TextureHandler texHandler {WIDTH, HEIGHT};
		

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
						texHandler.decDepthMap();
						break;
					case SDLK_RIGHT: 
						texHandler.incDepthMap();
						break;
					case SDLK_UP: 
						texHandler.incColorMap();
						break;
					case SDLK_DOWN: 
						texHandler.decColorMap();
						break;
				}
			}
		}

		// do fuzzy
		if (texHandler.isFuzzy()) texHandler.doFuzzy();


		// render
		StereoRender::render(result, texHandler, WIDTH, HEIGHT, OFFSET);
		renderer.render(result);
	}
	
	return 0;
}