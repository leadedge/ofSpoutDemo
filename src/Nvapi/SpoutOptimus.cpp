#include "SpoutOptimus.h"

//
//	SpoutOptimus.cpp
//
//	Set NVIDIA graphics as Global for all applications
//
//	Based on : http://onemanmmo.com/index.php?cmd=newsitem&comment=news.1.211.0
//
//	http://www.codegur.net/17270429/forcing-hardware-accelerated-rendering
//
//	Uses NVAPI to load the base profile and save it to a file to restore later.
//	The base profile is then modified and applied to the system. 
//	The application has to be re-started before the settings take effect,
//	but other apps started subsequently should use the modified base settings.
//

// 11.02.17 - return driver number and driver-branch string


nVidia::nVidia() {
	hSession = 0;
	hProfile = 0;
	status = NVAPI_ERROR;
}

nVidia::~nVidia() { }

	bool nVidia::IsLaptop(bool *integrated)
	{
		bool bOptimus = false;
		bool bLaptop = false;
		bool bIntegrated = false;

		// (0) Initialize NVAPI. This must be done first of all
		status = NvAPI_Initialize();
		if (status != NVAPI_OK) {
			// will fail silently if not supported
			printf("nVidia::IsLaptop : NvAPI_Initialize error\n");
			return false;
		}

		// Then, we need to get the handle for the system's physical GPU.
		// The NvAPI_EnumPhysicalGPUs function accomplishes this.
		// We then go through all the handles and use NvAPI_GPU_GetSystemType()
		// to check if the GPU is a laptop GPU.
		// If it is, we have to check whether the GPU is discrete
		// using NvAPI_GPU_GetGPUType(). 

		// For reference, here's the documentation for those NVApi functions:
		// http://docs.nvidia.com/gameworks/content/gameworkslibrary/coresdk/nvapi/group__gpu.html

		// NVAPI_INTERFACE NvAPI_EnumPhysicalGPUs (NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *pGpuCount)
		// This function returns an array of physical GPU handles.
		// Each handle represents a physical GPU present in the system.
		// That GPU may be part of an SLI configuration, or may not be visible
		// to the OS directly.
		// At least one GPU must be present in the system and
		// running an NVIDIA display driver.
		// The array nvGPUHandle will be filled with physical GPU handle values.
		// The returned gpuCount determines how many entries in the array are valid.
		NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
		NvU32 GpuCount = 0;
		status = NvAPI_EnumPhysicalGPUs(nvGPUHandle, &GpuCount);
		if (status != NVAPI_OK) {
			printf("nVidia::IsLaptop : NvAPI_EnumPhysicalGPUs error\n");
			return false;
		}
		// printf("nVidia::IsLaptop : GPuCount = %d\n", GpuCount);

		// Go through all the handles and use NvAPI_GPU_GetSystemType()
		// to check if the GPU is a laptop GPU.
		// NvAPI_GPU_GetSystemType (NvPhysicalGpuHandle hPhysicalGpu, NV_SYSTEM_TYPE * pSystemType)
		// This function identifies whether the GPU is a notebook GPU or a desktop GPU.
		// Return values
		// NVAPI_INVALID_ARGUMENT : hPhysicalGpu or pOutputsMask is NULL
		// NVAPI_OK	: *pSystemType contains the GPU system type
		// NVAPI_NVIDIA_DEVICE_NOT_FOUND : No NVIDIA GPU driving a display was found
		// NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE : hPhysicalGpu was not a physical GPU handle
		//
		NV_SYSTEM_TYPE SystemType = NV_SYSTEM_TYPE_UNKNOWN;
		// NV_SYSTEM_TYPE_UNKNOWN = 0,
		// NV_SYSTEM_TYPE_LAPTOP = 1,
		// NV_SYSTEM_TYPE_DESKTOP = 2,
		for (unsigned int i = 0; i < GpuCount; i++) {
			if (NvAPI_GPU_GetSystemType(nvGPUHandle[i], &SystemType) == NVAPI_OK) {
				/*
				printf("System type = %d\n", SystemType);
				if (SystemType == NV_SYSTEM_TYPE_UNKNOWN)
					printf("Unknown System type\n");
				if (SystemType == NV_SYSTEM_TYPE_LAPTOP)
					printf("Laptop system\n");
				if (SystemType == NV_SYSTEM_TYPE_DESKTOP)
					printf("Desktop system\n");
				*/
				if (SystemType == NV_SYSTEM_TYPE_LAPTOP) {

					bLaptop = true; // Shows it is a laptop.

					// we have to check whether the GPU is discrete using NvAPI_GPU_GetGPUType()
					// NVAPI_INTERFACE NvAPI_GPU_GetGPUType(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_TYPE * pGpuType);
					// This function returns the GPU type(integrated or discrete).
					//    0 - NV_SYSTEM_TYPE_GPU_UNKNOWN 	
					//    1 - NV_SYSTEM_TYPE_IGPU : Integrated GPU.
					//    2 - NV_SYSTEM_TYPE_DGPU : Discrete GPU.
					//
					NV_GPU_TYPE GpuType = NV_SYSTEM_TYPE_GPU_UNKNOWN;
					status = NvAPI_GPU_GetGPUType(nvGPUHandle[i], &GpuType);
					if (status == NVAPI_OK) {
						/*
						printf("GpuType = %d\n", GpuType);
						if (GpuType == NV_SYSTEM_TYPE_GPU_UNKNOWN)
							printf("Unknown GPU type\n");
						if (GpuType == NV_SYSTEM_TYPE_IGPU)
							printf("Integrated GPU\n");
						if (GpuType == NV_SYSTEM_TYPE_GPU_UNKNOWN)
							printf("Discrete GPU\n");
						*/

						// TODO : what does integrated mean
						// Using the integrate GPU or has an integrated GPU ?
						if (GpuType == NV_SYSTEM_TYPE_IGPU) {
							bIntegrated = true;
							bOptimus = true;
						}

					}
					
				}
			}
			*integrated = bIntegrated;
		}

		NvAPI_Unload();

		// printf("nVidia::IsOptimus : Optimus graphics = %d\n", bOptimus);
		// printf("nVidia::IsLaptop = %d (integrated = %d)\n", bLaptop, bIntegrated);

		return bLaptop;
	}

	//
	// 0 - high preformance
	// 1 - integrated
	// 2 - auto select
	//
	bool nVidia::ActivateNVIDIA(int index)
	{
		
		// ====================================================
		// (0) Initialize NVAPI. This must be done first of all
		status = NvAPI_Initialize();
		if (status != NVAPI_OK) {
			// will fail silently if not supported
			printf("nVidia::ActivateNVIDIA : NvAPI_Initialize error\n");
			return false;
		}

		// (1) Create the session handle to access driver settings
		hSession = 0;
		status = NvAPI_DRS_CreateSession(&hSession);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_CreateSession error\n");
			return false;
		}

		// (2) load all the system settings into the session
		status = NvAPI_DRS_LoadSettings(hSession);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_LoadSettings error\n");
			return false;
		}

		// (3) Obtain the Base profile. Any setting needs to be inside
		// a profile, putting a setting on the Base Profile enforces it
		// for all the processes on the system
		hProfile = 0;
		status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_GetBaseProfile error\n");
			return false;
		}


		// Now modify the settings to set NVIDIA global
		// TODO : documentation
		NVDRS_SETTING drsSetting1 = {0};
		drsSetting1.version = NVDRS_SETTING_VER;
		drsSetting1.settingId = SHIM_MCCOMPAT_ID;
		drsSetting1.settingType = NVDRS_DWORD_TYPE;

		NVDRS_SETTING drsSetting2 = {0};
		drsSetting2.version = NVDRS_SETTING_VER;
		drsSetting2.settingId = SHIM_RENDERING_MODE_ID;
		drsSetting2.settingType = NVDRS_DWORD_TYPE;

		NVDRS_SETTING drsSetting3 = {0};
		drsSetting3.version = NVDRS_SETTING_VER;
		drsSetting3.settingId = SHIM_RENDERING_OPTIONS_ID;
		drsSetting3.settingType = NVDRS_DWORD_TYPE;

		// Optimus flags for enabled applications
		if(index == 0)
			drsSetting1.u32CurrentValue = SHIM_MCCOMPAT_ENABLE;			// 0
		else if(index == 1)
			drsSetting1.u32CurrentValue = SHIM_MCCOMPAT_INTEGRATED;		// 1
		else
			drsSetting1.u32CurrentValue = SHIM_MCCOMPAT_AUTO_SELECT;	// 2

		// other options
		//		SHIM_MCCOMPAT_INTEGRATED		// 1
		//		SHIM_MCCOMPAT_USER_EDITABLE
		//		SHIM_MCCOMPAT_VARYING_BIT
		//		SHIM_MCCOMPAT_AUTO_SELECT		// 2
		
		// Enable application for Optimus
		// drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_ENABLE; // 0
		if(index == 0)
			drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_ENABLE;		// 0
		else if(index == 1)
			drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_INTEGRATED;	// 1
		else
			drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_ENABLE;		// 2

		// other options
		//		SHIM_RENDERING_MODE_INTEGRATED		// 1
		//		SHIM_RENDERING_MODE_USER_EDITABLE
		//		SHIM_RENDERING_MODE_VARYING_BIT
		//		SHIM_RENDERING_MODE_AUTO_SELECT		// 2
		//		SHIM_RENDERING_MODE_OVERRIDE_BIT
		//		SHIM_MCCOMPAT_OVERRIDE_BIT
		
		// Shim rendering modes per application for Optimus
		// drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE; // 0
		if(index == 0)
			drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE; // 0
		else if(index == 1)
			drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE | SHIM_RENDERING_OPTIONS_IGPU_TRANSCODING;	// 1
		else
			drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE;		// 2

		// other options
		//		SHIM_RENDERING_OPTIONS_DISABLE_ASYNC_PRESENT,
		//		SHIM_RENDERING_OPTIONS_EHSHELL_DETECT,
		//		SHIM_RENDERING_OPTIONS_FLASHPLAYER_HOST_DETECT,
		//		SHIM_RENDERING_OPTIONS_VIDEO_DRM_APP_DETECT,
		//		SHIM_RENDERING_OPTIONS_IGNORE_OVERRIDES,
		//		SHIM_RENDERING_OPTIONS_CHILDPROCESS_DETECT,
		//		SHIM_RENDERING_OPTIONS_ENABLE_DWM_ASYNC_PRESENT,
		//		SHIM_RENDERING_OPTIONS_PARENTPROCESS_DETECT,
		//		SHIM_RENDERING_OPTIONS_ALLOW_INHERITANCE,
		//		SHIM_RENDERING_OPTIONS_DISABLE_WRAPPERS,
		//		SHIM_RENDERING_OPTIONS_DISABLE_DXGI_WRAPPERS,
		//		SHIM_RENDERING_OPTIONS_PRUNE_UNSUPPORTED_FORMATS,
		//		SHIM_RENDERING_OPTIONS_ENABLE_ALPHA_FORMAT,
		//		SHIM_RENDERING_OPTIONS_IGPU_TRANSCODING,				// 1 ** include for force integrated
		//		SHIM_RENDERING_OPTIONS_DISABLE_CUDA,
		//		SHIM_RENDERING_OPTIONS_ALLOW_CP_CAPS_FOR_VIDEO,
		//		SHIM_RENDERING_OPTIONS_ENABLE_NEW_HOOKING,
		//		SHIM_RENDERING_OPTIONS_DISABLE_DURING_SECURE_BOOT,
		//		SHIM_RENDERING_OPTIONS_INVERT_FOR_QUADRO,
		//		SHIM_RENDERING_OPTIONS_INVERT_FOR_MSHYBRID,
		//		SHIM_RENDERING_OPTIONS_REGISTER_PROCESS_ENABLE_GOLD,


		// Code from "SOP" example
		//	if( ForceIntegrated ){
		//		drsSetting1.u32CurrentValue = SHIM_MCCOMPAT_INTEGRATED;
		//		drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_INTEGRATED;
		//		drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE | SHIM_RENDERING_OPTIONS_IGPU_TRANSCODING;
		//	}else{
		//		drsSetting1.u32CurrentValue = SHIM_MCCOMPAT_ENABLE;
		//		drsSetting2.u32CurrentValue = SHIM_RENDERING_MODE_ENABLE;
		//		drsSetting3.u32CurrentValue = SHIM_RENDERING_OPTIONS_DEFAULT_RENDERING_MODE;
		//	}

		status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting1);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_SetSetting 1 error\n");
			return false;
		}

		status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting2);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_SetSetting 2 error\n");
			return false;
		}

		status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting3);
		if (status != NVAPI_OK) {
			printf("nVidia::ActivateNVIDIA : NvAPI_DRS_SetSetting 3 error\n");
			return false;
		}

		// (5) Now apply (or save) our changes to the system
		status = NvAPI_DRS_SaveSettings(hSession);
		if (status != NVAPI_OK) {
			char temp[256];
			sprintf_s(temp, 256, "nVidia::ActivateNVIDIA : NvAPI_DRS_SaveSettings error (%x) [%p]", (unsigned int)status, hSession);
			printf("%s\n", temp);
			return false;
		}

		// (6) We clean up. This is analogous to doing a free()
		NvAPI_DRS_DestroySession(hSession);
		hSession = 0;
		NvAPI_Unload();

		// printf("NVAPI Settings applied OK\nClose and restart program\n");

		return true;

} // end ActivateGlobal (index)


//
//  0 - high performance
//  1 - integrated
//  2 - auto select
// -1 - fail
//
int nVidia::GetNVIDIA(unsigned int *DriverVersion, char * BuilBranchString)
{
	int mode = 0;
	
	// printf("nVidia::GetNVIDIA()\n");

	// (0) Initialize NVAPI. This must be done first of all
	status = NvAPI_Initialize();
	if (status != NVAPI_OK) {
		// will fail silently if not supported
		printf("nVidia::GetNVIDIA : NvAPI_Initialize error\n");
		return -1;
	}

	//
	// NVAPI_INTERFACE NvAPI_SYS_GetDriverAndBranchVersion(NvU32 *pDriverVersion,
	//													   NvAPI_ShortString szBuildBranchString)
	NvU32 driverVersion;
	NvAPI_ShortString szBuildBranchString;
	NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, szBuildBranchString);
	// printf("Driver version : %d\n", driverVersion); // 37633
	// printf("BuildBranchString = %s\n", szBuildBranchString); // r375_00

	/*
	// TODO - enumeration
	NvDisplayHandle hNvDisp = NULL;
	NvPhysicalGpuHandle hPhysicalGpu[64];
	NvU32 GpuCount = 0;
	NvAPI_GetPhysicalGPUsFromDisplay(hNvDisp, hPhysicalGpu, &GpuCount);
	NvAPI_ShortString szFullName;
	NvAPI_GPU_GetFullName(hPhysicalGpu, szFullName);
	*/

	*DriverVersion = driverVersion;
	strcpy_s(BuilBranchString, 64, szBuildBranchString);


	// (1) Create the session handle to access driver settings
	hSession = 0;
	status = NvAPI_DRS_CreateSession(&hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::GetNVIDIA : NvAPI_DRS_CreateSession error\n");
		return 0;
	}

	// (2) load all the system settings into the session
	status = NvAPI_DRS_LoadSettings(hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::GetNVIDIA : NvAPI_DRS_LoadSettings error\n");
		return 0;
	}

	// (3) Obtain the Base profile. Any setting needs to be inside
	// a profile, putting a setting on the Base Profile enforces it
	// for all the processes on the system
	hProfile = 0;
	status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
	if (status != NVAPI_OK) {
		printf("nVidia::GetNVIDIA : NvAPI_DRS_GetBaseProfile error\n");
		return 0;
	}

	// Now get the settings
	NVDRS_SETTING drsSetting1 = {0};
	drsSetting1.version = NVDRS_SETTING_VER;
	drsSetting1.settingId = SHIM_MCCOMPAT_ID;
	drsSetting1.settingType = NVDRS_DWORD_TYPE;

	NVDRS_SETTING drsSetting2 = {0};
	drsSetting2.version = NVDRS_SETTING_VER;
	drsSetting2.settingId = SHIM_RENDERING_MODE_ID;
	drsSetting2.settingType = NVDRS_DWORD_TYPE;

	NVDRS_SETTING drsSetting3 = {0};
	drsSetting3.version = NVDRS_SETTING_VER;
	drsSetting3.settingId = SHIM_RENDERING_OPTIONS_ID;
	drsSetting3.settingType = NVDRS_DWORD_TYPE;

	//	 1,  1, 0
	//   0,  0, 8192
	//  16,  1, 0
	// Optimus flags for enabled applications
	// SHIM_MCCOMPAT_ENABLE			High performance (1)
	// SHIM_MCCOMPAT_INTEGRATED		Integrated		 (0)
	// SHIM_MCCOMPAT_AUTO_SELECT	Auto select		 (16)

	status = NvAPI_DRS_GetSetting(hSession, hProfile, SHIM_MCCOMPAT_ID, &drsSetting1);
	if(drsSetting1.u32CurrentValue == 1)
		mode = 0; // high performance
	else if(drsSetting1.u32CurrentValue == 0)
		mode = 1; // integrated
	else
		mode = 2; // Auto select

	// Enable application for Optimus
	// status = NvAPI_DRS_GetSetting(hSession, hProfile, SHIM_RENDERING_MODE_ID, &drsSetting2);
	// printf("drsSetting2.u32CurrentValue = %d\n", drsSetting2.u32CurrentValue);

	// Shim rendering modes per application for Optimus
	// status = NvAPI_DRS_GetSetting(hSession, hProfile, SHIM_RENDERING_OPTIONS_ID, &drsSetting3);
	// printf("drsSetting3.u32CurrentValue = %d\n", drsSetting3.u32CurrentValue);

	// (6) We clean up. This is analogous to doing a free()
	NvAPI_DRS_DestroySession(hSession);
	hSession = 0;
	NvAPI_Unload();

	return mode;

} // end GetNVIDIA


//
// Threaded optimization can affect texture sharing
// This might have something to do with the OpenGL/DirectX interop
// or some other reason. The symptom is that fps seems capped at 40.
//
// OGL_THREAD_CONTROL_ID = 0x20C1221E
//    OGL_THREAD_CONTROL_ENABLE = 0x00000001,  ON
//    OGL_THREAD_CONTROL_DISABLE = 0x00000002, OFF
//    OGL_THREAD_CONTROL_NUM_VALUES = 2,
//    OGL_THREAD_CONTROL_DEFAULT = 0           AUTO
//
// Mode
// 0 - auto
// 1 - on
// 2 - off
//
int nVidia::SetThreadedOptimization(int mode)
{
	// (0) Initialize NVAPI. This must be done first of all
	status = NvAPI_Initialize();
	if (status != NVAPI_OK) {
		// will fail silently if not supported
		printf("nVidia::SetThreadedOptimization : NvAPI_Initialize error\n");
		return -1;
	}

	// (1) Create the session handle to access driver settings
	hSession = 0;
	status = NvAPI_DRS_CreateSession(&hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::SetThreadedOptimization : NvAPI_DRS_CreateSession error\n");
		return 0;
	}

	// (2) load all the system settings into the session
	status = NvAPI_DRS_LoadSettings(hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::SetThreadedOptimization : NvAPI_DRS_LoadSettings error\n");
		return 0;
	}

	// (3) Obtain the Base profile. Any setting needs to be inside
	// a profile, putting a setting on the Base Profile enforces it
	// for all the processes on the system
	hProfile = 0;
	status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
	if (status != NVAPI_OK) {
		printf("nVidia::SetThreadedOptimization : NvAPI_DRS_GetBaseProfile error\n");
		return 0;
	}

	//
	// Docs http://developer.download.nvidia.com/NVAPI/PG-5116-001_v01_public.pdf
	//
	//
	// (4) Specify that we want the THREAD_CONTROL setting
	// first we fill the NVDRS_SETTING struct, then we call the function
	NVDRS_SETTING drsSetting = {0};
	drsSetting.version = NVDRS_SETTING_VER;
	drsSetting.settingId = OGL_THREAD_CONTROL_ID;
	drsSetting.settingType = NVDRS_DWORD_TYPE;
	// 0 - auto, 1 - on, 2 - off
	drsSetting.u32CurrentValue = mode;
	status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting);
	if (status != NVAPI_OK) printf("nVidia::SetThreadedOptimization : NvAPI_DRS_SetSetting error\n"); 

    // (5) Now we apply (or save) our changes to the system
	status = NvAPI_DRS_SaveSettings(hSession);
	if (status != NVAPI_OK) printf("nVidia::SetThreadedOptimization : NvAPI_DRS_SaveSettings error\n");
	
	// (6) We clean up. This is analogous to doing a free()
	NvAPI_DRS_DestroySession(hSession);
	hSession = 0;
	NvAPI_Unload();

	return mode;

} // end SetThreadedOptimization


// Threaded optimization mode
// 0 - auto : 1 - on : 2 - off
int nVidia::GetThreadedOptimization()
{

	// (0) Initialize NVAPI. This must be done first of all
	status = NvAPI_Initialize();
	if (status != NVAPI_OK) {
		// will fail silently if not supported
		printf("nVidia::GetThreadedOptimization : NvAPI_Initialize error\n");
		return -1;
	}

	// (1) Create the session handle to access driver settings
	hSession = 0;
	status = NvAPI_DRS_CreateSession(&hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::GetThreadedOptimization : NvAPI_DRS_CreateSession error\n");
		return -1;
	}

	// (2) load all the system settings into the session
	status = NvAPI_DRS_LoadSettings(hSession);
	if (status != NVAPI_OK) {
		printf("nVidia::GetThreadedOptimization : NvAPI_DRS_LoadSettings error\n");
		return -1;
	}

	// (3) Obtain the Base profile. Any setting needs to be inside
	// a profile, putting a setting on the Base Profile enforces it
	// for all the processes on the system
	hProfile = 0;
	status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
	if (status != NVAPI_OK) {
		printf("nVidia::GetThreadedOptimization : NvAPI_DRS_GetBaseProfile error\n");
		return -1;
	}

	//
	// Docs http://developer.download.nvidia.com/NVAPI/PG-5116-001_v01_public.pdf
	//
	//
	// (4) Specify that we want the THREAD_CONTROL setting
	// first we fill the NVDRS_SETTING struct, then we call the function
	NVDRS_SETTING drsSetting = { 0 };
	drsSetting.version = NVDRS_SETTING_VER;
	drsSetting.settingId = OGL_THREAD_CONTROL_ID;
	drsSetting.settingType = NVDRS_DWORD_TYPE;
	// 0 - auto, 1 - on, 2 - off
	// drsSetting.u32CurrentValue = mode;
	// status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting);
	// if (status != NVAPI_OK) printf("nVidia::GetThreadedOptimization : NvAPI_DRS_SetSetting error\n");
	// printf("nVidia::GetThreadedOptimization : NVAPI_DRS_GetSetting\n");
	status = NvAPI_DRS_GetSetting(hSession, hProfile, OGL_THREAD_CONTROL_ID, &drsSetting);
	if (status != NVAPI_OK) {
		printf("nVidia::GetThreadedOptimization : NVAPI_DRS_GetSetting error\n");
		return -1;
	}

	// printf("drsSetting.u32CurrentValue = %i\n", drsSetting.u32CurrentValue);

	// current DWORD value of this setting
	int mode = (int)drsSetting.u32CurrentValue;

	// (5) We clean up. This is analogous to doing a free()
	NvAPI_DRS_DestroySession(hSession);
	hSession = 0;
	NvAPI_Unload();

	// printf("thread mode = %d\n", mode);

	return mode;

} // end GetThreadedOptimization


