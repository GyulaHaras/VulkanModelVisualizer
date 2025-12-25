//#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ModelTextParser.h"

// TODO: For testing only, exclusive use for cube.obj testing case. Consider first pass for figuring out amount of lines of every data type
#define MAX_LINE_LENGTH 1024
#define VERTEX_BLOCK_SIZE 8
#define NORMAL_BLOCK_SIZE 6
#define TEX_BLOCK_SIZE 14
#define FACE_BLOCK_SIZE 12

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
	vec3* normals = NULL;
	vec2* texture_coords = NULL;

	int* vertices_length = 0;

	vmv_rsize_t vertices_current_size = 0;
	//TODO: Consider a different heuristic for vector size, consider a first pass 
	// through the document for finding number of every type of data
	vmv_rsize_t vertices_alloc_size = sizeof(vec3) * VERTEX_BLOCK_SIZE;
	vmv_rsize_t normals_alloc_size = sizeof(vec3) * NORMAL_BLOCK_SIZE;
	vmv_rsize_t texture_coords_alloc_size = sizeof(vec2) * TEX_BLOCK_SIZE;

	if (file_ptr != NULL) {
		printf("File found.\n");
		char* token = NULL;
		vertices = malloc(vertices_alloc_size);
		// TODO: Consider the need of independent size for normals
		normals = malloc(vertices_alloc_size);
		texture_coords = malloc(normals_alloc_size);

		int step = 1;
		// Indices for indexing multiple arrays data:
		// [0]-> Vertices
		// [1]-> Normals
		// [2]-> Texture coords
		// [3]-> Faces

		int indices[4] = {0,0,0,0};
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
			vec2 texture_pos = {0.0, 0.0};
			// Check if there is enough space for more vertices, if not realloc
			printf("%d/%d \n", vertices_current_size, vertices_alloc_size);

			switch (token[0]) {
				case 'o':
					printf("Object: %s ", token);
					break;
				case 'v':
					if (strcmp(token, "vt") == 0) {
						printf("Vertex Texture: %s ", token);
						parse_vector(texture_pos.xy, 2, &next_token);
						printf("u: %f v: %f\n", texture_pos.components.x, texture_pos.components.y);
						int i = indices[0];
						texture_coords[i] = texture_pos;
						printf("New vertex {%f,%f} added to pos[%d]\n",texture_coords[i].components.x,texture_coords[i].components.y, indices[2]);
						indices[2]++;
					}
					else if ( strcmp(token, "vn") == 0) {
						// TODO: Consider if having Vertex Normal logic independent from vertices is necessary.
						printf("Vertex Normal: %s ", token);
						parse_vector(vertex.xyz, 3, &next_token);
						printf("x: %f y: %f z: %f\n", vertex.components.x, vertex.components.y, vertex.components.z);
						int i = indices[1];
						normals[i] = vertex;
						printf("New Normal vector {%f,%f,%f} added to pos[%d]\n",normals[i].components.x,normals[i].components.y,normals[i].components.z,indices[1]);
						indices[1]++;
					}
					else {
						printf("Vertex: %s ", token);
						// TODO: move strtok logic to inside the vertex parser.
						// token = strtok_tsafe(NULL, delim, &next_token);
						parse_vector(vertex.xyz, 3, &next_token);
						printf("x: %f y: %f z: %f\n", vertex.components.x, vertex.components.y, vertex.components.z);
						int i = indices[0];
						vertices[i] = vertex;
						printf("New vertex {%f,%f,%f} added to pos[%d]\n",vertices[i].components.x,vertices[i].components.y,vertices[i].components.z,indices[0]);
						indices[0]++;
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