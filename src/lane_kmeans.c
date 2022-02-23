#include "lane_kmeans.h"

#include <math.h>

#include "lane_log.h"

void lane_kmeans_apply(const lane_hough_normal_t *const lines, uint16_t lines_amount, lane_kmeans_medoid_t **result, uint8_t iterations, uint8_t clusters) {
	lane_kmeans_mapped_value_t *points;
	lane_kmeans_medoid_t *medoids;
	int i, j, k, st[clusters], sr[clusters], total[clusters];
	uint16_t diff;

	if (!result || lines_amount < 1 || iterations < 1 || clusters < 1) {
		LANE_LOG_ERROR("Invalid arguments passed to %s", __func__);
		return;
	}

	points = calloc(lines_amount, sizeof(lane_kmeans_mapped_value_t));
	medoids = calloc(clusters, sizeof(lane_kmeans_medoid_t));

	for (i = 0; i < lines_amount; ++i) {
		points[i].line = &(lines[i]);
		points[i].cluster = 0;
		points[i].nearest = UINT16_MAX;
	}

	// Assign initial points
	// (just assume that lines \geq |clusters|)
	for (i = 0; i < clusters; ++i) {
		medoids[i].theta = points[i].line->theta;
		medoids[i].rho = points[i].line->rho;
		sr[i] = st[i] = total[i] = 0;
	}

	// TODO put this in a different iterate method
	for (i = 0; i < iterations; ++i) {
		for (j = 0; j < clusters; ++j) {
			for (k = 0; k < lines_amount; ++k) {
				diff = sqrt(pow(points[k].line->theta - medoids[j].theta, 2) + pow(points[k].line->rho - medoids[j].rho, 2));
				if (diff < points[k].nearest) {
					points[k].nearest = diff;
					points[k].cluster = j;
				}
			}
		}

		for (j = 0; j < lines_amount; ++j) {
			st[points[j].cluster] += points[j].line->theta;
			sr[points[j].cluster] += points[j].line->rho;
			(void) ++total[points[j].cluster];
			points[j].nearest = UINT16_MAX;
		}

		for (j = 0; j < clusters; ++j) {
			medoids[j].theta = st[j] / total[j];
			medoids[j].rho = sr[j] / total[j];
		}
	}

	free(points);

	(*result) = medoids;
}

void lane_kmeans_medoid_plot(lane_image_t *image, const lane_hough_space_t *const space, const lane_kmeans_medoid_t medoid) {
	// we can just use the trigonometry functions from the Hough code
	lane_hough_resolved_line_t line;
	lane_hough_normal_t normal = {.rho=medoid.rho, .theta=medoid.theta};

	line = lane_hough_resolve_line(image, space, normal);
	lane_hough_plot_line(image, &line);
}

