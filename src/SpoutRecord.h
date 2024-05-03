//
// spoutRecord.h
//

#pragma once

#ifndef __spoutRecord__
#define __spoutRecord__

#include <windows.h>
#include <stdio.h>
#include <iostream>

#define SPOUTGL

// Change path as necessary
#ifdef SPOUTGL
#include "..\..\..\SpoutGL\Spout.h"
#else
#include "SpoutDX\SpoutDX.h"
#endif


class spoutRecord {

public:

	spoutRecord();
	~spoutRecord();

	// FFmpeg start recording
	bool Start(std::string ffmpegPath, std::string OutputFile,
		unsigned int width, unsigned int height, bool bRgba = false);

	// FFmpeg stop recording
	void Stop();

#ifdef SPOUTGL
	// Write pixels of an OpenGL texture image frame
	bool Write(GLuint TextureID, GLuint TextureTarget, unsigned int width, unsigned int height);
#else
	// Set the D3D11 device for recorder functions
	bool OpenDirectX(ID3D11Device* pDevice = nullptr);
	// Write pixels of an D3D11 texture image frame
	bool Write(ID3D11Texture2D* pTexture);
#endif

	// FFmpeg write pixel buffer
	bool Write(unsigned char* pixelBuffer, unsigned int nBytes);

	// Return encoding status
	bool IsEncoding();

	// Enable / disable system audio recording
	void EnableAudio(bool bAudio = true);

	// Set codec for recording
	// 0-mpeg4, 1-libx264, 2-libx265, 3-h264_nvenc, 4-hevc_nvenc, 5-hap 
	void SetCodec(int codec = 0);

	// Set user defined codec string for FFmpeg
	void SetCodec(std::string codecString);

	// Set user defined container extension for output
	void SetExtension(std::string extension);

	// Set x264 preset
	// 0 - ultrafast, 1 - superfast, 2 - veryfast, 3 - faster
	void SetPreset(int preset = 0);

	// Set x264 quality (crf)
	// 0 - low, 1 - medium, 2 - high
	void SetQuality(int quality);

	// Set chroma subsampling
	// 0 - low 4:2:0, 1 - medium 4:2:2, 2 - high 4:4:4
	void SetChroma(int chroma);

	// Set x264 constant rate factor - CRF
	//    0 - 51
	//    Default - 23
	//    Visually lossless - 18
	void SetRate(int rate = 23);

	// Set frame rate for FFmpeg output
	// Producing rate should be the same
	void SetFps(int fps);

private:

#ifdef SPOUTGL
	Spout spout;
#else
	spoutDX spout;
#endif
	FILE* m_FFmpeg = nullptr; // FFmpeg pipe for recording
	FILE* m_pCout = nullptr;
	HWND m_hConsole = NULL;
	unsigned char* m_pixelBuffer = nullptr; // Receiving pixel buffer
	unsigned int m_nBytes=0;
	bool m_bAudio = false; // system audio
	int m_codec = 0; // 0 - mp4, 1 - h264
	int m_Preset = 0; // x264 preset
	int m_Quality = 0; // x264 quality
	int m_Chroma = 2; // 4:2:0, 4:2:2, 4:4:4
	int m_Crf = 23; // x264 rate factor (crf)
	int m_FPS = 30; // input and output frame rates must match
	std::string m_FFmpegCodec; // User FFmpeg codec args
	std::string m_FileExt = "mp4";

};

#endif
