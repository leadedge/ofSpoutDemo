//
// Mirror in place
//
layout(rgba8, binding=0) uniform image2D src;
layout (location = 0) uniform bool swap;
void main() {
	// Mirror
	if(gl_GlobalInvocationID.x > imageSize(src).x/2)
	    return;
	uint xpos = imageSize(src).x-gl_GlobalInvocationID.x;
	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	vec4 c1 = imageLoad(src, ivec2(xpos, gl_GlobalInvocationID.y));
	if (swap) {
		c0 = vec4(c0.b, c0.g, c0.r, c0.a);
		c1 = vec4(c1.b, c1.g, c1.r, c1.a);
	}
	imageStore(src, ivec2(xpos, gl_GlobalInvocationID.y), c0);
	imageStore(src, ivec2(gl_GlobalInvocationID.xy), c1);
}
