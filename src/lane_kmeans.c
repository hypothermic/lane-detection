#include "lane_kmeans.h"

#include <math.h>

#include "lane_log.h"

static int mapped_value_compare(const void *a, const void *b) {
	lane_kmeans_mapped_value_t va = *((lane_kmeans_mapped_value_t *)a),
				   vb = *((lane_kmeans_mapped_value_t *)b);

	return (va.line->theta > vb.line->theta) - (va.line->theta < vb.line->theta);
}

void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_mapped_value_t **result, uint8_t iterations, uint8_t cluster_size) {
	lane_kmeans_mapped_value_t *points;
	lane_kmeans_medoid_t *medoids;
	int i, j, k, md, sum[cluster_size], total[cluster_size];
	uint16_t diff;

	if (!result || lines_amount < 1 || iterations < 1 || cluster_size < 1) {
		LANE_LOG_ERROR("Invalid arguments passed to %s", __func__);
		return;
	}

	points = calloc(lines_amount, sizeof(lane_kmeans_mapped_value_t));
	medoids = calloc(cluster_size, sizeof(lane_kmeans_medoid_t));

	for (i = 0; i < lines_amount; ++i) {
		points[i].line = &(lines[i]);
		points[i].cluster = 0;
		points[i].nearest = UINT16_MAX;
	}

	//sort the lines by theta
	qsort(points, lines_amount, sizeof(lane_kmeans_mapped_value_t), &mapped_value_compare);

	// Assign initial points
	// (just assume that lines \geq |cluster_size|)
	md = floor(points[lines_amount-1].line->theta - points[0].line->theta) / lines_amount;
	j = points[0].line->theta;
	for (i = 0; i < cluster_size; ++i) {
		medoids[i].theta = j;
		sum[i] = total[i] = 0;
		j += md;
	}

	// TODO put this in a different iterate method
	for (i = 0; i < iterations; ++i) {
		for (j = 0; j < cluster_size; ++j) {
			for (k = 0; k < lines_amount; ++k) {
				diff = abs(points[k].line->theta - medoids[j].theta);
				if (diff < points[k].nearest) {
					points[k].nearest = diff;
					points[k].cluster = j;
				}
			}
		}

		for (j = 0; j < lines_amount; ++j) {
			sum[points[j].cluster] += points[j].line->theta;
			(void) ++total[points[j].cluster];
			points[j].nearest = UINT16_MAX;
		}

		for (j = 0; j < cluster_size; ++j) {
			medoids[j].theta = sum[j] / total[j];
		}
	}

	free(medoids);

	(*result) = points;
}

