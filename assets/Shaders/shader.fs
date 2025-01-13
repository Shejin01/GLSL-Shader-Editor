#version 330 core

in vec2 TexCoord;

// Built-In Uniforms
//*************************//
uniform vec2 resolution;
uniform float time;
uniform vec2 cursorPos;
uniform mat4 viewMatrix;
//*************************//

uniform float blendFactor;

float smoothMax(float a, float b, float k) {
	return log(exp(k*a) + exp(k*b)) / k;
}
float smoothMin(float a, float b, float k) {
	return -smoothMax(-a, -b, k);
}

float deCircle(vec2 point, float radius) {
	return length(point) - radius;
}

void main() {
	vec2 center = resolution / 2.0;
	vec2 p = gl_FragCoord.xy;
	float radius = 50.0;
	float distance = 125.0;
	vec2 pos = distance * vec2(cos(time), sin(time));
	float blendFactor = 0.05;

	float result = deCircle(p - center, radius);
	result = smoothMin(result, deCircle(p - center - pos, radius), blendFactor);

	gl_FragColor = vec4(vec3(result), 1.0);
}