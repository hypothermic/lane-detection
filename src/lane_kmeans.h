#ifndef LANE_KMEANS_H
#define LANE_KMEANS_H

#include <stdint.h>
#include <stdlib.h>

#include "lane_hough.h"

typedef struct mapped_value	lane_kmeans_mapped_value_t;

typedef struct medoid		lane_kmeans_medoid_t;

struct mapped_value {
	const lane_hough_normal_t *line;
	uint8_t cluster;
	uint16_t nearest;
};

struct medoid {
	uint16_t theta;
};

void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_mapped_value_t **result, uint8_t iterations, uint8_t cluster_size);

#endif /* LANE_KMEANS_H */
