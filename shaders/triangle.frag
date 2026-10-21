#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout (std140, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;

    vec4 cameraPos;
    vec4 lightPos;
    vec4 lightColor;
} m;

void main(){
    vec3 normal = normalize(fragNormal);

    vec3 N = normalize(fragNormal);

    // Replace lightPosition for light.position
    vec3 lightPosition = vec3(0.0, 0.0, 0.0);
    vec3 cameraPosition = vec3(0.0, 0.0, 3.0);

    // Lambert lighting
    vec3 L = normalize(m.lightPos.xyz - fragPos);
    vec3 V = normalize(m.cameraPos.xyz - fragPos);

    float diffuse = max(dot(N,L), 0.0);

    vec3 R = reflect(-L, N);

    float specular = pow(max(dot(V, R), 0.0), 32.0);

    // Because of a lack of real ambient light, shadows will completely be black, this offset is temporary.
    float ambient = 0.1;
    // replace lightColor with light.color
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 color = fragColor * m.lightColor.xyz * (diffuse+ambient) + vec3(1.0)*specular;

    outColor = vec4(color, 1.0);
}