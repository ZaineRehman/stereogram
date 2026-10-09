#pragma once

#include <iostream>

#include "render.hpp"


class TextureHandler {
public: 
	size_t screenWidth, screenHeight;

	std::vector<size_t> colorMap_widths {};
	std::vector<size_t> colorMap_heights {};
	std::vector<std::vector<RGBA_t>> colorMaps {};
	size_t colorMap_index = 0;

	std::vector<size_t> depthMap_widths {};
	std::vector<size_t> depthMap_heights {};
	std::vector<std::vector<RGBA_t>> depthMaps {};
	size_t depthMap_index = 0;

	TextureHandler(size_t width, size_t height) : screenWidth(width), screenHeight(height) {
		loadColorMaps();
		loadDepthMaps();
	}

	void loadColorMaps();
	void loadDepthMaps();
	
	void centerDepthMaps();


	// gets a depth map pixel, returning black if not in bounds
	RGBA_t getDepthmapPixel(size_t x, size_t y) const;

	void doFuzzy();

	constexpr bool isFuzzy() const {
		return colorMap_index == colorMaps.size()-1;
	}

	inline uint8_t randRGB() const {
		return static_cast<uint8_t>(rand() % 255);
	};


	inline std::vector<RGBA_t> depthMap() const {
		return depthMaps[depthMap_index];
	}
	inline std::vector<RGBA_t> colorMap() const {
		return colorMaps[colorMap_index];
	}

	inline RGBA_t depthMap(size_t i) const {
		return depthMaps[depthMap_index][i];
	}
	inline RGBA_t colorMap(size_t i) const {
		return colorMaps[colorMap_index][i];
	}

	constexpr size_t colorMap_width() const {
		return colorMap_widths[colorMap_index];
	}
	constexpr size_t colorMap_height() const {
		return colorMap_heights[colorMap_index];
	}
	constexpr size_t depthMap_width() const {
		return depthMap_widths[depthMap_index];
	}
	constexpr size_t depthMap_height() const {
		return depthMap_heights[depthMap_index];
	}

	constexpr void incDepthMap() {
		depthMap_index++;
		if (depthMap_index == depthMaps.size()) depthMap_index = 0;
	}
	constexpr void decDepthMap() {
		if (depthMap_index == 0) depthMap_index = depthMaps.size();
		depthMap_index--;
	}
	constexpr void incColorMap() {
		colorMap_index++;
		if (colorMap_index == colorMaps.size()) colorMap_index = 0;
	}
	constexpr void decColorMap() {
		if (colorMap_index == 0) colorMap_index = colorMaps.size();
		colorMap_index--;
	}
};