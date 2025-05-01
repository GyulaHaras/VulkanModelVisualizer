#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ModelTextParser.h"

#define MAX_LINE_LENGTH 1024
#define BLOCK_SIZE 10

union vec3 {
	struct {
		float x, y, z;
	} components;
	float xyz[3];
};

void print_objModel() {
	 printf("Parsing .obj file...\n");
	errno_t file_read_error;
	FILE* file_ptr;

	file_read_error = fopen_s(&file_ptr, "models/cube.obj", "r");
	if (file_read_error) {
		printf("File read failed.\n");
		return;
	}

	char line[MAX_LINE_LENGTH];
	rsize_t line_length = sizeof line;
	const char* delim = " ";
	char* next_token = NULL;
	float token_f = 0.0;
	char* endptr;
	union vec3 vertex_position = { 0.0,0.0,0.0 };
	int direction = 0;

	union vec3* vertices;
	int vertices_length = 0;
	rsize_t vertices_current_size = 0;
	rsize_t vertices_alloc_size = sizeof(union vec3) * BLOCK_SIZE;

	if (file_ptr) {
		printf("File found.\n");
		char* token = NULL;
		vertices = malloc(vertices_alloc_size);
		while (fgets(line, MAX_LINE_LENGTH, file_ptr)) {
			token = strtok_s(line, delim, &next_token);
			
			// Check if there is enough space for more vertices, if not realloc
			printf("%d/%d", vertices_current_size, vertices_alloc_size);

			if (token[0] == 'v') {
				printf("%s ", token);
				while (token != NULL) {
					token = strtok_s(NULL, delim, &next_token);
					if (token) {
						//token_f = strtof(token, &endptr);
						token_f = 0.0;
						//if (endptr == NULL) {
						//	printf("Convertion to float failed. %s", endptr);
						//}
						vertex_position.xyz[direction] = token_f;
						// keep assigning x,y,z components in array form with indexes from 0-2
						direction = ++direction % 3;
						// if the direction resets to 0, the vec3 struction has been filled and can be pushed to the vertices array
						if (direction == 0) {
							if (vertices_current_size + sizeof(union vec3) > vertices_alloc_size) {
								// Increase allocation size
								vertices_alloc_size += sizeof(union vec3) * BLOCK_SIZE;
								union vec3* tmp_vertices = vertices;

								vertices = realloc(vertices, vertices_alloc_size);
								if (vertices == NULL) {
									free(tmp_vertices);
									printf("ERROR: Unable to reallocate more memory.");
									return;
								}
								//free(tmp_vertices);
							}

							vertices[vertices_length++] = vertex_position;
							// update the held size
							vertices_current_size += sizeof(union vec3);
						}
					}
					printf("%s ", token);
				}
				printf("\n");
			}
			else if (token[0] == 'f') {
				printf("%s ", token);
				while (token != NULL) {
					token = strtok_s(NULL, delim, &next_token);
					printf("%s ", token);
				}
				printf("\n");
			}
		}
		free(vertices);
	}
}