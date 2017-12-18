#include "Hooks.hpp"

HMODULE g_hLib;
HWND g_hWnd;
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH) 
	{
		g_hLib = hModule;
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Hooks::Initialize), nullptr, NULL, nullptr);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		if(!lpReserved)
			Hooks::Restore();
	}
	return TRUE;
}