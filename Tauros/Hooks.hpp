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
	using DispatchUserMessage_t   = bool(__thiscall*)(IBaseClientDLL*, ECstrike15UserMessages, unsigned int, unsigned int, const void*);
	using DoPostScreenEffects_t   =  int(__thiscall*)(IClientMode*, int);
	using OverrideMouseInput_t    = void(__thiscall*)(IClientMode*, float*, float*);
	using OverrideView_t          = void(__thiscall*)(IClientMode*, CViewSetup*);
	using OverrideConfig_t        = bool(__thiscall*)(IMaterialSystem*, MaterialSystem_Config_t*, bool);
	using PaintTraverse_t         = void(__thiscall*)(IPanel*, VPANEL, bool, bool);
	using PlaySound_t             = void(__thiscall*)(ISurface*, const char*);
	using DrawModelExecute_t      = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	using RenderView_t            = void(__thiscall*)(CViewRender*, CViewSetup&, CViewSetup&, int, int);
	using RenderSmokeOverlay_t    = void(__thiscall*)(CViewRender*, bool);
	
	//---------------------------------------------------
	// Hooked functions
	//---------------------------------------------------
	LRESULT   __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT   __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice);
	HRESULT   __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void      __stdcall Hooked_OverrideMouseInput(float* x, float* y);
	void      __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
	void      __stdcall Hooked_CreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active);
	void      __stdcall Hooked_PlaySound(const char* szFileName);
	void      __stdcall Hooked_PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce);
	void      __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
	void      __stdcall Hooked_DispatchUserMessage(ECstrike15UserMessages type, unsigned int a3, unsigned int length, const void* msgData);
	void      __stdcall Hooked_OverrideView(CViewSetup* pViewSetup);
	void      __stdcall Hooked_DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	bool      __stdcall Hooked_OverrideConfig(MaterialSystem_Config_t* config, bool forceUpdate);
	void      __stdcall Hooked_RenderSmokeOverlay(bool unk);
	int       __stdcall Hooked_DoPostScreenEffects(int unk);
	void      __stdcall Hooked_RenderView(CViewSetup &view, CViewSetup &hud, int nClearFlags, int whatToDraw);

	extern EndScene_t                g_fnOriginalEndScene;
	extern Reset_t                   g_fnOriginalReset;
	extern CreateMove_t              g_fnOriginalCreateMove;
	extern PlaySound_t               g_fnOriginalPlaySound;
	extern PaintTraverse_t           g_fnOriginalPaintTraverse;
	extern FrameStageNotify_t        g_fnOriginalFrameStageNotify;
	extern OverrideView_t            g_fnOriginalOverrideView;
	extern DrawModelExecute_t        g_fnOriginalDrawModelExecute;
	extern OverrideMouseInput_t      g_fnOriginalOverrideMouseInput;
	extern OverrideConfig_t          g_fnOriginalOverrideConfig;
	extern RenderSmokeOverlay_t      g_fnOriginalRenderSmokeOverlay;
	extern DoPostScreenEffects_t     g_fnOriginalDoPostScreenEffects;
}

