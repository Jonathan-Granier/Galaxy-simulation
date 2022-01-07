#version 450

layout(location = 0) in float outBrightness;

layout(location = 0) out vec4 outColor;

void main() {
    float brightness = outBrightness / 5.0;
    vec3 color1 = vec3(0.05, 0.05, 0.3);
	vec3 color2 = vec3(0.05, 0.3, 0.3);
    
	outColor = vec4(0.7 * mix(color1, color2 , 0.05 * brightness ) * brightness, 1.);
}
