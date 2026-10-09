#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main(){
    vec3 normal = normalize(fragNormal);

    //  TODO: Temporary visualization test. Remove after confirming proper normal IO.
    //outColor = vec4(fragColor, 1.0);
    outColor = vec4(fragNormal*0.5+0.5, 1.0);
}