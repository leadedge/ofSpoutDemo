//
// Temperature 
// Adapted from : https://www.shadertoy.com/view/ltlcWN
//
layout(rgba8, binding=0) uniform image2D src;
layout(rgba8, binding=1) uniform writeonly image2D dst;
layout(location = 0) uniform float temp;
			
vec3 rgb2hsv(in vec3 c)
{
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);
	vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);
	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}
		
vec3 hsv2rgb(in vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
		
vec3 kelvin2rgb(in float K)
{
	float t = K / 100.0;
	vec3 o1, o2;
	float tg1 = t - 2.;
	float tb1 = t - 10.;
	float tr2 = t - 55.0;
	float tg2 = t - 50.0;
	o1.r = 1.;
	o1.g = (-155.25485562709179 - 0.44596950469579133 * tg1 + 104.49216199393888 * log(tg1)) / 255.;
	o1.b = (-254.76935184120902 + 0.8274096064007395 * tb1 + 115.67994401066147 * log(tb1)) / 255.;
	o1.b = mix(0., o1.b, step(2001., K));
	o2.r = (351.97690566805693 + 0.114206453784165 * tr2 - 40.25366309332127 * log(tr2)) / 255.;
	o2.g = (325.4494125711974 + 0.07943456536662342 * tg2 - 28.0852963507957 * log(tg2)) / 255.;
	o2.b = 1.;
	o1 = clamp(o1, 0., 1.);
	o2 = clamp(o2, 0., 1.);
	return mix(o1, o2, step(66., t));
}
		
vec3 temperature(in vec3 c_in, in float K)
{
	vec3 chsv_in = rgb2hsv(c_in);
	vec3 c_temp = kelvin2rgb(K);
	vec3 c_mult = c_temp * c_in;
	vec3 chsv_mult = rgb2hsv(c_mult);
	return hsv2rgb(vec3(chsv_mult.x, chsv_mult.y, chsv_in.z));
}
		
void main() {
	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));
	vec4 c2 = vec4(temperature(c1.rgb, temp), c1.a);
	// Output
	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), c2);
}
