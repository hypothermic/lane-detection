#ifndef LANE_KMEANS_H
#define LANE_KMEANS_H

#include <stdint.h>
#include <stdlib.h>

#include "lane_hough.h"

/**
 * @inheritDoc
 */
typedef struct mapped_value	lane_kmeans_mapped_value_t;

/**
 * @inheritDoc
 */
typedef struct medoid		lane_kmeans_medoid_t;

/**
 * A way to keep the normal values and the processing data together
 */
struct mapped_value {
	const lane_hough_normal_t *line;
	uint8_t cluster;
	uint16_t nearest;
};

/**
 * A running average of the values
 */
struct medoid {
	uint16_t theta, rho;
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

void lane_kmeans_medoid_plot(lane_image_t *image, const lane_hough_space_t *const space, const lane_kmeans_medoid_t medoid);

#endif /* LANE_KMEANS_H */
