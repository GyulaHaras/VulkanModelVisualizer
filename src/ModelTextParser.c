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
		// TODO: Add typedef for fopen_s for a posix/linux compliant function. Maybe a wrapper of fopen()
		// TODO: Include or define a ERRNO enum instead of INT values
		// TODO: Still needs to check for the existence of the original fopen_s
		// TODO: Should move this implementation somewhere more reusable.
		// Original call of fopen_s with args: &file_ptr, "models/cube.obj", "r"
		vmv_errno_t fopen_s(FILE **pFile, const char* filename, const char* mode){
			vmv_errno_t filecode = 0; 
			// TODO: fopen is not returning a pointer to the file, is it failing?
			*pFile = fopen(filename, mode);
			if(pFile == NULL){
				filecode = 2;
			}
			return filecode;
		}
		// TODO: Add typedef for strtok_s for a thread safe posix/linux compliant function. Maybe a wrapper of strtok_r()
		// TODO: If the implementation is the same, create a template for validatting and returning pointers of different types. 
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

// TODO: vec cannot be a union since xyz and uv need their own memory with different values.
typedef union vec {
	vec3 xyz[3];
	vec2 uv[2];
} vec;

vec3 parse_vertex(char** text) {
	// char* token = NULL;
	char* endptr;
	float value = 0.0;
	vec3 v = { 0.0,0.0,0.0 };

	value = strtof(*text, &endptr);
	for (int ijk = 0; ijk < 3; ijk++) {
		v.xyz[ijk] = value;
		value = strtof(endptr, &endptr);
	}

	return v;
}

// TODO: There is no point in having an almost identical function for parsing a similar struct
//vec2 parse_uv(char** text){
//	char* endptr;
//	float value = 0.0;
//	vec2 uv = { 0.0, 0.0 };
//	for (int ij = 0; ij < 2; ij++) {
//		uv.xy[ij] = value;
//	}
//	return uv;
//}

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

			token = strtok_tsafe(line, delim, &next_token);
			
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
						printf("Vertex Normal: %s ", token);
					}
					else {
						printf("Vertex: %s ", token);
						token = strtok_tsafe(NULL, delim, &next_token);
						vec3 vertex = parse_vertex(&token);
						printf("x: %f\n", vertex.components.x);
					}
					break;
				case 'f':
					printf("Face: %s ", token);
					break;
				default:
					printf("Something else: %s ", token);
					break;
			}

			//if (token[0] == 'v') {
			//	parse_vertex(&next_token);
			//	//printf("%s ", token);
			//	while (token != NULL) {
			//		token = strtok_s(NULL, delim, &next_token);
			//		if (token) {
			//			token_f = strtof(token, &endptr);
			//			//token_f = 0.0;
			//			//if (endptr == NULL) {
			//			//	printf("Convertion to float failed. %s", endptr);
			//			//}
			//			vertex_position.xyz[direction] = token_f;
			//			// keep assigning x,y,z components in array form with indexes from 0-2
			//			direction = ++direction % 3;
			//			// if the direction resets to 0, the vec3 struction has been filled and can be pushed to the vertices array
			//			if (direction == 0) {
			//				if (vertices_current_size + sizeof(vec3) > vertices_alloc_size) {
			//					// Increase allocation size
			//					vertices_alloc_size += sizeof(vec3) * BLOCK_SIZE;
			//					vec3* tmp_vertices = vertices;

			//					vertices = realloc(vertices, vertices_alloc_size);
			//					if (vertices == NULL) {
			//						free(tmp_vertices);
			//						printf("ERROR: Unable to reallocate more memory.");
			//						return;
			//					}
			//					//free(tmp_vertices);
			//				}

			//				vertices[vertices_length++] = vertex_position;
			//				// update the held size
			//				vertices_current_size += sizeof(vec3);
			//			}
			//		}
			//		//printf("%s ", token);
			//	}
			//	//printf("\n");
			//}
			//else if (token[0] == 'f') {
			//	//printf("%s ", token);
			//	while (token != NULL) {
			//		token = strtok_s(NULL, delim, &next_token);
			//		//printf("%s ", token);
			//	}
			//	//printf("\n");
			//}
		}
		free(vertices);
	}
}