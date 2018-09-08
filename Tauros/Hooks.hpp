#pragma once

#include <Windows.h>
#include <d3d9.h>

#include "SourceEngine/SDK.hpp"

//Link with the D3D9 implementations
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace Hooks
{
	/// <summary>
	/// Set up hooks. This should be called when the dll is loaded.
	/// </summary>
	void Initialize();

	/// <summary>
	/// Register classes and put them into IoC container
	/// </summary>
	void FillContainer();

	/// <summary>
	/// Clean up and remove the hooks. This should be called when the dll is unloaded.
	/// </summary>
	void Restore();

	/// <summary>
	/// Initialize the GUI system and rest of hooks. Called from EndScene.
	/// </summary>
	/// <param name="pDevice"> Pointer to the D3D9 Device </param>
	void GUI_Init(IDirect3DDevice9* pDevice);

	//---------------------------------------------------
	// Hook prototypes
	//---------------------------------------------------
	using EndScene_t              = long(__stdcall* )(IDirect3DDevice9*);
	using Reset_t                 = long(__stdcall* )(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
    using CreateMove_t            = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
	using FrameStageNotify_t      = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
	using OverrideMouseInput_t    = void(__thiscall*)(IClientMode*, float*, float*);
	using OverrideView_t          = void(__thiscall*)(IClientMode*, CViewSetup*);
	using OverrideConfig_t        = bool(__thiscall*)(IMaterialSystem*, MaterialSystem_Config_t*, bool);
	using PaintTraverse_t         = void(__thiscall*)(IPanel*, VPANEL, bool, bool);
	using EmitSound_t             = void(__thiscall*)(IEngineSound*, IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, /*StartSoundParams_t&*/ void*);
	using DrawModelExecute_t      = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	using LockCursor_t            = void(__thiscall*)(ISurface*);
	
	//---------------------------------------------------
	// Hooked functions
	//---------------------------------------------------
	LRESULT   __stdcall Hooked_WndProc(HWND, UINT, WPARAM, LPARAM);
	HRESULT   __stdcall Hooked_EndScene(IDirect3DDevice9*);
	HRESULT   __stdcall Hooked_Reset(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	void      __stdcall Hooked_OverrideMouseInput(float*, float*);
	void      __stdcall Hooked_CreateMove(int, float, bool, bool&);
	void      __stdcall Hooked_CreateMove_Proxy(int, float, bool);
	void      __stdcall Hooked_EmitSound(IRecipientFilter&, int, int, const char*, unsigned int, const char*, float, int, float, int, int, const Vector*, const Vector*, void*, bool, float, int, /*StartSoundParams_t&*/ void*);
	void      __stdcall Hooked_PaintTraverse(VPANEL, bool, bool);
	void      __stdcall Hooked_FrameStageNotify(ClientFrameStage_t);
	void      __stdcall Hooked_OverrideView(CViewSetup*);
	void      __stdcall Hooked_DrawModelExecute(IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	bool      __stdcall Hooked_OverrideConfig(MaterialSystem_Config_t*, bool);
	void	  __stdcall Hooked_LockCursor();
}

