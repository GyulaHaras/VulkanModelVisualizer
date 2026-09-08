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

// // Teapot .obj model
// #define VERTEX_BLOCK_SIZE 3366
// #define NORMAL_BLOCK_SIZE 6
// #define TEX_BLOCK_SIZE 14
// #define FACE_BLOCK_SIZE 6319

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
		// TODO: Should move this implementation somewhere more reusable.
		vmv_errno_t fopen_s(FILE **pFile, const char* filename, const char* mode){
			vmv_errno_t filecode = 0; 
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

typedef struct face {
		vec3* v[3];
		vec3* n[3];
		vec2* t[3];
} face;

// TODO: Consider creating a "document metadata struct" for registering the pointers 
// for the points in the big string where the starts of a specific type of data is 
// contained so to not waste the first pass which scans only for amount of data.

void parse_vector(float* vector, int vec_size, char** next_token) {
	char* endptr;
	char* token = "";
	float value = 0.0;
							
	for (int ijk = 0; ijk < vec_size; ijk++) {
		token = strtok_tsafe(NULL, " ", next_token);
		value = strtof(token, &endptr);
		vector[ijk] = value;
		// Need to call strtok again here to get the next float in the line.
		value = strtof(endptr, &endptr);
	}
}


void parse_face (face* vector, int vec_size, int next_face_index,
	char** next_face_substr, 
	vec3* vertices, vec3* normals, vec2* texture_coords){

	char* face_str = "";
	char* face_comp_str = "";
	char* next_comp;
	int value;

	// Temporary holder of the face components since iterating on the face struct members is not practical
	face current_face;
	int face_props[3];
	vec3 current_vertex;
	vec3 current_normal;
	vec2 current_texture_coords;
	int faces_vec_size = (sizeof vector)/(sizeof vector[0]);

	// TODO: Add a format validator for only 3D-mentional vectors (separated by a space).
	for(int ijk = 0; ijk < vec_size; ijk++){
		face_str = strtok_tsafe(NULL, " ", next_face_substr);
		// Initializing a different line of parsing for the different face attributes
		face_comp_str = strtok_tsafe(face_str, "/", &next_comp);

		for(int vnt = 0; vnt < 3; vnt++){
			value = strtol(face_comp_str, NULL, 10);
			face_props[vnt] = value;

			face_comp_str = strtok_tsafe(NULL, "/", &next_comp);
		}

		// Linking data pointers to their respective face though their indices.
		current_face.v[ijk] = &vertices[face_props[0]];
		current_face.n[ijk] = &normals[face_props[1]]; 
		current_face.t[ijk] = &texture_coords[face_props[2]]; 

		// TODO: Consider validattion face data before adding it to the array

		vector[next_face_index] = current_face;
		printf("\n");
	}
}

void print_objModel() {
	 printf("Parsing .obj file...\n");
	vmv_errno_t file_read_error;
	FILE* file_ptr = NULL;

	file_read_error = fopen_s(&file_ptr, "models/cube.obj", "r");
	if (file_read_error) {
		perror("File read failed.\n");
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
	face* faces = NULL;

	int* vertices_length = 0;

	vmv_rsize_t vertices_current_size = 0;
	//TODO: Consider a different heuristic for vector size, consider a first pass 
	// through the document for finding number of every type of data
	vmv_rsize_t vertices_alloc_size = sizeof(vec3) * VERTEX_BLOCK_SIZE;
	vmv_rsize_t normals_alloc_size = sizeof(vec3) * NORMAL_BLOCK_SIZE;
	vmv_rsize_t texture_coords_alloc_size = sizeof(vec2) * TEX_BLOCK_SIZE;
	vmv_rsize_t face_alloc_size = sizeof(face) * FACE_BLOCK_SIZE;

	if (file_ptr != NULL) {
		printf("File found.\n");
		char* token = NULL;
		vertices = malloc(vertices_alloc_size);
		normals = malloc(normals_alloc_size);
		texture_coords = malloc(texture_coords_alloc_size);
		faces = malloc(face_alloc_size);

		if(
			vertices == NULL ||
			normals == NULL ||
			texture_coords == NULL ||
			faces == NULL
		){
			perror("Problem allocating memory.");
		}

		int step = 1;
		// Indices for indexing multiple arrays data:
		// [0]-> Vertices
		// [1]-> Normals
		// [2]-> Texture coords
		// [3]-> Faces

		int indices[4] = {0,0,0,0};
		vec3** tmp_v;
		vec2 tmp_t;
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
			face current_face = {};
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
						int i = indices[2];
						texture_coords[i] = texture_pos;
						printf("New Texture Coords {%f,%f} added to pos[%d]\n",texture_coords[i].components.x,texture_coords[i].components.y, indices[2]);
						indices[2]++;
					}
					else if ( strcmp(token, "vn") == 0) {
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
						parse_vector(vertex.xyz, 3, &next_token);
						printf("x: %f y: %f z: %f\n", vertex.components.x, vertex.components.y, vertex.components.z);
						int i = indices[0];
						vertices[i] = vertex;
						printf("New vertex {%f,%f,%f} added to pos[%d]\n",vertices[i].components.x,vertices[i].components.y,vertices[i].components.z,indices[0]);
						indices[0]++;
					}
					break;
				case 'f':
					printf("Face #%d: %s ",indices[3], token);
					printf("-----\n");
					parse_face(faces, 3, indices[3]++, &next_token, vertices, normals, texture_coords);

					tmp_v = faces[indices[3]-1].v;
					printf("Vertices: [%f,%f,%f], [%f,%f,%f], [%f,%f,%f]\n", 
						tmp_v[0]->components.x, tmp_v[0]->components.y, tmp_v[0]->components.z,
						tmp_v[1]->components.x, tmp_v[1]->components.y, tmp_v[1]->components.z,
						tmp_v[2]->components.x, tmp_v[2]->components.y, tmp_v[2]->components.z
					);

					printf("Texture Coords: [%f,%f]\n", tmp_t.components.x, tmp_t.components.y);
					printf("-----\n");
					break;

				default:
					printf("Something else: %s ", token);
					break;
			}
		}

		free(faces);
		faces = NULL;
		free(vertices);
		vertices = NULL;
		free(normals);
		normals = NULL;
		free(texture_coords);
		texture_coords = NULL;
	}
}