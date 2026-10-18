#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Light {
    vec3 position;
    vec3 color;
} light;

void main(){
    vec3 normal = normalize(fragNormal);

    vec3 N = normalize(fragNormal);

    // Replace lightPosition for light.position
    vec3 lightPosition = vec3(0.0, 0.0, 0.0);
    // Lambert lighting
    vec3 L = normalize(lightPosition - fragPos);

    float diffuse = max(dot(N,L), 0.0);

    // Because of a lack of real ambient light, shadows will completely be black, this offset is temporary.
    float ambient = 0.1;
    // replace lightColor with light.color
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 color = fragColor * lightColor * (diffuse+ambient);

    outColor = vec4(color, 1.0);
}