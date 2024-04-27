/*
				SpoutShaders.cpp

		Functions to manage compute shaders

	- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	Copyright (c) 2016-2024, Lynn Jarvis. All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, 
	are permitted provided that the following conditions are met:

		1. Redistributions of source code must retain the above copyright notice, 
		   this list of conditions and the following disclaimer.

		2. Redistributions in binary form must reproduce the above copyright notice, 
		   this list of conditions and the following disclaimer in the documentation 
		   and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"	AND ANY 
	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE	ARE DISCLAIMED. 
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	========================

	14.07.23 - first version
	17.10.23 - Support float GL formats
	18.10.23 - SetGLformat - only for compatible GL formats
	20.10.23 - SetGLformat - add missing GL_RGBA8
	09.11.23 - Add contrast adaptive sharpen
	21.11.23 - ComputeShader
			   Find the maximum number of work group units allowed to limit workgroup size
	19.04.24 - Add colour temperature shader
	24.04.24 - Add support for changing OpenGL format
			   GL_RGBA8, GL_RGBA16, GL_RGBA16F, GL_RGBA32F
			   Add SetShaderSource function to intialise shader strings
	27.04.24 - SetGLformat - change GL_RGBA to GL_RGBA8 for m_GLformat
			   SetShaderSource - correct missing format name for destination
			   Remove CheckShaderFormat

*/

#include "spoutShaders.h"

//
// Class: spoutShaders
//
// Functions to manage compute shaders
//

spoutShaders::spoutShaders() {

	// Initialize shader strings
	SetGLformat(m_GLformat);
	
}


spoutShaders::~spoutShaders() {

	if (m_copyProgram     > 0) glDeleteProgram(m_copyProgram);
	if (m_swapProgram     > 0) glDeleteProgram(m_swapProgram);
	if (m_flipProgram     > 0) glDeleteProgram(m_flipProgram);
	if (m_mirrorProgram   > 0) glDeleteProgram(m_mirrorProgram);
	if (m_brcosaProgram   > 0) glDeleteProgram(m_brcosaProgram);
	if (m_tempProgram     > 0) glDeleteProgram(m_tempProgram);
	if (m_hBlurProgram    > 0) glDeleteProgram(m_hBlurProgram);
	if (m_vBlurProgram    > 0) glDeleteProgram(m_vBlurProgram);
	if (m_sharpenProgram  > 0) glDeleteProgram(m_sharpenProgram);
	if (m_casProgram      > 0) glDeleteProgram(m_casProgram);
	if (m_kuwaharaProgram > 0) glDeleteProgram(m_kuwaharaProgram);
}

//---------------------------------------------------------
// Function: CopyTexture
//
// OpenGL texture copy using compute shader
//    bInvert - flip image
//    bSwap - swap red/blue (RGBA/BGRA)
// Approximately X2 faster than FBO blit
// Textures must have the same size and internal format type GL_RGBA/GL_BGRA/GL_RGBA8
// Texture targets can be different, GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_ARB
// Cannot be used with GL/DX interop
bool spoutShaders::Copy(GLuint SourceID, GLuint DestID,
	unsigned int width, unsigned int height, bool bInvert, bool bSwap)
{
	return ComputeShader(m_copystr, m_copyProgram, SourceID, DestID,
		width, height, bInvert, bSwap);
}

//---------------------------------------------------------
// Function: Flip
//    Flip image in place
bool spoutShaders::Flip(GLuint SourceID, unsigned int width, unsigned int height, bool bSwap)
{
	return ComputeShader(m_flipstr, m_flipProgram, SourceID, 0, width, height, bSwap);
}

//---------------------------------------------------------
// Function: Mirror
//    Mirror image in place
bool spoutShaders::Mirror(GLuint SourceID, unsigned int width, unsigned int height, bool bSwap)
{
	return ComputeShader(m_mirrorstr, m_mirrorProgram, SourceID, 0, width, height);
}

//---------------------------------------------------------
// Function: Swap
//    Texture swap RGBA <> BGRA
bool spoutShaders::Swap(GLuint SourceID, unsigned int width, unsigned int height)
{
	return ComputeShader(m_swapstr, m_swapProgram, SourceID, 0, width, height);
}

//---------------------------------------------------------
// Function: Temperature
//     Colour temperature
//     https://www.shadertoy.com/view/ltlcWN
//     https://en.wikipedia.org/wiki/Color_temperature
//     temp -  3500 - 9500 : default 6500 (daylight)
bool spoutShaders::Temperature(GLuint SourceID, GLuint DestID,
	unsigned int width, unsigned int height, float temp)
{
	return ComputeShader(m_tempstr, m_tempProgram,
		SourceID, DestID, width, height, temp);
}


//---------------------------------------------------------
// Function: Adjust
// Brightness, contrast, saturation, gamma
//    brightness   -1 > 1
//    contrast      0 > 1
//    saturation    0 > 1
//    gamma         0 > 1
bool spoutShaders::Adjust(GLuint SourceID, GLuint DestID, 
	unsigned int width, unsigned int height,
	float brightness, float contrast,
	float saturation, float gamma)
{
	return ComputeShader(m_brcosastr, m_brcosaProgram, SourceID, DestID,
		width, height, brightness, contrast, saturation, gamma);
}

//---------------------------------------------------------
// Function: Blur
//     Two pass Gaussian blur
//     amount - 1 - 4 typical
bool spoutShaders::Blur(GLuint SourceID, GLuint DestID,
	unsigned int width, unsigned int height, float amount)
{
	// Horizontal blur
	if (ComputeShader(m_hblurstr, m_hBlurProgram, SourceID, DestID, width, height, amount)) {
		// Vertical blur on Horizontal blur result
		return ComputeShader(m_vblurstr, m_vBlurProgram, SourceID, DestID, width, height, amount);
	}
	return false;
}

//---------------------------------------------------------
// Function: Sharpen
// Sharpen using unsharp mask
//     sharpenWidth     - 1 3x3, 2 5x5, 3 7x7
//     sharpenStrength  - 1 - 3 typical
bool spoutShaders::Sharpen(GLuint SourceID, GLuint DestID, 
	unsigned int width, unsigned int height,
	float sharpenWidth, float sharpenStrength)
{
	return ComputeShader(m_sharpenstr, m_sharpenProgram, 
		SourceID, DestID,
		width, height,
		sharpenWidth, sharpenStrength);
}

//---------------------------------------------------------
// Function: AdaptiveSharpen
// Sharpen using Contrast Adaptive sharpe algorithm
//     level - 0 > 1
bool spoutShaders::AdaptiveSharpen(GLuint SourceID,
	unsigned int width, unsigned int height, float caswidth, float caslevel)
{
	return ComputeShader(m_casstr, m_casProgram,
		SourceID, 0, width, height, caswidth, caslevel);
}

//---------------------------------------------------------
// Function: Kuwahara
//     Kuwahara filter
//     amount - 1 - 4 typical
bool spoutShaders::Kuwahara(GLuint SourceID, GLuint DestID, 
	unsigned int width, unsigned int height, float amount)
{
	/*
	// Load shader from file for debugging
	if (m_kuwaharastr.empty()) {
		char exepath[MAX_PATH]={};
		GetModuleFileNameA(NULL, exepath, MAX_PATH);
		PathRemoveFileSpecA(exepath);
		strcat_s(exepath, "\\data\\shaders\\kuwahara.txt");
		m_kuwaharastr = GetFileString(exepath);
		// printf("%s\n", m_kuwaharastr.c_str());
	}
	*/
	return ComputeShader(m_kuwaharastr, m_kuwaharaProgram,
		SourceID, DestID, width, height, amount);
}

//---------------------------------------------------------
// Function: SetGLformat
// Set OpenGL format for shaders
// GL_RGBA8, GL_RGBA16, GL_RGBA16F, GL_RGBA32F
// https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)
void spoutShaders::SetGLformat(GLint glformat)
{

	SpoutLogNotice("spoutShaders::SetGLformat(0x%X)", glformat);

	// For final notice of change
	GLint oldformat = m_GLformat;
	std::string oldformatname = m_GLformatName;

	m_GLformat = glformat;

	// GL_RGBA not supported
	if (m_GLformat == GL_RGBA)
		m_GLformat = GL_RGBA8;

	switch (m_GLformat) {
		case GL_RGBA8:
			m_GLformatName = "rgba8";
			break;
		case GL_RGBA16:
			m_GLformatName = "rgba16";
			break;
		case GL_RGBA16F:
			m_GLformatName = "rgba16f";
			break;
		case GL_RGBA32F:
			m_GLformatName = "rgba32f";
			break;
		default:
			break;
	}

	// All shaders have to be re-compiled
	if (m_copyProgram     > 0) glDeleteProgram(m_copyProgram);
	if (m_swapProgram     > 0) glDeleteProgram(m_swapProgram);
	if (m_flipProgram     > 0) glDeleteProgram(m_flipProgram);
	if (m_mirrorProgram   > 0) glDeleteProgram(m_mirrorProgram);
	if (m_brcosaProgram   > 0) glDeleteProgram(m_brcosaProgram);
	if (m_tempProgram     > 0) glDeleteProgram(m_tempProgram);
	if (m_hBlurProgram    > 0) glDeleteProgram(m_hBlurProgram);
	if (m_vBlurProgram    > 0) glDeleteProgram(m_vBlurProgram);
	if (m_sharpenProgram  > 0) glDeleteProgram(m_sharpenProgram);
	if (m_casProgram      > 0) glDeleteProgram(m_casProgram);
	if (m_kuwaharaProgram > 0) glDeleteProgram(m_kuwaharaProgram);
	m_copyProgram     = 0;
	m_swapProgram     = 0;
	m_flipProgram     = 0;
	m_mirrorProgram   = 0;
	m_brcosaProgram   = 0;
	m_tempProgram     = 0;
	m_casProgram      = 0;
	m_hBlurProgram    = 0;
	m_vBlurProgram    = 0;
	m_sharpenProgram  = 0;
	m_kuwaharaProgram = 0;

	// Reset all shader strings
	SetShaderSource(m_GLformatName);

	if (m_GLformat != oldformat) {
		SpoutLogNotice("spoutShaders::SetGLformat - shader format reset from 0x%X (%s) to 0x%X (%s)",
			oldformat, oldformatname.c_str(), m_GLformat, m_GLformatName.c_str());
	}

}

//---------------------------------------------------------
// Function: ComputeShader
//    Apply compute shader on source to dest
//    or to read/write source with provided uniforms
bool spoutShaders::ComputeShader(std::string &shaderstr, GLuint &program,
	GLuint SourceID, GLuint DestID, unsigned int width, unsigned int height,
	float uniform0, float uniform1, float uniform2, float uniform3)
{
	if (shaderstr.empty() || SourceID == 0) {
		SpoutLogWarning("spoutShaders::ComputeShader - no shader or texture");
		return false;
	}

	// Find the maximum number of work group units allowed
	int workgroups = 0;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroups);
	// wgx*wgy must be less that max workgroup invocations
	unsigned int nWgX = (unsigned int)sqrt((float)workgroups);
	unsigned int nWgY = nWgX;

	// The X and Y work group sizes should match image width and height
	// Adjust down for aspect ratio
	if (width >= height) {
		nWgX = width/(unsigned int)ceil((float)width/(float)nWgX);
		nWgY = nWgX * height/width;
	}
	else {
		nWgY = height/(unsigned int)ceil((float)height/(float)nWgY);
		nWgX = nWgY * width/height;
	}

	if (program == 0) {
		program = CreateComputeShader(shaderstr, nWgX, nWgY);
		if (program == 0) {
			SpoutLogWarning("spoutShaders::ComputeShader - CreateComputeShader failed");
			return false;
		}
		SpoutLogNotice("spoutShaders::ComputeShader - created program (%d)", program);

	}

	glUseProgram(program);
	glBindImageTexture(0, SourceID, 0, GL_FALSE, 0, GL_READ_WRITE, m_GLformat);
	if(DestID > 0)
	  glBindImageTexture(1, DestID, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_GLformat);
	if (uniform0 != -1.0) glUniform1f(0, uniform0);
	if (uniform1 != -1.0) glUniform1f(1, uniform1);
	if (uniform2 != -1.0) glUniform1f(2, uniform2);
	if (uniform3 != -1.0) glUniform1f(3, uniform3);
	glDispatchCompute(width/nWgX, height/nWgY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_READ_WRITE, m_GLformat);
	glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, m_GLformat);
	glUseProgram(0);

	return true;

}

//---------------------------------------------------------
// Function: CreateComputeShader
// Create compute shader from a source string
unsigned int spoutShaders::CreateComputeShader(std::string shader, unsigned int nWgX, unsigned int nWgY)
{
	// Compute shaders are only supported since openGL 4.3
	int major = 0;
	int minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	float version = (float)major + (float)minor / 10.0f;
	if (version < 4.3f) {
		SpoutLogError("CreateComputeShader - OpenGL version > 4.3 required");
		return 0;
	}

	//
	// Compute shader source initialized in header
	// See also GetFileString for testing
	//

	// Common
	std::string shaderstr = "#version 440\n";
	shaderstr += "layout(local_size_x = ";
	shaderstr += std::to_string(nWgX);
	shaderstr += ", local_size_y = ";
	shaderstr += std::to_string(nWgY);
	shaderstr += ", local_size_z = 1) in;\n";
	
	// Full shader string
	shaderstr += shader;

	// Create the compute shader program
	GLuint computeProgram = glCreateProgram();
	if (computeProgram > 0) {
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
		if (computeShader > 0) {
			// Compile and link shader
			GLint status = 0;
			const char* source = shaderstr.c_str();
			glShaderSource(computeShader, 1, &source, NULL);
			glCompileShader(computeShader);
			glAttachShader(computeProgram, computeShader);
			glLinkProgram(computeProgram);

			// Link error log
			GLsizei length = 0;
			glGetProgramiv(computeProgram, GL_INFO_LOG_LENGTH, &length);
			if (length > 0) {
				char infoLog[1024]{};
				glGetProgramInfoLog(computeProgram, 1024, &length, infoLog);
				if (length > 0)
					SpoutLogWarning("CreateComputeShader - link error log\n%s", infoLog);
			}

			glGetProgramiv(computeProgram, GL_LINK_STATUS, &status);
			if (status == GL_FALSE) {
				SpoutLogError("CreateComputeShader - linking failed");
				glDetachShader(computeProgram, computeShader);
				glDeleteProgram(computeShader);
				glDeleteProgram(computeProgram);
				return 0;
			}
			else {
				// After linking, the shader object is not needed
				glDeleteShader(computeShader);
				return computeProgram;
			}
		}
		else {
			SpoutLogError("CreateComputeShader - glCreateShader failed");
			return 0;
		}
	}
	SpoutLogError("CreateComputeShader - glCreateProgram failed");
	return 0;
}


//---------------------------------------------------------
// Function: GetFileString
// Load complete shader source from file
// Used for development.
std::string spoutShaders::GetFileString(const char* filepath)
{
	if (!filepath || !*filepath) return "";

	std::string path = filepath;
	std::string logstr = "";

	if (!path.empty()) {
		// Does the file exist
		if (_access(path.c_str(), 0) != -1) {
			// Open the file
			std::ifstream logstream(path);
			// File loaded OK ?
			if (logstream.is_open()) {
				// Get the file text as a single string
				logstr.assign((std::istreambuf_iterator< char >(logstream)), std::istreambuf_iterator< char >());
				logstr += ""; // ensure a NULL terminator
				logstream.close();
			}
		}
		else {
			SpoutLogError("GetFileString [%s] not found", path.c_str());
		}
	}
	return logstr;
}


//---------------------------------------------------------
// Function: SetShaderSource
// Set all shader source strings
// Used when the format changes
void spoutShaders::SetShaderSource(std::string GLformatName)
{
	SpoutLogNotice("spoutShaders::SetShaderSource(%s)", GLformatName.c_str());

	//
	// Texture copy
	//
	m_copystr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout (location = 0) uniform bool flip;\n"
		"layout (location = 1) uniform bool swap;\n"
		"void main() {\n"
		"	// Copy \n"
		"	vec4 c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	uint ypos = gl_GlobalInvocationID.y;\n"
		"	if(flip) ypos = imageSize(src).y-ypos;\n" // Flip image option
			// Texture copy with output alpha = 1
		"	if(swap) {\n" // Swap RGBA<>BGRA option
		"		imageStore(dst, ivec2(gl_GlobalInvocationID.x, ypos), vec4(c.b,c.g,c.r,c.a));\n"
		"	}\n"
		"	else {\n"
		"		imageStore(dst, ivec2(gl_GlobalInvocationID.x, ypos), vec4(c.r,c.g,c.b,c.a));\n"
		"	}\n"
		"}";

	//
	// Flip in place
	//
	m_flipstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout (location = 0) uniform bool swap;\n"
		"void main() {\n"
		"	// Flip \n"
		"	if(gl_GlobalInvocationID.y > imageSize(src).y/2)\n" // Half image
		"		return;\n"
		"	uint ypos = imageSize(src).y-gl_GlobalInvocationID.y;\n" // Flip y position
		"	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n" // This pixel
		"	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.x, ypos));\n" // Flip pixel
		"	if (swap) {\n" // Swap RGBA<>BGRA option
		"		c0 = vec4(c0.b, c0.g, c0.r, c0.a);\n"
		"		c1 = vec4(c1.b, c1.g, c1.r, c1.a);\n"
		"	}\n"
		"	imageStore(src, ivec2(gl_GlobalInvocationID.x, ypos), c0);\n" // Move this pixel to flip position
		"	imageStore(src, ivec2(gl_GlobalInvocationID.xy), c1);\n"  // Move flip pixel to this position
		"}";

	//
	// Mirror in place
	//
	m_mirrorstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout (location = 0) uniform bool swap;\n"
		"void main() {\n"
		"	// Mirror \n"
		"	if(gl_GlobalInvocationID.x > imageSize(src).x/2)\n"
		"		return;\n"
		"	uint xpos = imageSize(src).x-gl_GlobalInvocationID.x;\n"
		"	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	vec4 c1 = imageLoad(src, ivec2(xpos, gl_GlobalInvocationID.y));\n"
		"	if (swap) {\n"
		"		c0 = vec4(c0.b, c0.g, c0.r, c0.a);\n"
		"		c1 = vec4(c1.b, c1.g, c1.r, c1.a);\n"
		"	}\n"
		"	imageStore(src, ivec2(xpos, gl_GlobalInvocationID.y), c0);\n"
		"	imageStore(src, ivec2(gl_GlobalInvocationID.xy), c1);\n"
		"}";

	//
	// Swap RGBA <> BGRA
	//
	m_swapstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"void main() {\n"
		"	// Swap \n"
		"	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	imageStore(src, ivec2(gl_GlobalInvocationID.xy), vec4(c0.b, c0.g, c0.r, c0.a));\n"
		"}";

	//
	// Adjust - brightness, contrast, saturation, gamma, temp
	//
	m_brcosastr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n" // Read/Write
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n" // Write only
		"layout(location = 0) uniform float brightness;\n"
		"layout(location = 1) uniform float contrast;\n"
		"layout(location = 2) uniform float saturation;\n"
		"layout(location = 3) uniform float gamma;\n"
		"void main() {\n"
		"	// Adjust \n"
		"	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	\n"
			// Gamma (0 > 10) default 1
		"	vec3 c2 = pow(c1.rgb, vec3(1.0 / gamma)); // rgb\n"
		"	\n"
			// Saturation (0 > 3) default 1
		"	float luminance = dot(c2, vec3(0.2125, 0.7154, 0.0721)); // weights sum to 1\n"
		"	c2 = mix(vec3(luminance), c2, vec3(saturation));\n"
		"	\n"
			// Contrast (0 > 2) default
		"	c2 = (c2 - 0.5) * contrast + 0.5;\n"
		"	\n"
			// Brightness (-1 > 1) default 0
		"	c2 += brightness;\n"
		"	\n"
			// Output with original alpha
		"	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), vec4(c2, c1.a)); \n"
		"}\n";


	//
	// Temperature 
	// https://www.shadertoy.com/view/ltlcWN
	//
	m_tempstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout(location = 0) uniform float temp;\n"
		"\n"
		"vec3 rgb2hsv(in vec3 c)\n"
		"{\n"
		"	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
		"	vec4 p = c.g < c.b ? vec4(c.bg, K.wz) : vec4(c.gb, K.xy);\n"
		"	vec4 q = c.r < p.x ? vec4(p.xyw, c.r) : vec4(c.r, p.yzx);\n"
		"	float d = q.x - min(q.w, q.y);\n"
		"	float e = 1.0e-10;\n"
		"	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
		"}\n"
		"\n"
		"vec3 hsv2rgb(in vec3 c)\n"
		"{\n"
		"	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
		"	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
		"	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
		"}\n"
		"\n"
		"vec3 kelvin2rgb(in float K)\n"
		"{\n"
		"	float t = K / 100.0;\n"
		"	vec3 o1, o2;\n"
		"	float tg1 = t - 2.;\n"
		"	float tb1 = t - 10.;\n"
		"	float tr2 = t - 55.0;\n"
		"	float tg2 = t - 50.0;\n"
		"	o1.r = 1.;\n"
		"	o1.g = (-155.25485562709179 - 0.44596950469579133 * tg1 + 104.49216199393888 * log(tg1)) / 255.;\n"
		"	o1.b = (-254.76935184120902 + 0.8274096064007395 * tb1 + 115.67994401066147 * log(tb1)) / 255.;\n"
		"	o1.b = mix(0., o1.b, step(2001., K));\n"
		"	o2.r = (351.97690566805693 + 0.114206453784165 * tr2 - 40.25366309332127 * log(tr2)) / 255.;\n"
		"	o2.g = (325.4494125711974 + 0.07943456536662342 * tg2 - 28.0852963507957 * log(tg2)) / 255.;\n"
		"	o2.b = 1.;\n"
		"	o1 = clamp(o1, 0., 1.);\n"
		"	o2 = clamp(o2, 0., 1.);\n"
		"	return mix(o1, o2, step(66., t));\n"
		"}\n"
		"\n"
		"vec3 temperature(in vec3 c_in, in float K)\n"
		"{\n"
		"	vec3 chsv_in = rgb2hsv(c_in);\n"
		"	vec3 c_temp = kelvin2rgb(K);\n"
		"	vec3 c_mult = c_temp * c_in;\n"
		"	vec3 chsv_mult = rgb2hsv(c_mult);\n"
		"	return hsv2rgb(vec3(chsv_mult.x, chsv_mult.y, chsv_in.z));\n"
		"}\n"
		"\n"
		"void main() {\n"
		"	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	vec4 c2 = vec4(temperature(c1.rgb, temp), c1.a);\n"
		"	// Output \n"
		"	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), c2);\n"
		"}\n";

	//
	// Sharpen - unsharp mask
	//
	m_sharpenstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout(location = 0) uniform float sharpenwidth;\n"
		"layout(location = 1) uniform float sharpenstrength;\n"
		"\n"
		"void main() {\n"
		"	// Sharpen \n"
		// Original pixel
		"	vec4 orig = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		"	\n"
		// Get the blur neighbourhood 3x3 or 5x5
		"	float dx = sharpenwidth;\n"
		"	float dy = sharpenwidth;\n"
		"	\n"
		"	vec4 c1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, -dy));\n"
		"	vec4 c2 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, -dy));\n"
		"	vec4 c3 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, -dy));\n"
		"	vec4 c4 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, 0.0));\n"
		"	vec4 c5 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, 0.0));\n"
		"	vec4 c6 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, dy));\n"
		"	vec4 c7 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy));\n"
		"	vec4 c8 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, dy));\n"
		"	\n"
		// Gaussian blur filter
		// [ 1, 2, 1 ]
		// [ 2, 4, 2 ]
		// [ 1, 2, 1 ]
		//  c1 c2 c3
		//  c4    c5
		//  c6 c7 c8
		"	vec4 blur = ((c1 + c3 + c6 + c8) + 2.0 * (c2 + c4 + c5 + c7) + 4.0 * orig) / 16.0;\n"
		// Subtract the blurred image from the original image
		"	vec4 coeff_blur = vec4(sharpenstrength);\n"
		"	vec4 coeff_orig = vec4(1.0) + coeff_blur;\n"
		"	vec4 c9 = coeff_orig * orig - coeff_blur * blur;\n"
		"	\n"
		// Output
		"	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), c9);\n"
		"}\n";

	//
	// Gaussian blur
	// Adapted from Openframeworks "09_gaussianBlurFilter" example
	// https://openframeworks.cc/
	//

	//
	// Horizontal Gaussian blur
	//
	m_hblurstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout(location = 0) uniform float amount;\n"
		"\n"
		"void main() {\n"
		"	// H blur\n"
		"	vec4 c1 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*-4.0, 0.0));\n"
		"	vec4 c2 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*-3.0, 0.0));\n"
		"	vec4 c3 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*-2.0, 0.0));\n"
		"	vec4 c4 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*-1.0, 0.0));\n"
		"	vec4 c5 = 0.382928 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, 0.0));\n"
		"	vec4 c6 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*1.0, 0.0));\n"
		"	vec4 c7 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*2.0, 0.0));\n"
		"	vec4 c8 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*3.0, 0.0));\n"
		"	vec4 c9 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(amount*4.0, 0.0));\n"
		"	\n"
		// Output
		"	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9));\n"
		"	\n"
		"}\n";

	//
	// Vertical Gaussian blur
	//
	m_vblurstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout(location = 0) uniform float amount;\n"
		"\n"
		"void main() {\n"
		"	// V blur\n"
		"	vec4 c1 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-4.0));\n"
		"	vec4 c2 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-3.0));\n"
		"	vec4 c3 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-2.0));\n"
		"	vec4 c4 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*-1.0));\n"
		"	vec4 c5 = 0.382928 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, 0.0));\n"
		"	vec4 c6 = 0.241732 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*1.0));\n"
		"	vec4 c7 = 0.060598 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*2.0));\n"
		"	vec4 c8 = 0.005977 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*3.0));\n"
		"	vec4 c9 = 0.000229 * imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, amount*4.0));\n"
		"	\n"
		// Output
		"	imageStore(dst, ivec2(gl_GlobalInvocationID.xy), (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 + c9));\n"
		"	\n"
		"}\n";

	//
	// Contrast Adaptive sharpening
	//
	// AMD FidelityFX https://gpuopen.com/fidelityfx-cas/
	// Adapted from  https://www.shadertoy.com/view/ftsXzM
	//
	m_casstr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout (location = 0) uniform float caswidth;\n"
		"layout (location = 1) uniform float caslevel;\n"
		//
		"float luminance(in vec3 col)\n"
		"{\n"
		"	return dot(vec3(0.2126, 0.7152, 0.0722), col);\n"
		"}\n"
		"void main() {\n"
		// Centre pixel (rgba)
		"	vec4 c0 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy));\n"
		// Offsets 1, 2, 3
		"	float dx = caswidth;\n"
		"	float dy = caswidth;\n"
		//
		// Neighbourhood
		//
		//     b
		//  a  x  c
		//     d
		//
		// Centre pixel (rgb)
		"	vec3 col = imageLoad(src, ivec2(gl_GlobalInvocationID.xy)).rgb;\n" // x
		"	float max_g = luminance(col);\n"
		"	float min_g = luminance(col);\n"
		//
		"	vec3 col1;\n"
		"	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(-dx, 0.0)).rgb;\n" // a
		"	max_g = max(max_g, luminance(col1));\n"
		"	min_g = min(min_g, luminance(col1));\n"
		"	vec3 colw = col1;\n"
		//
		"	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy)).rgb;\n" // b
		"	max_g = max(max_g, luminance(col1));\n"
		"	min_g = min(min_g, luminance(col1));\n"
		"	colw += col1;\n"
		//
		"	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(dx, 0.0)).rgb;\n" // c
		"	max_g = max(max_g, luminance(col1));\n"
		"	min_g = min(min_g, luminance(col1));\n"
		"	colw += col1;\n"
		//
		"	col1 = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(0.0, dy)).rgb;\n" // d
		"	max_g = max(max_g, luminance(col1));\n"
		"	min_g = min(min_g, luminance(col1));\n"
		"	colw += col1;\n"
		// 
		// CAS algorithm
		//
		"	float d_min_g = min_g;\n"
		"	float d_max_g = 1.0-max_g;\n"
		"	float A;\n"
		"	if (d_max_g < d_min_g) {\n"
		"		 A = d_max_g / max_g;\n"
		"	} else {\n"
		"		 A = d_min_g / max_g;\n"
		"	}\n"
		"	A = sqrt(A);\n"
		"	A *= mix(-0.125, -0.2, caslevel);\n" // level - CAS level 0-1
		// Sharpened result
		"	vec3 col_out = (col+colw*A)/(1.0+4.0*A);\n"
		// Output
		"	imageStore(src, ivec2(gl_GlobalInvocationID.xy), vec4(col_out, c0.a));\n"
		"}";

	//
	// Kuwahara effect
	// Adapted from : Jan Eric Kyprianidis (http://www.kyprianidis.com/)
	//
	m_kuwaharastr = "layout(" + GLformatName + ", binding=0) uniform image2D src;\n"
		"layout(" + GLformatName + ", binding=1) uniform writeonly image2D dst;\n"
		"layout(location = 0) uniform float radius;\n"
		"\n"
		"void main() {\n"
		"\n"
		"	vec3 m[4];\n"
		"	vec3 s[4];\n"
		"	for (int j = 0; j < 4; ++j) {\n"
		"		m[j] = vec3(0.0);\n"
		"		s[j] = vec3(0.0);\n"
		"	}\n"
		"\n"
		"	vec3 c;\n"
		"	int ir = int(floor(radius));\n"
		"	for (int j = -ir; j <= 0; ++j) {\n"
		"		for (int i = -ir; i <= 0; ++i) {\n"
		"			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;\n"
		"			m[0] += c;\n"
		"			s[0] += c * c;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	for (int j = -ir; j <= 0; ++j) {\n"
		"		for (int i = 0; i <= ir; ++i) {\n"
		"			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;\n"
		"			m[1] += c;\n"
		"			s[1] += c * c;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	for (int j = 0; j <= ir; ++j) {\n"
		"		for (int i = 0; i <= ir; ++i) {\n"
		"			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;\n"
		"			m[2] += c;\n"
		"			s[2] += c * c;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	for (int j = 0; j <= ir; ++j) {\n"
		"		for (int i = -ir; i <= 0; ++i) {\n"
		"			c = imageLoad(src, ivec2(gl_GlobalInvocationID.xy) + ivec2(i, j)).rgb;\n"
		"			m[3] += c;\n"
		"			s[3] += c * c;\n"
		"		}\n"
		"	}\n"
		"\n"
		"	float min_sigma2 = 1e+2;\n"
		"	float n = float((radius+1)*(radius+1));\n"
		"	for (int k = 0; k < 4; ++k) {\n"
		"		m[k] /= n;\n"
		"		s[k] = abs(s[k] / n - m[k] * m[k]);\n"
		"		float sigma2 = s[k].r + s[k].g + s[k].b;\n"
		"		if (sigma2 < min_sigma2) {\n"
		"			min_sigma2 = sigma2;\n"
		"			imageStore(dst, ivec2(gl_GlobalInvocationID.xy), vec4(m[k], 1.0));\n"
		"		}\n"
		"	}\n"
		"}\n";
}

// =====================================================================================
