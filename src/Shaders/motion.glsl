//
// Motion blur
//
// src - last frame
// dst - this frame
//
layout(rgba8, binding=0) uniform image2D src;
layout(rgba8, binding=1) uniform image2D dst;
layout(location = 0) uniform float amount;
void main() {

	vec4 lastframe = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	vec4 thisframe = imageLoad(dst, ivec2(gl_GlobalInvocationID.xy));
	
	// 0.0 - 1.0
	// Limit maximum to 0.98
	// 1.0 is always the last frame and freezes the image
	float amt = amount;
	if(amt > 0.98) amt = 0.98;
	
	vec4 newframe = lastframe*amt + thisframe*(1.0-amt);
	
	// Save the accumulated result for the next frame
	imageStore(src, ivec2(gl_GlobalInvocationID.xy), newframe);
	
	// Display the accumulated result for this frame
	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), newframe);
	
}
