#pragma once

#include <iostream>
#include <vector>

#include "render.hpp"
#include "textureHandler.hpp"


template<typename T>
constexpr T max(T n1, T n2) { return n1 > n2 ? n1 : n2; }
template<typename T>
constexpr T min(T n1, T n2) { return n1 > n2 ? n2 : n1; }


class StereoRender {
public: 
	static void render(
		std::vector<RGBA_t>& result, 
		const TextureHandler& texHandler, 
		size_t width, size_t height, int maxOffset
	) {
		for (size_t x = 0; x < width; ++x) {
			for (size_t y = 0; y < height; ++y) {
				//RGBA_t built = {0,0,0,255};

				// put extra empty slice on left (x = x - colormap_width?)
				
				// offset = normalized from depthmap
				int offset = (static_cast<float>(texHandler.getDepthmapPixel(x, y).r) / 255.0) * maxOffset;

				// each pixel val, taken from colormap: 
				// x < colormap_width?
				//		 ((x + offset) % colormap_width, y % colormap_height)
				if (x < texHandler.colorMap_width()) {
					result[y*width + x] = texHandler.colorMap(
						(y % texHandler.colorMap_height())*texHandler.colorMap_width() + 
						((x + offset) % texHandler.colorMap_width())
					);
				}
				// else, take from already done image
				//		 ((x + offset – colormap_width), y)
				else {
					result[y*width + x] = result[y*width + (x + offset - texHandler.colorMap_width())];
				}
			}
		}
	}
};