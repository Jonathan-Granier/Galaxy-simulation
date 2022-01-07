#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inSpeed;

layout(location = 0) out float outBrightness;

layout(binding = 0) uniform ModelInfo {
    mat4 model;
    mat4 view;
    mat4 proj;
} modelUbo;

void main() {
    gl_PointSize = 1;
    gl_Position = modelUbo.proj * modelUbo.view * modelUbo.model * vec4(inPosition, 1.0);
    outBrightness = length(inSpeed.xyz);
}
