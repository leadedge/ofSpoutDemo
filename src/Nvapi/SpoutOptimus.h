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
		bool ActivateNVIDIA(int iPreference);
		int GetNVIDIA(unsigned int *DriverVersion, char * BuilBranchString);
		int SetThreadedOptimization(int mode);
		int GetThreadedOptimization();

	protected :

		NvDRSSessionHandle hSession;
		NvDRSProfileHandle hProfile;
		NvAPI_Status status;

};


