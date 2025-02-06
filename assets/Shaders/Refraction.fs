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
	bool isRefractive;
};

struct Surface {
	float sd;
	Material material;
};

float sdPlane(vec3 point, vec3 normal) {
    vec3 n = normal;       // plane's normal vector
    float distanceFromOrigin = 0.; // position along normal

    return dot(point, n) + distanceFromOrigin;
}

float sdBox( vec3 p, vec3 b ) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdSphere(vec3 point, float radius) {
	return length(point) - radius;
}

Surface closestSurface(Surface s1, Surface s2) {
	if (s1.sd < s2.sd) return s1;
	return s2;
}

Surface map(vec3 point) {
	Material material1;
	material1.diffuse = vec3(0.6f);
	material1.specular = vec3(0.4f);
	material1.shininess = 2.0f;
	material1.isRefractive = true;
	
	Material material2;
	if (mod(floor(point.x) + floor(point.z), 2.0) == 0.0)
		material2.diffuse = vec3(1.0f);
	else material2.diffuse = vec3(0.5f);
	material2.specular = 0.4f * material2.diffuse;
	material2.shininess = 16.0f;
	
	Material material3;
	material3.diffuse = vec3(1.0f, 0.0f, 0.0f);
	material3.specular = vec3(0.1f);
	material3.shininess = 16.0f;
	
	Material material4 = material3;
	material4.diffuse = vec3(0.0f, 1.0f, 0.0f);
	Material material5 = material3;
	material5.diffuse = vec3(0.0f, 0.0f, 1.0f);
	Material material6 = material3;
	material6.diffuse = vec3(0.179191, 0.758027, 1.000000);
	
	vec3 center = vec3(0.0f, 0.0f, -3.0f);
	float radius = 1.0f;
	Surface sphere = Surface(sdSphere(point - center, radius), material1);
	
	float p = 10.f;
	Surface plane1 = Surface(sdBox(point - p*vec3(0.0, -1.0f, 0.0f), vec3(10., 1., 10.)), material2);
	Surface plane2 = Surface(sdBox(point - p*vec3(0.0, 1.0f, 0.0f), vec3(10., 1., 10.)), material2);
	Surface plane3 = Surface(sdBox(point - p*vec3(-1.0f, 0.0f, 0.0f), vec3(1., 10., 10.)), material3);
	Surface plane4 = Surface(sdBox(point - p*vec3(1.0f, 0.0f, 0.0f), vec3(1., 10., 10.)), material4);
	Surface plane5 = Surface(sdBox(point - p*vec3(0.0f, 0.0f, -1.0f), vec3(10., 10., 1.)), material6);
	Surface plane6 = Surface(sdBox(point - p*vec3(0.0f, 0.0f, 1.0f), vec3(10., 10., 1.)), material5);
	
	Surface co1 = closestSurface(plane1, plane2);
	Surface co2 = closestSurface(plane3, plane4);
	Surface co3 = closestSurface(plane5, plane6);
	Surface co4 = closestSurface(co1, co2);
	Surface co5 = closestSurface(co3, co4);
	
	return closestSurface(sphere, co5);
}

#define EPSILON 0.001
#define RAYMARCH_STEPS 255
Surface raymarch(out vec3 point, in vec3 ro, in vec3 rd, float tmin, float tmax) {
	float t = tmin;
	vec3 p = vec3(0.0f);
	Surface co;
	for (int i = 0; i < RAYMARCH_STEPS && t < tmax; i++) {
		p = ro + t * rd;
		p = vec3(inverse(camera.iViewMatrix) * vec4(p, 1.0f));
		co = map(p);
		if (co.sd < EPSILON) break;
		t += co.sd;
	}
	point = p;
	return co;
}

vec3 getBackground() {
	return vec3(0.0);
}

vec3 normal(vec3 point) {
	vec2 epsilon = vec2(EPSILON, 0.0f);
	float d = map(point).sd;
	vec3 normal = normalize(vec3(
		map(point + epsilon.xyy).sd - d,
		map(point + epsilon.yxy).sd - d,
		map(point + epsilon.yyx).sd - d
	));
	return normal;
}

vec3 blinnPhong(vec3 point, Material material) {
	//vec3 lightDir = normalize(vec3(4.0f, 3.0f, 2.0f));
	vec3 lightPos = vec3(0.0f, 5.0f, -3.0f);
	vec3 lightDir = normalize(lightPos - point);
	vec3 lightColor = vec3(0.7);
	vec3 normal = normal(point);
	
	vec3 diffuse = material.diffuse;
	
	vec3 viewDir = normalize(vec3(camera.iCameraPos - point));
	vec3 halfwayDir = normalize(vec3(lightDir + viewDir));
	vec3 specular = material.specular * pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
	
	vec3 ambient = material.diffuse * 0.2f;
	
	vec3 brdf = diffuse + specular;
	float cosTheta = max(dot(lightDir, normal), 0.0f);
	vec3 result = brdf * (lightColor * cosTheta);
	return result + ambient;
}

vec3 calculateLighting(vec3 point, Material material) {
	if (material.isRefractive) {
		vec3 normal = normal(point);
		vec3 incidentRay = -normalize(point - camera.iCameraPos); 
		float ior;
		float cosI = dot(normal, incidentRay);
		if(cosI > 0.0) {
		    ior = 1.52 / 1.0;
		    normal = -normal;
		}
		else ior = 1.0 / 1.52;
		
		vec3 refractedRay = refract(incidentRay, normal, ior);
		vec3 hitp = vec3(0.0f);
		vec3 origin = point - normal;
		Surface co = raymarch(hitp, point, refractedRay, EPSILON, 100.0f);
		if (co.sd < EPSILON) {
			return blinnPhong(hitp, co.material);
		}
		else return getBackground();
	}

	return blinnPhong(point, material);
}

void main() {
	vec3 ro = vec3(0.0f);
	vec2 ndc = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.x;
	vec3 rd = normalize(vec3(ndc, -1.0));
	vec3 point = vec3(0.0f);
	Surface co = raymarch(point, ro, rd, EPSILON, 100.0f);
	vec3 color = vec3(0.0f);
	if (co.sd < EPSILON) color = calculateLighting(point, co.material);
	else color = getBackground();

	gl_FragColor = vec4(color, 1.0f);
}


