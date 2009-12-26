#include "Fuji.h"

#if MF_INPUT == MF_DRIVER_IPHONE

#include "MFInput_Internal.h"

/*** Structure definitions ***/

/*** Globals ***/

/**** Platform Specific Functions ****/

void MFInput_InitModulePlatformSpecific()
{
}

void MFInput_DeinitModulePlatformSpecific()
{
}

void MFInput_UpdatePlatformSpecific()
{
}

uint32 MFInput_GetDeviceFlags(int device, int deviceID)
{
	return 0;
}

MFInputDeviceStatus MFInput_GetDeviceStatusInternal(int device, int id)
{
	return IDS_Unavailable;
}

void MFInput_GetGamepadStateInternal(int id, MFGamepadState *pGamepadState)
{
	MFZeroMemory(pGamepadState, sizeof(MFGamepadState));
}

void MFInput_GetKeyStateInternal(int id, MFKeyState *pKeyState)
{
	MFZeroMemory(pKeyState, sizeof(MFKeyState));
}

void MFInput_GetMouseStateInternal(int id, MFMouseState *pMouseState)
{
	MFZeroMemory(pMouseState, sizeof(MFMouseState));
}

const char* MFInput_GetDeviceNameInternal(int source, int sourceID)
{
	return "iPhone";
}

const char* MFInput_GetGamepadButtonNameInternal(int button, int sourceID)
{
	return "iPhone";
}

bool MFInput_GetKeyboardStatusState(int keyboardState, int keyboardID)
{
	return false;
}

#endif