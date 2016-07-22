/*********************************************************
	 ---   My Module Specific Messages ---

See IPC documentation for more information:
http://www.cs.cmu.edu/~ipc/

*********************************************************/
#include <carmen/carmen.h>
#ifndef CARMEN_MAPPER_MESSAGES_H
#define CARMEN_MAPPER_MESSAGES_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
	double *complete_map;
	int size;
	carmen_map_config_t config;
	double timestamp;
	char *host;
} carmen_mapper_map_message;

#define CARMEN_MAPPER_MAP_MESSAGE_NAME	"carmen_mapper_map_message"
#define CARMEN_MAPPER_MAP_MESSAGE_FMT		"{<double:2>, int, {int, int, double, [byte:64], string, double, double}, double, string}"
#define CARMEN_MAPPER_MAP_MOVING_OBJECTS_RAW_MAP_MESSAGE_NAME "carmen_mapper_map_moving_objects_raw_map_message_name"


typedef struct
{
	int *coord_x;
	int *coord_y;
	double *value;
	int size;
	carmen_map_config_t config;
	double timestamp;
	char *host;
} carmen_mapper_compact_map_message;

#define CARMEN_MAPPER_COMPACT_MAP_MESSAGE_NAME	"carmen_mapper_compact_map_message"
#define CARMEN_MAPPER_COMPACT_MAP_MESSAGE_FMT		"{<int:4>, <int:4>, <double:4>, int, {int, int, double, [byte:64], string, double, double}, double, string}"


typedef struct
{
	carmen_map_config_t config;
	short int *complete_x_offset;
	short int *complete_y_offset;
	short int **x_offset;
	short int **y_offset;
	double *complete_distance;
	double **distance;
} carmen_mapper_distance_map;

#define CARMEN_MAPPER_DISTANCE_MAP_MESSAGE_NAME	"carmen_mapper_distance_map"
#define CARMEN_MAPPER_DISTANCE_MAP_MESSAGE_FMT	"{{int, int, double, [byte:64], string, double, double}, int, <short:2>, <short:2>, double, string}"


//typedef struct
//{
//	carmen_map_config_t config;
//	int size;
//	short int *complete_x_offset;
//	short int *complete_y_offset;
//	double timestamp;
//	char *host;
//} carmen_mapper_distance_map_message;
//


#ifdef __cplusplus
}
#endif

#endif

// @}
