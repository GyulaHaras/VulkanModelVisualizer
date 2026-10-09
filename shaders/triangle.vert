#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;

layout (std140, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} m;

// vec2 positions[3] = vec2[](
//     vec2(-1.0, -1.0),
//     vec2(3.0, -1.0),
//     vec2(-1.0, 3.0)
// );

// vec3 colors[3] = vec3[](
//     vec3(1,0,0),
//         vec3(0,1,0),
//             vec3(0,0,1)
// );

void main(){

    gl_Position = m.proj*m.view*m.model*vec4(inPos, 1.0);
    fragNormal = mat3(m.model)*inNormal;
    fragColor = inColor;
}