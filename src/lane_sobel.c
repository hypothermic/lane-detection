#include "lane_sobel.h"

#include "math.h"

/**
 * @inheritDoc
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest, uint8_t threshold) {
	lane_image_t *out;
	float kx[3][3] = {
		{-1,  0,  1},
		{-2,  0,  2},
		{-1,  0,  1}
	};
	float ky[3][3] = {
		{ 1,  2,  1},
		{ 0,  0,  0},
		{-1, -2, -1},
	};
	int x, y;
	int mx, my;
	int m;

	out = lane_image_new(src->width, src->height);

	// For each pixel
	for (y = 1; y < src->height-2; ++y) {
		for (x = 1; x < src->width-2; ++x) {

			// Apply kernels
			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < 3; b++) {
					mx += src->data[(y - 1 + a) * src->width + x - 1 + b].r * kx[a][b];
				}
			}

			for (int a = 0; a < 3; a++) {
				for (int b = 0; b < 3; b++) {
					my += src->data[(y - 1 + a) * src->width + x - 1 + b].r * ky[a][b];
				}
			}

			m = (int)sqrt((mx * mx) + (my * my));

			if (m < threshold) m = 0;
			if (m > 255) m = 255;

        		out->data[(y * src->width) + x].r = m;
        		out->data[(y * src->width) + x].g = m;
        		out->data[(y * src->width) + x].b = m;
		}
	}

	(*dest) = out;
}

