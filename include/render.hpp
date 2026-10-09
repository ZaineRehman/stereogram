#pragma once

#include <iostream>
#include <vector>

#include <SDL3/sdl.h>

struct RGBA_t {
	uint8_t r, g, b, a;
};


class Renderer {
public:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;

	size_t width, height;

	Renderer(
		size_t width, size_t height, 
		std::string windowName
	) : width(width), height(height) {
		SDL_Init(SDL_INIT_VIDEO);

		SDL_CreateWindowAndRenderer(
			windowName.c_str(), 
			width, 
			height, 
			0, 
			&window, 
			&renderer
		);

		texture = SDL_CreateTexture(
			renderer, 
			SDL_PIXELFORMAT_RGBA32, 
			SDL_TEXTUREACCESS_STREAMING, 
			width, 
			height
		);

		//SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	}

	~Renderer() {
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void render(const std::vector<RGBA_t>& pixels) {
		SDL_UpdateTexture(
			texture, 
			nullptr, 
			pixels.data(), 
			width * sizeof(RGBA_t)
		);

		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}
};


std::vector<RGBA_t> loadBMP(const std::string& path, size_t& width, size_t& height) {
	SDL_Surface* surface = SDL_LoadBMP(path.c_str());

	if (!surface) return {};

	width = static_cast<size_t>(surface->w);
	height = static_cast<size_t>(surface->h);

	SDL_Surface* converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(surface);

	if (!converted) return {};

	std::vector<RGBA_t> pixels(width * height);

	SDL_memcpy(
		pixels.data(),
		converted->pixels,
		pixels.size() * sizeof(RGBA_t)
	);

	SDL_DestroySurface(converted);

	return pixels;
}