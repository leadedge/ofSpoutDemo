//
// Swap RGBA <> BGRA
//
layout(rgba8, binding=0) uniform image2D src;
void main() {
	// Swap
	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	imageStore(src, ivec2(gl_GlobalInvocationID.xy), vec4(c0.b, c0.g, c0.r, c0.a));
}

			