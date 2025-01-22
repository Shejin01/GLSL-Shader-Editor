#version 330 core

in vec2 TexCoord;

// Built-In Uniforms
//*************************//
uniform vec2 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform float iFrameRate;
uniform int iFrame;
uniform vec4 iMouse;	 // Shadertoy Implementation

struct Camera {
	mat4 iViewMatrix;
	vec3 iCameraPos;
	vec3 iCameraDir;
};
struct Mouse {
	vec2 iCursorPos; // Custom Implementation
	float iScrollOffset;
	float iScrollAmount;
};
uniform Camera camera;
uniform Mouse mouse;
//*************************//

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
	vec2 center = iResolution / 2.0;
	vec2 p = gl_FragCoord.xy;
	float radius = 50.0;
	float distance = 125.0;
	vec2 pos = distance * vec2(cos(iTime), sin(iTime));
	float blendFactor = 0.05;

	float result = deCircle(p - center, radius);
	result = smoothMin(result, deCircle(p - center - pos, radius), blendFactor);

	gl_FragColor = vec4(vec3(result), 1.0);
}