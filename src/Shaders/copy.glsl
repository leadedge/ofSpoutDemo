//
// Texture copy
//
layout(rgba8, binding=0) uniform readonly image2D src;
layout(rgba8, binding=1) uniform writeonly image2D dst;
layout (location = 0) uniform bool flip;
layout (location = 1) uniform bool swap;
void main() {
	// Copy
	vec4 c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	uint ypos = gl_GlobalInvocationID.y;
	if(flip) ypos = imageSize(src).y-ypos; // Flip image option
	//
	// Texture copy with output alpha = 1
	//
	if(swap) {
		// Swap RGBA<>BGRA option
	    imageStore(dst, ivec2(gl_GlobalInvocationID.x, ypos), vec4(c.b,c.g,c.r,c.a));
	}
	else {
	    imageStore(dst, ivec2(gl_GlobalInvocationID.x, ypos), vec4(c.r,c.g,c.b,c.a));
	}
}
