#include "lane_kmeans.h"

#include <math.h>

void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_mapped_value_t **result, uint8_t iterations, uint8_t cluster_size) {
	lane_kmeans_mapped_value_t *points;
	int i;

	points = calloc(lines_amount, sizeof(lane_kmeans_mapped_value_t));

	for (i = 0; i < lines_amount; ++i) {
		points[i].line = &(lines[i]);
		//points[i].x = 
	}

	(*result) = points;
}

