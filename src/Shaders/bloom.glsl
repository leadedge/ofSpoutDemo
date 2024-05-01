//
//    Bloom
//
// Adapted from (Proper gaussian) : https://www.shadertoy.com/view/MtfSDH
// Improved version of https://www.shadertoy.com/view/lsXGWn
//
// Chosen due to smooth change with amount and effectiveness
//
layout(rgba8, binding=0) uniform image2D src;
layout(location = 0) uniform float amount;
void main()
{
    float blurSize = 1.0/512.0;
    ivec2 texcoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 sum = vec4(0);
    sum += imageLoad(src, ivec2(texcoord.x, texcoord.y)) * 0.20;

    sum += imageLoad(src, ivec2(texcoord.x + blurSize, texcoord.y)) * 0.11;
    sum += imageLoad(src, ivec2(texcoord.x - blurSize, texcoord.y)) * 0.11;
    sum += imageLoad(src, ivec2(texcoord.x, texcoord.y + blurSize)) * 0.11;
    sum += imageLoad(src, ivec2(texcoord.x, texcoord.y - blurSize)) * 0.11;

    sum += imageLoad(src, ivec2(texcoord.x + blurSize, texcoord.y + blurSize)) * 0.07;
    sum += imageLoad(src, ivec2(texcoord.x + blurSize, texcoord.y - blurSize)) * 0.07;
    sum += imageLoad(src, ivec2(texcoord.x - blurSize, texcoord.y + blurSize)) * 0.07;
    sum += imageLoad(src, ivec2(texcoord.x - blurSize, texcoord.y - blurSize)) * 0.07;

    sum += imageLoad(src, ivec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.02;
    sum += imageLoad(src, ivec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.02;
    sum += imageLoad(src, ivec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.02;
    sum += imageLoad(src, ivec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.02;
	
    // increase blur with intensity and add to original
    sum = sum*amount + imageLoad(src, texcoord); 
   
    imageStore(src, texcoord, sum);
   
}
