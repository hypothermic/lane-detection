#include "lane_sobel.h"

#include "math.h"

/**
 * Horizontal kernel of the Sobel operator
 */
const int kx[3][3] = {
	{-1,  0,  1},
	{-2,  0,  2},
	{-1,  0,  1}
};

const int ky[3][3] = {
	{ 1,  2,  1},
	{ 0,  0,  0},
	{-1, -2, -1},
};

/**
 * @inheritDoc
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **dest) {
	lane_image_t *out;
	int x, y, mx, my, m, i, j;

	out = lane_image_new(src->width, src->height);

	// For each pixel
	for (y = 1; y < src->height-2; ++y) {
		for (x = 1; x < src->width-2; ++x) {
			mx = my = 0;
			
			// Convolve kernels with the original image to get
			// the horizontal and vertical magnitudes (mx, my)
			for (i = 0; i < 3; ++i) {
				for (j = 0; j < 3; ++j) {
					mx += src->data[(y - 1 + i) * src->width + x - 1 + j].r * kx[i][j];
					my += src->data[(y - 1 + i) * src->width + x - 1 + j].r * ky[i][j];
				}
			}

			// Calculate the gradient magnitude
			m = sqrt(pow(mx, 2) + pow(my, 2));

			// Restrict the gradient magnitude within bounds
			if (m < 0) m = 0;
			if (m > 255) m = 255;

        		out->data[(y * src->width) + x].r = m;
        		out->data[(y * src->width) + x].g = m;
        		out->data[(y * src->width) + x].b = m;
		}
	}

	(*dest) = out;
}

