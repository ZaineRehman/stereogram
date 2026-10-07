#include <iostream>

#include <SDL3/SDL.h>

// https://blog.demofox.org/2023/10/22/how-to-make-your-own-spooky-magic-eye-pictures-autostereograms/
// https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-41-real-time-stereograms
// https://kapandaria.wordpress.com/stereograma/
// https://github.com/ssloy/tinyraytracer/wiki/Part-2:-low-budget-stereo-rendering

// https://www.google.com/search?client=firefox-b-1-d&channel=hub1&q=c%2B%2B+bmp+reader

int main() {

	std::cout << "Dope!\n";
	
	// read color map bmp into texture
	
	// read depth map bmp into texture
	
	// render
	{
		// put extra empty slice on left (x = x - colormap_width?)
		
		// offset = normalized from depthmap to some val (0-20?)
		// each pixel val, taken from colormap: 
		// x < colormap_width?
			// ((x + offset) % colormap_width, y % colormap_height)
		// else
			// ((x + offset – colormap_width), y)
	}
	
	return 0;
}