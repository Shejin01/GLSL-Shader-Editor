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

struct Material {
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Surface {
	float sd;
	Material material;
};

Surface sdSphere(vec3 point, float radius, Material material) {
	float d = length(point) - radius;
	return Surface(d, material);
}

Surface sdBox( vec3 p, vec3 b, Material material) {
  vec3 q = abs(p) - b;
  return Surface(length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0), material);
}

Surface closestSurface(Surface s1, Surface s2) {
	if (s1.sd < s2.sd) return s1;
	return s2;
}

Surface opSubtraction( Surface s1, Surface s2 ) {
	Surface temp = s1;
	temp.sd = -s1.sd;
	if (-s1.sd > s2.sd) return temp;
	else return s2;
}

Surface sdScene(vec3 point) {
 	Material material1;
	material1.diffuse = vec3(0.0, 0.566474, 1.0);
	material1.specular = vec3(0.182900, 0.502741, 0.930636);
	material1.shininess = 16.0f;
	Material material2;
	material2.diffuse = vec3(0.855491, 0.0, 0.0);
	material2.specular = 0.1*vec3(0.705202, 0.000000, 0.000000);
	material2.shininess = 4.0f;
	Material material3;
	material3.diffuse = vec3(1.0f);
	material3.specular = vec3(0.5f);
	material3.shininess = 4.0f;
 
	vec3 center1 = vec3(0.0f, 0.0f, -4.0f);
	float speed = 0.5f;
	vec3 spin = 10.0*vec3(sin(speed*iTime), 0.0f, cos(speed*iTime));
	vec3 center2 = spin + vec3(0.0f, 0.0f, -4.0f);
	float radius1 = 1.0f;
	float radius2 = 1.0f;
	Surface s1 = sdSphere(point - center1, radius1, material1);
	Surface s2 = sdSphere(point - center2, radius2, material2);
	return closestSurface(s1, s2);
}

vec3 calculateNormal(vec3 point) {
	vec2 epsilon = vec2(0.00001, 0.0);
	float d = sdScene(point).sd;
	vec3 normal = normalize(vec3(
		sdScene(point + epsilon.xyy).sd - d,
		sdScene(point + epsilon.yxy).sd - d,
		sdScene(point + epsilon.yyx).sd - d
	));
	return normal;
}

float shadow(vec3 ro, vec3 rd, float tmin, float tmax, float w) {
	float t = tmin;
	float ph = 1e20;
	float res = 1.0f;
	for (int i = 0; i < 160; i++) {
		vec3 p = ro + t * rd;
		float h = sdScene(p).sd;
		if (h < 0.001) return 0.0f;
		
		float y = h*h/(2.0*ph);
		float d = sqrt(h*h-y*y);
		res = min(res, d / (w*max(0.0, t-y)));
		ph = h;
		t += d;
		
		if (d >= tmax) break;
	}
	return res;
}

#define PI 3.1415926535
vec3 calculateLighting(vec3 point, Material material) {
	vec3 lightPos = vec3(10.0f, 3.0f, 0.0f);
	vec3 lightColor = vec3(1.0f);
	//vec2 cursorNDC = (2.0 * mouse.iCursorPos.xy - iResolution.xy) / iResolution.y;
	//vec3 lightPos = vec3(5.0*cursorNDC, 0.0f);
	
	vec3 normal = calculateNormal(point);
	vec3 lightDir = normalize(lightPos - point);
	/*float occlusion = shadow(point, lightDir, 0.1, length(lightPos - point), 0.1);
	//vec3 lightDir = normalize(vec3(1.0f, 0.7f, 0.5f));
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * material.diffuse;

	vec3 cameraDir = normalize(camera.iCameraPos - point);
	vec3 halfwayDir = normalize(lightDir + cameraDir);
	float spec = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
	vec3 specular = spec * material.specular;
	
	vec3 ambient = material.diffuse * 0.0f;
	
	return (diffuse + specular)*occlusion + ambient;*/
	
	vec3 diffuse = material.diffuse;
	
	vec3 viewDir = normalize(camera.iCameraPos - point);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 specular = material.specular * pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
	
	vec3 brdf = diffuse + specular;
	float cosTheta = max(dot(lightDir, normal), 0.0);
	vec3 result = brdf * (lightColor * cosTheta);
	return result;
}

Surface raymarch(out vec3 point) {
	vec2 ndc = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.x;
	vec3 ro = vec3(0.0f);
	vec3 rd = normalize(vec3(ndc, -1.0f));
	vec3 p = vec3(0.0f);
	float t = 0.0f;
	
	Surface co;
	for (int i = 0; i < 160; i++) {
		p = ro + t * rd;
		p = vec3(inverse(camera.iViewMatrix) * vec4(p, 1.0f));
		co = sdScene(p);
		t += co.sd;
		if (co.sd < 0.001 || t > 100.0f) break;
	}
	
	point = p;
	return co;
}

void main() {	
	vec3 point = vec3(0.0f);
	Surface co = raymarch(point);
	vec3 color = vec3(0.0f);
	if (co.sd < 0.001) {
		color = calculateLighting(point, co.material);
	}

	gl_FragColor = vec4(color, 1.0f);
}

