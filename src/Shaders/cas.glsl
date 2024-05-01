//
// Contrast Adaptive sharpening
//
// AMD FidelityFX https://gpuopen.com/fidelityfx-cas/
// Adapted from  https://www.shadertoy.com/view/ftsXzM
//
layout(rgba8, binding=0) uniform image2D src;
layout (location = 0) uniform float caswidth;
layout (location = 1) uniform float caslevel;
//
float luminance(in vec3 col)
{
	return dot(vec3(0.2126, 0.7152, 0.0722), col);
}

void main() {
	// Centre pixel (rgba)
	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	// Offsets 1, 2, 3
	float dx = caswidth;
	float dy = caswidth;
	//
	// Neighbourhood
	//
	//     b
	//  a  x  c
	//     d
	//
	// Centre pixel (rgb)
	vec3 col = imageLoad(src, ivec2(gl_GlobalInvocationID.xy)).rgb; // x
	float max_g = luminance(col);
	float min_g = luminance(col);
	//
	vec3 col1;
	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, 0.0)).rgb; // a
	max_g = max(max_g, luminance(col1));
	min_g = min(min_g, luminance(col1));
	vec3 colw = col1;
	//
	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy)).rgb; // b
	max_g = max(max_g, luminance(col1));
	min_g = min(min_g, luminance(col1));
	colw += col1;
	//
	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, 0.0)).rgb; // c
	max_g = max(max_g, luminance(col1));
	min_g = min(min_g, luminance(col1));
	colw += col1;
	//
	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy)).rgb; // d
	max_g = max(max_g, luminance(col1));
	min_g = min(min_g, luminance(col1));
	colw += col1;
	// 
	// CAS algorithm
	//
	float d_min_g = min_g;
	float d_max_g = 1.0-max_g;
	float A;
	if (d_max_g < d_min_g) {
	    A = d_max_g / max_g;
	} else {
	    A = d_min_g / max_g;
	}
	A = sqrt(A);
	A *= mix(-0.125, -0.2, caslevel); // level - CAS level 0-1
	// Sharpened result
	vec3 col_out = (col+colw*A)/(1.0+4.0*A);
	// Output
	imageStore(src, ivec2(gl_GlobalInvocationID.xy), vec4(col_out, c0.a));
}
