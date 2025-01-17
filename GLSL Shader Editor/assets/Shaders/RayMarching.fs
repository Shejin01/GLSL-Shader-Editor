#version 330 core

in vec2 TexCoord;

// Built-In Uniforms
//*************************//
uniform vec2 resolution;
uniform float time;
uniform vec2 cursorPos;
uniform mat4 viewMatrix;
//*************************//

float smoothMax(float a, float b, float k) {
	return log(exp(k*a) + exp(k*b)) / k;
}
float smoothMin(float a, float b, float k) {
	return -smoothMax(-a, -b, k);
}

float deCircle(vec3 point, float radius) {
	return length(point) - radius;
}

vec3 opRepeat(vec3 p, vec3 gap, vec3 offset) {
	return mod(p, gap) - offset;
}

vec3 palette(float t) {
	vec3 color1 = vec3(0.0, 0.5, 0.4);
	vec3 color2	= vec3(0.2, 0.4, 0.2);
	vec3 color3	= vec3(2.0, 1.0, 1.0);
	vec3 color4	= vec3(0.0, 0.25, 0.25);
	float colorSpread = 0.2f;
	return color1 + color2 * cos(6.2831853072 * (color3 * t * colorSpread + color4));
}

vec3 rotateX(vec3 p, float angle) {
	mat3 rotation = mat3(
		1.0f, 0.0f, 0.0f,
		0.0f, cos(angle), -sin(angle),
		0.0f, sin(angle),  cos(angle)
	);
	return rotation * p;
}
vec3 rotateY(vec3 p, float angle) {
	mat3 rotation = mat3(
		cos(angle), 0.0f, sin(angle),
		0.0f, 1.0f, 0.0f,
		-sin(angle),  0.0f, cos(angle)
	);
	return rotation * p;
}
vec3 rotateZ(vec3 p, float angle) {
	mat3 rotation = mat3(
		cos(angle), -sin(angle), 0.0f,
		sin(angle),  cos(angle), 0.0f,
		0.0f, 0.0f, 1.0f
	);
	return rotation * p;
}

void main() {
	float radius = 0.4f;
	vec3 color = vec3(0.0f);

	vec2 ndc = (2.0 * gl_FragCoord.xy - resolution.xy) / resolution.y;
	vec3 ro = vec3(0.0f);
	vec3 rd = normalize(vec3(ndc, -1.0f));
	float t = 0.0f;
	for (int i = 0; i < 80; i++) {
		vec3 p = ro + t * rd;
		p = vec3(inverse(viewMatrix) * vec4(p, 1.0f));
		/*p = rotateX(p, t*0.004);
		p = rotateY(p, t*0.005);*/
		p = rotateZ(p, t*0.03*sin(time));
		
		p = opRepeat(p, vec3(2.0f), vec3(1.0f));
		float d = deCircle(p, radius);
		t += d;

		if (t > 100.0f) break;
		if (d < 0.001f) {
			color = palette(t*0.1f);
			break;
		}
	}

	gl_FragColor = vec4(color, 1.0f);
}






