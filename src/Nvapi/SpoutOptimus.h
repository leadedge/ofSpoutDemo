#include <windows.h>
#include <sal.h>
#include "nvapi.h"
#include "NvApiDriverSettings.h"
#include "stdio.h"
#include "string"

class nVidia
{
    public:

        nVidia();
        ~nVidia();

		bool IsLaptop(bool *integrated);
		bool GetNvidiaSystem(int *systemType, int *gpuType);
		bool SetNvidiaGPU(int iPreference);
		int  GetNvidiaGPU(unsigned int *DriverVersion, char * BuilBranchString);
		bool SetThreadedOptimization(int mode);
		int  GetThreadedOptimization();

	protected :

		bool InitializeNvidia();
		void ReleaseNvidia();
		bool CreateSession();
		bool LoadBaseProfile();
		bool GetGPUtype(int *systemType, int *gpuType);

		NvDRSSessionHandle hSession;
		NvDRSProfileHandle hProfile;
		NvAPI_Status status;
		bool initialized;

};


