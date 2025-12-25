//#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ModelTextParser.h"

#define MAX_LINE_LENGTH 1024
#define BLOCK_SIZE 10

#ifdef _WIN32
	typedef errno_t vmv_errno_t;
	typedef rsize_t vmv_rsize_t;
	// TODO: Add typedef for strtok_s for a thread safe posix/linux compliant function. Maybe a wrapper of strtok_s()
	char* strtok_tsafe (FILE *restrict *restrict pFile, const char* filename, const char* mode) {
		//TODO: Use strtok_s in Windows environmentts
	}
#elif __linux__
	#include <sys/types.h>
	typedef int vmv_errno_t;
	typedef ssize_t vmv_rsize_t;
		// TODO: Still needs to check for the existence of the original fopen_s
		// TODO: Should move this implementation somewhere more reusable.
		vmv_errno_t fopen_s(FILE **pFile, const char* filename, const char* mode){
			vmv_errno_t filecode = 0; 
			// TODO: fopen is not returning a pointer to the file, is it failing?
			*pFile = fopen(filename, mode);
			if(pFile == NULL){
				filecode = 2;
			}
			return filecode;
		}
		char* strtok_tsafe(char* line, char* delim, char** next_token){
			// TODO: Use strtok_r in non-windows environments
			char* token = NULL;
			token = strtok_r(line, delim, next_token);
			return token; 
		}
#endif

typedef union vec2 {
	struct {
		float x, y;
	} components;
	float xy[2];
} vec2;

typedef union vec3 {
	struct {
		float x, y, z;
	} components;
	float xyz[3];
} vec3;

void parse_vector(float* vector, int vec_size, char** next_token) {
	// char* token = NULL;
	char* endptr;
	char* token = "";
	float value = 0.0;
	// vec3 v = { 0.0,0.0,0.0 };
							
	for (int ijk = 0; ijk < vec_size; ijk++) {
		token = strtok_tsafe(NULL, " ", next_token);
		value = strtof(token, &endptr);
		vector[ijk] = value;
		value = strtof(endptr, &endptr);
		// TODO: need to call strtok again here to get the next float in the line.
	}
}

void print_objModel() {
	 printf("Parsing .obj file...\n");
	vmv_errno_t file_read_error;
	FILE* file_ptr = NULL;

	file_read_error = fopen_s(&file_ptr, "models/cube.obj", "r");
	if (file_read_error) {
		printf("File read failed.\n");
		return;
	}

	char line[MAX_LINE_LENGTH];
	vmv_rsize_t line_length = sizeof line;
	const char* delim = " ";
	char* next_token = NULL;
	float token_f = 0.0;
	char* endptr;
	vec3 vertex_position = { 0.0,0.0,0.0 };
	int direction = 0;

	vec3* vertices = NULL;
	int* vertices_length = 0;
	vmv_rsize_t vertices_current_size = 0;
	vmv_rsize_t vertices_alloc_size = sizeof(vec3) * BLOCK_SIZE;

	if (file_ptr != NULL) {
		printf("File found.\n");
		char* token = NULL;
		vertices = malloc(vertices_alloc_size);
		int step = 1;
		while (fgets(line, MAX_LINE_LENGTH, file_ptr)) {
			printf("%d\n", step++);
			// TODO: Consider a direct read for the first character without a 
			// tokenizer in case it is faster since it is expecter no more than 
			// 1 or 2 characterss are expected before the first space. Additionally, 
			// there could be an overhead cost for reinitializing the strtok pointer 
			// for every new line.
			
			// Initializing the first token in the line string and vector variables to put in parsed values
			token = strtok_tsafe(line, delim, &next_token);
			vec3 vertex = {0.0, 0.0, 0.0};
			// Check if there is enough space for more vertices, if not realloc
			printf("%d/%d \n", vertices_current_size, vertices_alloc_size);

			switch (token[0]) {
				case 'o':
					printf("Object: %s ", token);
					break;
				case 'v':
					if (strcmp(token, "vt") == 0) {
						printf("Vertex Texture: %s ", token);
					}
					else if ( strcmp(token, "vn") == 0) {
						// TODO: Consider if having Vertex Normal logic independent from vertices is necessary.
						printf("Vertex Normal: %s ", token);
						parse_vector(vertex.xyz, 3, &next_token);
						printf("x: %f y: %f z: %f\n", vertex.components.x, vertex.components.y, vertex.components.z);
					}
					else {
						printf("Vertex: %s ", token);
						// TODO: move strtok logic to inside the vertex parser.
						// token = strtok_tsafe(NULL, delim, &next_token);
						parse_vector(vertex.xyz, 3, &next_token);
						printf("x: %f y: %f z: %f\n", vertex.components.x, vertex.components.y, vertex.components.z);
					}
					break;
				case 'f':
					printf("Face: %s ", token);
					break;
				default:
					printf("Something else: %s ", token);
					break;
			}
		}
		free(vertices);
	}
}