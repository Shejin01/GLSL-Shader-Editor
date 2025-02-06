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
	bool isReflective;
};

struct Surface {
	float sd;
	Material material;
};

float sdSphere(vec3 point, float radius) {
	return length(point) - radius;
}

float sdPlane(vec3 point, vec3 normal) {
    vec3 n = normal;       // plane's normal vector
    float distanceFromOrigin = 0.; // position along normal

    return dot(point, n) + distanceFromOrigin;
}

Surface closestSurface(Surface s1, Surface s2) {
	if (s1.sd < s2.sd) return s1;
	return s2;
}

Surface map(vec3 point) {
	Material material1;
	material1.diffuse = vec3(0.75f);
	material1.specular = vec3(0.1f);
	material1.shininess = 8.0f;
	material1.isReflective = true;
	Material material2;
	if (mod(floor(0.5*point.x) + floor(0.5*point.z), 2.0) == 0.0f)
		material2.diffuse = vec3(1.0f, 0.0f, 0.0f);
	else material2.diffuse = vec3(0.0f, 0.0f, 1.0f);
	material2.specular = 0.2f * material2.diffuse;
	material2.shininess = 8.0f;
	material2.isReflective = false;

	vec3 center = vec3(0.0f, 1.0f, 0.0f);
	vec3 center2 = vec3(0.0f, 3.0f, -4.0f);
	vec3 center3 = vec3(0.0f, 4.0f, -1.0f);
	Surface sphere = Surface(sdSphere(point - center, 1.0f), material1);
	Surface sphere2 = Surface(sdSphere(point - center2, 1.0f), material1);
	Surface sphere3 = Surface(sdSphere(point - center3, 1.0f), material2);
	Surface ground = Surface(sdPlane(point - vec3(0.0f, -1.0f, 0.0f), vec3(0.,1.,0.)), material2);
	Surface mirror = Surface(sdPlane(point - vec3(5.0, 0.0, 0.0), vec3(-1.,0.,0.)), material1);
	Surface mirror2 = Surface(sdPlane(point - vec3(-5.0, 0.0, 0.0), vec3(1., 0., 0.)), material1);
	Surface mirror3 = Surface(sdPlane(point - vec3(0.0, 0.0, -5.0), vec3(0., 0., 1.)), material1);
	Surface mirror4 = Surface(sdPlane(point - vec3(0.0, 0.0, 5.0), vec3(0., 0., -1.)), material1);
	Surface mirror5 = Surface(sdPlane(point - vec3(0.0, 5.0, 0.0), vec3(0., -1., 0.)), material1);
	
	Surface co1 = closestSurface(sphere, sphere2);
	Surface co2 = closestSurface(sphere3, ground);
	Surface co3 = closestSurface(mirror, mirror2);
	Surface co4 = closestSurface(mirror3, mirror4);
	Surface co5 = closestSurface(co4, co3);
	Surface co6 = closestSurface(co5, mirror5);
	return closestSurface(co6, closestSurface(co1, co2));
}

#define EPSILON 0.001
#define RAYMARCH_STEPS 320
#define MAX_DISTANCE 1000.0f
Surface raymarch(out vec3 point, in vec3 ro, in vec3 rd, float tmin, float tmax) {
	float t = tmin;
	vec3 p = vec3(0.0f);
	Surface co;
	for (int i = 0; i < RAYMARCH_STEPS && t < tmax; i++) {
		p = ro + t * rd;
		co = map(p);
		if (co.sd < EPSILON) break;
		t += co.sd;
	}
	point = p;
	return co;
}

vec3 normal(vec3 point) {
	vec2 epsilon = vec2(EPSILON, 0.0);
	float d = map(point).sd;
	vec3 normal = normalize(vec3(
		map(point + epsilon.xyy).sd - d,
		map(point + epsilon.yxy).sd - d,
		map(point + epsilon.yyx).sd - d
	));
	return normal;
}

vec3 blinnPhong(vec3 point, Material material) {
	vec3 normal = normal(point);
	vec3 lightDir = normalize(vec3(4.0f, 3.0f, 2.0f));
	vec3 viewDir = normalize(camera.iCameraPos - point);
	vec3 ambient = material.diffuse * 0.2f;
	
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * material.diffuse;
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
	vec3 specular = spec * material.specular;
	
	return ambient + diffuse + specular;
}

#define MAX_BOUNCES 8
vec3 calculateLighting(vec3 point, Material material) {
	vec3 normal = normal(point);
	vec3 viewDir = normalize(camera.iCameraPos - point);
	
	if (material.isReflective) {		
		vec3 p = point;
		vec3 reflectedDir = normalize(reflect(-viewDir, normal));
		vec3 color = vec3(0.0f);
		float dimFactor = 1.0f;
		for (int i = 0; i < MAX_BOUNCES; i++) {
			vec3 hitp = vec3(0.0f);
			Surface hit = raymarch(hitp, p, reflectedDir, 1.5*EPSILON, MAX_DISTANCE);
			if (hit.sd < EPSILON) {
				dimFactor *= 0.8;
				if (hit.material.isReflective) {
					vec3 incidentDir = normalize(hitp - p);
					reflectedDir = normalize(reflect(incidentDir, normal(hitp)));
					p = hitp;
				}
				else {
					//color = hit.material.diffuse;
					color = dimFactor * blinnPhong(hitp, hit.material);
					return color;
				}
			}
			else {
				color = vec3(0.0f);
				return color;
			}
		}
	}
	else {
		return blinnPhong(point, material);
	}
}

void main() {
	vec2 ndc = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.x;
	vec3 ro = camera.iCameraPos;
	vec3 rd = mat3(inverse(camera.iViewMatrix))*normalize(vec3(ndc, -1.0f));
	vec3 p = vec3(0.0f);
	Surface co = raymarch(p, ro, rd, 0.0, MAX_DISTANCE);
	vec3 color = vec3(0.0f);
	
	if (co.sd < EPSILON) {
		color = calculateLighting(p, co.material);
	}

	gl_FragColor = vec4(color, 1.0f);
}


