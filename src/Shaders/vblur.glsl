//
// Gaussian blur
// Adapted from Openframeworks "09_gaussianBlurFilter" example
// https://openframeworks.cc/
//
// Vertical Gaussian blur
// See also horizontal blur
//
layout(rgba8, binding=0) uniform image2D src;
layout(rgba8, binding=1) uniform writeonly image2D dst;
layout(location = 0) uniform float amount;
void main() {
	// V blur
	vec4 c1 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-4.0));
	vec4 c2 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-3.0));
	vec4 c3 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-2.0));
	vec4 c4 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-1.0));
	vec4 c5 = 0.382928 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, 0.0));
	vec4 c6 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*1.0));
	vec4 c7 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*2.0));
	vec4 c8 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*3.0));
	vec4 c9 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*4.0));
	// Output
	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9));
}
