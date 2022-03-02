/**
 * @file lane_sobel.c
 * @author Matthijs Bakker
 * @brief Sobel filter for images
 *
 * This code unit provides a Sobel filter which convolutes
 * two predefined 3x3 kernels with an image and calculates
 * the gradient magnitude for each pixel.
 */

#include "lane_sobel.h"

#include <math.h>

#include "lane_log.h"

/**
 * @internal
 *
 * The diameter of a kernel
 */
#define KERNEL_DIAMETER	3 	// predefined Sobel kernel diameter

/**
 * @internal
 *
 * The radius of a kernel
 */
#define KERNEL_RADIUS		1 // result of (3-1)/2

/**
 * @internal
 *
 * Converts degrees to radians.
 */
#define RADIANS(degrees)	(degrees * M_PI / 180L)

/**
 * @internal
 *
 * Horizontal kernel of the Sobel operator
 */
const int kx[KERNEL_DIAMETER][KERNEL_DIAMETER] = {
	{-1,  0,  1},
	{-2,  0,  2},
	{-1,  0,  1}
};

/**
 * @internal
 *
 * Vertical kernel of the Sobel operator
 */
const int ky[KERNEL_DIAMETER][KERNEL_DIAMETER] = {
	{ 1,  2,  1},
	{ 0,  0,  0},
	{-1, -2, -1},
};

/*
 * @inheritDoc
 */
void lane_sobel_apply(const lane_image_t *const src, lane_image_t **magnitudes, double **directions) {
	lane_image_t *outm;
	double *outd;
	int x, y, mx, my, m, i, j, si, ci;

	// Because the kernel cannot be convoluted with the 1 pixel
	// border of the input image (there are no neighbor pixels)
	// we have to cut off a part of the image and correct for it.
	outm = lane_image_new(src->width - KERNEL_RADIUS * 2, src->height - KERNEL_RADIUS * 2);
	outd = calloc(outm->width * outm->height, sizeof(double));

	if (!outm || !outd) {
		LANE_LOG_ERROR("Unable to initialize memory");
		return;
	}

	// For each pixel
	for (y = KERNEL_RADIUS; y < src->height - KERNEL_RADIUS; ++y) {
		for (x = KERNEL_RADIUS; x < src->width - KERNEL_RADIUS; ++x) {
			mx = my = 0;
			
			// Convolve kernels with the original image to get
			// the horizontal and vertical magnitudes (mx, my)
			for (i = 0; i < KERNEL_DIAMETER; ++i) {
				for (j = 0; j < KERNEL_DIAMETER; ++j) {
					si = (y - KERNEL_RADIUS + i) * src->width + x - KERNEL_RADIUS + j;
					mx += src->data[si].r * kx[i][j];
					my += src->data[si].r * ky[i][j];
				}
			}

			// Calculate the gradient magnitude
			m = sqrt(pow(mx, 2) + pow(my, 2));

			// Restrict the gradient magnitude within bounds
			//
			// TODO we could calculate the max using:
			//      sqrt(pow(4*255-4*0, 2)*2)
			//      and divide m by that.
			//
			if (m < 0) m = 0;
			if (m > 255) m = 255;
	
			// Calculate the corrected index of the target image
			// (this takes into consideration the cropping)
			ci = ((y - KERNEL_RADIUS) * outm->width) + (x - KERNEL_RADIUS);

			// Calculate the arc tangent
			outd[ci] = atan2(mx, my);

			// Update the output pixel
        		outm->data[ci].r = m;
        		outm->data[ci].g = m;
        		outm->data[ci].b = m;
		}
	}

	(*magnitudes) = outm;
	(*directions) = outd;
}

/*
 * @inheritDoc
 */
void lane_nonmax_apply(const lane_image_t *const src, const double *const directions, lane_image_t **dest) {
	lane_image_t *out;
	// {si,ci,pi,ni} = {source,current,prev,next} index
	int x, y, si, ci, pi, ni;
	uint16_t d;

	out = lane_image_new(src->width - KERNEL_RADIUS * 2, src->height - KERNEL_RADIUS * 2);

	// For each pixel
	for (y = KERNEL_RADIUS; y < src->height - KERNEL_RADIUS; ++y) {
		for (x = KERNEL_RADIUS; x < src->width - KERNEL_RADIUS; ++x) {
			// Calculate the correct index for the input {image,directions} array
			si = y * src->width + x;

			d = RADIANS(directions[si]);
			if (d < 0L) {
				d += 180L;
			}

			if (d > 181L) {
				LANE_LOG_ERROR("Je bent slecht in wiskunde");
			}

			// Check which direction the line faces
			// Northeast (45 deg) or east (90 deg) facing
			if (d > 22 && d < 113) {
				// Check for southeast (45+(90-45)/2≃68)
				if (d < 68) {
					pi = (y - 1) * src->width + (x + 1);
					ni = (y + 1) * src->width + (x - 1);
				// Else, it faces east
				} else {
					pi = (y - 0) * src->width + (x + 1);
					ni = (y - 0) * src->width + (x - 1);
				}
			// Southeast (135 deg) or north (0 deg) facing
			} else {
				// Check for southeast
				if (d > 112 && d < 158) {
					pi = (y - 1) * src->width + (x - 1);
					ni = (y + 1) * src->width + (x + 1);
				// Else, it faces north
				} else {
					pi = (y - 1) * src->width + (x - 0);
					ni = (y + 1) * src->width + (x - 0);
				}
			}

			// Calculate the corrected index for the output
			ci = ((y - KERNEL_RADIUS) * out->width) + (x - KERNEL_RADIUS);
			// Check if this pixel is local maxima of those neighboring in the same direction
			if ((src->data[si].r > src->data[ni].r) && (src->data[si].r > src->data[pi].r)) {
				out->data[ci].r = src->data[si].r;
				out->data[ci].g = src->data[si].g;
				out->data[ci].b = src->data[si].b;
			} else {
				out->data[ci].r = out->data[ci].g = out->data[ci].b = 0;
			}
		}
	}

	(*dest) = out;
}

