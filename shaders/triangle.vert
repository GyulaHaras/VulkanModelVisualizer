#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragColor;
layout(location = 2) out vec3 fragNormal;

layout (std140, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} m;

void main(){
    vec4 worldPos = m.model * vec4(inPos, 1.0);

    fragPos = worldPos.xyz;
    fragNormal = mat3(m.model)*inNormal;
    fragColor = inColor;

    gl_Position = m.proj*m.view*worldPos;
}