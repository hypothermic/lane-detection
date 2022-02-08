#ifndef LANE_LOG_H
#define LANE_LOG_H

#ifdef LANE_LOG_ENABLE

#define LANE_LOG_INFO(format, ...)	printf("INFO | " format "\n", ##__VA_ARGS__)
#define LANE_LOG_ERROR(format, ...)	fprintf(stderr, "ERROR | " format "\n", ##__VA_ARGS__)

#else

#define LANE_LOG_INFO(format, ...)	({(void)0;})
#define LANE_LOG_ERROR(format, ...)	({(void)0;})

#endif /* LANE_LOG_ENABLE */

#endif /* LANE_LOG_H */