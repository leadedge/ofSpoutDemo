//
// Sharpen - unsharp mask
//
layout(rgba8, binding=0) uniform image2D src;
layout(rgba8, binding=1) uniform writeonly image2D dst;
layout(location = 0) uniform float sharpenwidth;
layout(location = 1) uniform float sharpenstrength;
void main() {
	// Sharpen 
	// Original pixel
	vec4 orig = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	// Get the blur neighbourhood 3x3 or 5x5
	float dx = sharpenwidth;
	float dy = sharpenwidth;
	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, -dy));
	vec4 c2 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, -dy));
	vec4 c3 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, -dy));
	vec4 c4 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, 0.0));
	vec4 c5 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, 0.0));
	vec4 c6 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, dy));
	vec4 c7 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy));
	vec4 c8 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, dy));
	//
	// Gaussian blur filter
	// [ 1, 2, 1 ]
	// [ 2, 4, 2 ]
	// [ 1, 2, 1 ]
	//  c1 c2 c3
	//  c4    c5
	//  c6 c7 c8
	//
	vec4 blur = ((c1 + c3 + c6 + c8) + 2.0 * (c2 + c4 + c5 + c7) + 4.0 * orig) / 16.0;
	// Subtract the blurred image from the original image
	vec4 coeff_blur = vec4(sharpenstrength);
	vec4 coeff_orig = vec4(1.0) + coeff_blur;
	vec4 c9 = coeff_orig * orig - coeff_blur * blur;
	//
	// Output
	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), c9);
}
