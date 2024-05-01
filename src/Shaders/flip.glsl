//
// Flip in place
//
layout(rgba8, binding=0) uniform image2D src;
layout (location = 0) uniform bool swap;
void main() {
	// Flip
	if(gl_GlobalInvocationID.y > imageSize(src).y/2) // Half image
	    return;
	uint ypos = imageSize(src).y-gl_GlobalInvocationID.y; // Flip y position
	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy)); // This pixel
	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.x, ypos)); // Flip pixel
	if (swap) { // Swap RGBA<>BGRA option
		c0 = vec4(c0.b, c0.g, c0.r, c0.a);
		c1 = vec4(c1.b, c1.g, c1.r, c1.a);
	}
	imageStore(src, ivec2(gl_GlobalInvocationID.x, ypos), c0); // Move this pixel to flip position
	imageStore(src, ivec2(gl_GlobalInvocationID.xy), c1);  // Move flip pixel to this position
}
