/**
 * @file lane_kmeans.h
 * @author Matthijs Bakker
 * @brief Various <i>k</i>-means operations for images
 *
 * This code unit provides various <i>k</i>-means operations,
 * including the segmentation of image pixels by <i>k</i>-means
 * and the grouping of Hough Transform results by <i>k</i>-means.
 */

#ifndef LANE_KMEANS_H
#define LANE_KMEANS_H

#include <stdint.h>
#include <stdlib.h>

#include "lane_hough.h"

/**
 * @copydoc mapped_value
 */
typedef struct mapped_value	lane_kmeans_mapped_value_t;

/**
 * @copydoc mapped_pixel
 */
typedef struct mapped_pixel	lane_kmeans_mapped_pixel_t;

/**
 * @copydoc medoid
 */
typedef struct medoid		lane_kmeans_medoid_t;

/**
 * @copydoc centroid
 */
typedef struct centroid		lane_kmeans_centroid_t;

/**
 * @brief Parameters for a grouped normal line
 * 
 * A way to keep the normal values and the processing data together
 */
struct mapped_value {
	const lane_hough_normal_t *line;
	uint8_t cluster;
	uint16_t nearest;
};

/**
 * @brief Parameters for a grouped pixel
 *
 * A way to track the cluster and nearest values for a pixel in an image
 */
struct mapped_pixel {
	uint8_t luminance,
		cluster;
	uint16_t nearest;
};

/**
 * @brief A medoid (2-dimensional moving average)
 *
 * A running average of the values
 */
struct medoid {
	uint16_t theta, rho;
};

/**
 * @brief A centroid (1-dimensional moving average)
 *
 * A running average of a pixel luminance value
 */
struct centroid {
	uint8_t luminance;
};

/**
 * Group values together into <i>k</i> buckets using Lloyd's algorithm
 *
 * @param lines		The input lines to group
 * @param lines_amount	The amount of lines in the input
 * @param result	Where to store the resulting averages (size=cluster_size)
 * @param iterations	How many iterations to run
 * @param clusters	How many clusters to form
 */
void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_medoid_t **result, uint8_t iterations, uint8_t clusters);

/**
 * Plot the average result of a line cluster on an image
 *
 * @param image		The image that will be plotted upon
 * @param space		The Hough space where the line exists
 * @param medoid	The medoid which needs to be plotted
 */
void lane_kmeans_medoid_plot(lane_image_t *image, const lane_hough_space_t *const space, const lane_kmeans_medoid_t medoid);

/**
 * Segment an image into <i>k</i> amount of clusters
 *
 * @param image		The image that will be segmented (and thus modified)
 * @param iterations	The amount of iterations through Lloyd's algorithm
 * @param clusters	How many clusters should be used for grouping
 */
void lane_kmeans_segment(lane_image_t *image, uint8_t iterations, uint8_t clusters);

#endif /* LANE_KMEANS_H */
