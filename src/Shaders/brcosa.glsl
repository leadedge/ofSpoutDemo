//
// Adjust - brightness, contrast, saturation, gamma
//
layout(rgba8, binding=0) uniform image2D src; // Read/Write
layout(rgba8, binding=1) uniform writeonly image2D dst; // Write only
layout(location = 0) uniform float brightness;
layout(location = 1) uniform float contrast;
layout(location = 2) uniform float saturation;
layout(location = 3) uniform float gamma;
void main() {
	// Adjust
	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	// Gamma (0 > 10) default 1
	vec3 c2 = pow(c1.rgb, vec3(1.0 / gamma)); // rgb
	// Saturation (0 > 3) default 1
	float luminance = dot(c2, vec3(0.2125, 0.7154, 0.0721)); // weights sum to 1
	c2 = mix(vec3(luminance), c2, vec3(saturation));
	// Contrast (0 > 2) default
	c2 = (c2 - 0.5) * contrast + 0.5;
	// Brightness (-1 > 1) default 0
	c2 += brightness;
	// Output with original alpha
	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), vec4(c2, c1.a));
}
