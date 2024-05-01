//
// Kuwahara effect
// Adapted from : Jan Eric Kyprianidis (http://www.kyprianidis.com/)
//
layout(rgba8, binding=0) uniform image2D src;
layout(rgba8, binding=1) uniform writeonly image2D dst;
layout(location = 0) uniform float radius;
void main() {
	vec3 m[4];
	vec3 s[4];
	for (int j = 0; j < 4; ++j) {
		m[j] = vec3(0.0);
		s[j] = vec3(0.0);
	}
	
	vec3 c;
	int ir = int(floor(radius));
	for (int j = -ir; j <= 0; ++j) {
		for (int i = -ir; i <= 0; ++i) {
			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;
			m[0] += c;
			s[0] += c * c;
		}
	}
	
	for (int j = -ir; j <= 0; ++j) {
		for (int i = 0; i <= ir; ++i) {
			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;
			m[1] += c;
			s[1] += c * c;
		}
	}

	for (int j = 0; j <= ir; ++j) {
		for (int i = 0; i <= ir; ++i) {
			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;
			m[2] += c;
			s[2] += c * c;
		}
	}

	for (int j = 0; j <= ir; ++j) {
		for (int i = -ir; i <= 0; ++i) {
			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;
			m[3] += c;
			s[3] += c * c;
		}
	}

	float min_sigma2 = 1e+2;
	float n = float((radius+1)*(radius+1));
	for (int k = 0; k < 4; ++k) {
		m[k] /= n;
		s[k] = abs(s[k] / n - m[k] * m[k]);
		float sigma2 = s[k].r + s[k].g + s[k].b;
		if (sigma2 < min_sigma2) {
			min_sigma2 = sigma2;
			imageStore(dst, ivec2(gl_GlobalInvocationID.xy), vec4(m[k], 1.0));
		}
	}
}
