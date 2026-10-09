#include "textureHandler.hpp"


#include <iostream>
#include <filesystem>

#include "render.hpp"


std::vector<std::string> getFilesInDir(const char* path) {
	std::vector<std::string> files {};

	for (const std::filesystem::directory_entry& f : std::filesystem::directory_iterator(path)) {
		std::string file = f.path().string();
		std::cout << file << std::endl;
		// only get bmp
		if (file.find(".bmp") != std::string::npos) files.push_back(file);
	}

	return files;
}

void TextureHandler::loadColorMaps() {
	// read color map bmp into texture
	std::vector<std::string> colorFiles = getFilesInDir("assets/tile");

	for (const std::string& c : colorFiles) {
		colorMap_widths.push_back(0);
		colorMap_heights.push_back(0);

		colorMaps.push_back(loadBMP(
			c, 
			colorMap_widths[colorMap_index], 
			colorMap_heights[colorMap_index]
		));

		colorMap_index++;
	}
	colorMap_index = 0;

	// add fuzzy texture at back
	colorMaps.push_back({});
	for (size_t i = 0; i < 128*128; ++i) {
		colorMaps.back().push_back(RGBA_t{randRGB(), randRGB(), randRGB(), 255});
	}
	colorMap_widths.push_back(128);
	colorMap_heights.push_back(128);
}

void TextureHandler::loadDepthMaps() {
	// read depth map bmp into texture
	std::vector<std::string> depthFiles = getFilesInDir("assets/depth");

	for (const std::string& c : depthFiles) {
		depthMap_widths.push_back(0);
		depthMap_heights.push_back(0);

		depthMaps.push_back(loadBMP(
			c, 
			depthMap_widths[depthMap_index], 
			depthMap_heights[depthMap_index]
		));

		depthMap_index++;
	}
	depthMap_index = 0;

	centerDepthMaps();
}

void TextureHandler::centerDepthMaps() {
	for (size_t v = 0; v < depthMaps.size(); ++v) {
		// fill in top and left of depth map to align center of screen
		size_t leftPadding = (screenWidth - depthMap_widths[v])/2;
		if (depthMap_widths[v] > screenWidth) leftPadding = 0;
		size_t topPadding = (screenHeight - depthMap_heights[v])/2;
		if (depthMap_heights[v] > screenHeight) topPadding = 0;
	
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
}


// gets a depth map pixel, returning black if not in bounds
RGBA_t TextureHandler::getDepthmapPixel(size_t x, size_t y) const {
	if (x >= depthMap_widths[depthMap_index] || y >= depthMap_heights[depthMap_index]) return RGBA_t{0,0,0,255};
	return depthMaps[depthMap_index][y*depthMap_widths[depthMap_index] + x];
}


void TextureHandler::doFuzzy() {
	for (size_t i = 0; i < 128*128; ++i) {
		colorMaps.back()[i] = RGBA_t{randRGB(), randRGB(), randRGB(), 255};
	}
}