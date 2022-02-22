#ifndef LANE_KMEANS_H
#define LANE_KMEANS_H

#include <stdint.h>
#include <stdlib.h>

#include "lane_hough.h"

typedef struct mapped_value lane_kmeans_mapped_value_t;

struct mapped_value {
	const lane_hough_normal_t *line;
	uint16_t theta;
	int cluster_id;
	uint16_t nearest;
};

void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_mapped_value_t **result, uint8_t iterations, uint8_t cluster_size);

#endif /* LANE_KMEANS_H */
