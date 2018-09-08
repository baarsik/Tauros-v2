// Basic stuff
#include "XorStr.hpp"
#include "Constants.h"
#include "Container.hpp"
#include "VFTableHook.hpp"
#include "MaterialHelper.hpp"
#include "SignatureHelper.h"
#include "Hooks.hpp"
#include "Options.hpp"
#include "Utils.hpp"
#include "EnginePrediction.hpp"
#include <intrin.h>

// GUI related
#include "GUI.hpp"
#include "DrawManager.hpp"
#include "ImGUI/imgui.h"
#include "ImGUI/DX9/imgui_impl_dx9.h"

// Hacks
#include "Hacks/AimAssist.hpp"
#include "Hacks/AutoAccept.hpp"
#include "Hacks/AutoPistol.hpp"
#include "Hacks/BackTrack.hpp"
#include "Hacks/Bhop.hpp"
#include "Hacks/C4Timer.hpp"
#include "Hacks/Chams.hpp"
#include "Hacks/DamageInformer.hpp"
#include "Hacks/DLights.hpp"
#include "Hacks/ESP.hpp"
#include "Hacks/RankRevealer.hpp"
#include "Hacks/RCS.hpp"
#include "Hacks/Trigger.hpp"
#include "Hacks/VisualMisc.hpp"

// Helper classes
#include "EventListener.hpp"

using namespace std;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern HMODULE g_hLib;
extern HWND g_hWnd;

namespace Hooks
{
	unique_ptr<VFTableHook>            g_pD3DDevice9Hook = nullptr;
	unique_ptr<VFTableHook>            g_pClientHook = nullptr;
	unique_ptr<VFTableHook>            g_pClientModeHook = nullptr;
	unique_ptr<VFTableHook>            g_pMatSurfaceHook = nullptr;
	unique_ptr<VFTableHook>            g_pVGUIPanelHook = nullptr;
	unique_ptr<VFTableHook>            g_pModelRenderHook = nullptr;
	unique_ptr<VFTableHook>            g_pDrawModelExecuteHook = nullptr;
	unique_ptr<VFTableHook>            g_pEventManagerHook = nullptr;
	unique_ptr<VFTableHook>            g_pMaterialSystemHook = nullptr;
	unique_ptr<VFTableHook>            g_pInputSystemHook = nullptr;
	unique_ptr<VFTableHook>            g_pEngineSoundHook = nullptr;

	EndScene_t                         g_fnOriginalEndScene = nullptr;
	Reset_t                            g_fnOriginalReset = nullptr;
	CreateMove_t                       g_fnOriginalCreateMove = nullptr;
	EmitSound_t                        g_fnOriginalEmitSound = nullptr;
	PaintTraverse_t                    g_fnOriginalPaintTraverse = nullptr;
	FrameStageNotify_t                 g_fnOriginalFrameStageNotify = nullptr;
	OverrideView_t                     g_fnOriginalOverrideView = nullptr;
	DrawModelExecute_t                 g_fnOriginalDrawModelExecute = nullptr;
	OverrideMouseInput_t               g_fnOriginalOverrideMouseInput = nullptr;
	OverrideConfig_t                   g_fnOriginalOverrideConfig = nullptr;
	LockCursor_t                       g_fnOriginalLockCursor = nullptr;

	WNDPROC                            g_pOldWindowProc = nullptr;

	bool                               g_vecPressedKeys[256] = {};
	bool                               g_bWasInitialized = false;

	void Initialize()
	{
		//Find CSGO main window
		while (!g_hWnd)
		{
			g_hWnd = FindWindowA(XorStr("Valve001"), nullptr);
			Sleep(200);
		}

		NetvarManager::Instance()->CreateDatabase();
		Container::Instance().Register<SignatureHelper>();
		//NetvarManager::Instance()->Dump(Utils::GetDllDir() + XorStr("netvar_dump.txt"));

		g_pD3DDevice9Hook = make_unique<VFTableHook>(Container::Instance().Resolve<SignatureHelper>()->D3DDevice());
		g_pClientHook = make_unique<VFTableHook>(Interfaces::Client());
		g_pClientModeHook = make_unique<VFTableHook>(Interfaces::ClientMode());
		g_pMatSurfaceHook = make_unique<VFTableHook>(Interfaces::MatSurface());
		g_pVGUIPanelHook = make_unique<VFTableHook>(Interfaces::VGUIPanel());
		g_pModelRenderHook = make_unique<VFTableHook>(Interfaces::ModelRender());
		g_pEventManagerHook = make_unique<VFTableHook>(Interfaces::EventManager());
		g_pMaterialSystemHook = make_unique<VFTableHook>(Interfaces::MaterialSystem());
		g_pInputSystemHook = make_unique<VFTableHook>(Interfaces::InputSystem());
		g_pEngineSoundHook = make_unique<VFTableHook>(Interfaces::EngineSound());

		g_pOldWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooked_WndProc)));

		g_fnOriginalReset = g_pD3DDevice9Hook->Hook(16, Hooked_Reset);          // IDirect3DDevice9::Reset
		g_fnOriginalEndScene = g_pD3DDevice9Hook->Hook(42, Hooked_EndScene);    // IDirect3DDevice9::EndScene
    }

	void FillContainer()
	{
		auto container = &Container::Instance();
		container->Register<AimAssist>();
		container->Register<AutoAccept>();
		container->Register<AutoPistol>();
		container->Register<BackTrack>();
		container->Register<Bhop>();
		container->Register<C4Timer>();
		container->Register<Chams>();
		container->Register<DamageInformer>();
		container->Register<DLights>();
		container->Register<ESP>();
		container->Register<GrayWalls>();
		container->Register<Hands>();
		container->Register<EventListener>();
		container->Register<NoFlash>();
		container->Register<RankRevealer>();
		container->Register<RCS>();
		container->Register<Trigger>();
	}

	void Restore()
	{
		//Restore the WindowProc
		SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_pOldWindowProc));

		Container::Instance().Resolve<DrawManager>()->InvalidateObjects();

		//Remove the hooks
		g_pD3DDevice9Hook->RestoreTable();
		g_pClientHook->RestoreTable();
		g_pClientModeHook->RestoreTable();
		g_pMatSurfaceHook->RestoreTable();
		g_pVGUIPanelHook->RestoreTable();
		g_pModelRenderHook->RestoreTable();
		g_pEventManagerHook->RestoreTable();
		g_pMaterialSystemHook->RestoreTable();
    }

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		//Initializes the GUI and the renderer
		Container::Instance().Register<DrawManager>(pDevice);
		ImGui_ImplDX9_Init(g_hWnd, pDevice);
		Container::Instance().Register<GUI>(pDevice);
		Container::Instance().Resolve<DrawManager>()->CreateObjects(Container::Instance().Resolve<GUI>()->GetScale());
		FillContainer();

		g_fnOriginalCreateMove = g_pClientHook->Hook(22, reinterpret_cast<CreateMove_t>(Hooked_CreateMove_Proxy));                          // IBaseClientDLL::CreateMove CHL
		g_fnOriginalFrameStageNotify = g_pClientHook->Hook(37, reinterpret_cast<FrameStageNotify_t>(Hooked_FrameStageNotify));              // IBaseClientDLL::FrameStageNotify
		g_fnOriginalOverrideView = g_pClientModeHook->Hook(18, reinterpret_cast<OverrideView_t>(Hooked_OverrideView));                      // IClientMode::OverrideView
		g_fnOriginalOverrideMouseInput = g_pClientModeHook->Hook(23, reinterpret_cast<OverrideMouseInput_t>(Hooked_OverrideMouseInput));    // IClientMode::OverrideMouseInput
		g_fnOriginalOverrideConfig = g_pMaterialSystemHook->Hook(21, reinterpret_cast<OverrideConfig_t>(Hooked_OverrideConfig));            // IMaterialSystem::OverrideConfig
		g_fnOriginalPaintTraverse = g_pVGUIPanelHook->Hook(41, reinterpret_cast<PaintTraverse_t>(Hooked_PaintTraverse));                    // IPanel::PaintTraverse
		g_fnOriginalEmitSound = g_pEngineSoundHook->Hook(5, reinterpret_cast<EmitSound_t>(Hooked_EmitSound));                              // IEngineSound::EmitSound
		g_fnOriginalDrawModelExecute = g_pModelRenderHook->Hook(21, reinterpret_cast<DrawModelExecute_t>(Hooked_DrawModelExecute));         // IVModelRender::DrawModelExecute
		g_fnOriginalLockCursor = g_pMatSurfaceHook->Hook(67, reinterpret_cast<LockCursor_t>(Hooked_LockCursor));							// ISurface::LockCursor

		g_bWasInitialized = true;
	}

	HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		if (!g_bWasInitialized)
		{
			GUI_Init(pDevice);
			return g_fnOriginalEndScene(pDevice);
		}

		static auto wanted_ret_address = _ReturnAddress();
		if (_ReturnAddress() != wanted_ret_address) //Panorama fix credits to NuII from UC
			return g_fnOriginalEndScene(pDevice);

		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		//fix drawing without calling engine functons/cl_showpos
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		const auto gui = Container::Instance().Resolve<GUI>();
		gui->UpdateCursorVisibility();
		ImGui_ImplDX9_NewFrame();
		gui->Show();

		auto drawManager = Container::Instance().Resolve<DrawManager>();
		drawManager->BeginRendering();

		Container::Instance().Resolve<ESP>()->EndScene_Pre(pDevice);

		ImGui::Render();
		drawManager->EndRendering();

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);

		return g_fnOriginalEndScene(pDevice);
	}

	HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (!g_bWasInitialized)
			return g_fnOriginalReset(pDevice, pPresentationParameters);

		auto drawManager = Container::Instance().Resolve<DrawManager>();

		// Device is on LOST state.
		ImGui_ImplDX9_InvalidateDeviceObjects();
		drawManager->InvalidateObjects();

		// Call original Reset.
		const auto originalReset = g_fnOriginalReset(pDevice, pPresentationParameters);

		ImGui::CreateContext();
		const auto gui = Container::Instance().Resolve<GUI>();
		gui->UpdateSize(pDevice);
		drawManager->CreateObjects(gui->GetScale());
		ImGui_ImplDX9_CreateDeviceObjects();
		return originalReset;
	}

	bool __stdcall Hooked_OverrideConfig(MaterialSystem_Config_t* config, bool forceUpdate)
	{
		Container::Instance().Resolve<GrayWalls>()->OverrideConfig(config, forceUpdate);

		return g_fnOriginalOverrideConfig(Interfaces::MaterialSystem(), config, forceUpdate);
	}

	void __stdcall Hooked_PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce)
	{
		g_fnOriginalPaintTraverse(Interfaces::VGUIPanel(), vguiPanel, forceRepaint, allowForce);
		if (Options::g_bCleanScreenshot && Interfaces::Engine()->IsTakingScreenshot())
			return;

		static unsigned int overlayPanel = 0;
		if (overlayPanel == 0)
		{
			if (!strstr(Interfaces::VGUIPanel()->GetName(vguiPanel), XorStr("MatSystemTopPanel")))
				return;

			overlayPanel = vguiPanel;
		}

		if (overlayPanel != vguiPanel)
			return;

		if (!Interfaces::Engine()->IsInGame())
			return;

		Container::Instance().Resolve<DamageInformer>()->PaintTraverse_Post();
		Container::Instance().Resolve<C4Timer>()->PaintTraverse_Post();
	}

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!g_bWasInitialized)
			return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);

		switch(uMsg)
		{
			case WM_LBUTTONDOWN:
				g_vecPressedKeys[VK_LBUTTON] = true;
				break;
			case WM_LBUTTONUP:
				g_vecPressedKeys[VK_LBUTTON] = false;
				break;
			case WM_RBUTTONDOWN:
				g_vecPressedKeys[VK_RBUTTON] = true;
				break;
			case WM_RBUTTONUP:
				g_vecPressedKeys[VK_RBUTTON] = false;
				break;
			case WM_KEYDOWN:
				g_vecPressedKeys[wParam] = true;
				break;
			case WM_KEYUP:
				g_vecPressedKeys[wParam] = false;
				break;
			default: break;
		}

		Container::Instance().Resolve<GUI>()->CheckToggle(g_vecPressedKeys);
		Container::Instance().Resolve<AutoPistol>()->CheckToggle(g_vecPressedKeys);

        if(g_bWasInitialized && Options::g_bMainWindowOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;

		return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	void __stdcall Hooked_CreateMove(int sequenceNumber, float inputSampleFrametime, bool active, bool& bSendPacket)
	{
		g_fnOriginalCreateMove(Interfaces::Client(), sequenceNumber, inputSampleFrametime, active);
		const auto pLocal = C_CSPlayer::GetLocalPlayer();

		const auto pCmd = Interfaces::Input()->GetUserCmd(sequenceNumber);
		auto verified = Interfaces::Input()->GetVerifiedUserCmd(sequenceNumber);

		if (!pCmd || !verified)
			return;

		Container::Instance().Resolve<Bhop>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<RankRevealer>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<DLights>()->CreateMove_Post(pLocal, pCmd);

		static auto predictionSystem = std::make_unique<PredictionSystem>();
		predictionSystem->StartPrediction(pLocal, pCmd);
		Container::Instance().Resolve<RCS>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<Trigger>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<AimAssist>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<AutoPistol>()->CreateMove_Post(pLocal, pCmd);
		Container::Instance().Resolve<BackTrack>()->CreateMove_Post(pLocal, pCmd);
		predictionSystem->EndPrediction(pLocal);

		verified->m_cmd = *pCmd;
		verified->m_crc = pCmd->GetChecksum();
	}

	__declspec(naked) void __stdcall Hooked_CreateMove_Proxy(int sequenceNumber, float inputSampleFrametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[inputSampleFrametime]
			push dword ptr[sequenceNumber]
			call Hooks::Hooked_CreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t stage)
	{
		Container::Instance().Resolve<NoFlash>()->FrameStageNotify_Pre(stage);
		Container::Instance().Resolve<RCS>()->FrameStageNotify_Pre(stage);

		g_fnOriginalFrameStageNotify(Interfaces::Client(), stage);

		Container::Instance().Resolve<RCS>()->FrameStageNotify_Post(stage);
		Container::Instance().Resolve<BackTrack>()->FrameStageNotify_Post(stage);
	}

	void __stdcall Hooked_OverrideView(CViewSetup* pViewSetup)
	{
		Container::Instance().Resolve<RCS>()->OverrideView_Pre(pViewSetup);

		g_fnOriginalOverrideView(Interfaces::ClientMode(), pViewSetup);
	}

	void __stdcall Hooked_DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
	{
		g_pModelRenderHook->Unhook(21); // Prevent infinite recursive loop
		MatHelper.Initialize();

		if (!Options::g_bCleanScreenshot || !Interfaces::Engine()->IsTakingScreenshot())
		{
			Container::Instance().Resolve<Chams>()->DrawModelExecute_Pre(ctx, state, pInfo, pCustomBoneToWorld);
			Container::Instance().Resolve<Hands>()->DrawModelExecute_Pre(ctx, state, pInfo, pCustomBoneToWorld);
		}

		g_fnOriginalDrawModelExecute(Interfaces::ModelRender(), ctx, state, pInfo, pCustomBoneToWorld);
		Interfaces::ModelRender()->ForcedMaterialOverride(nullptr);
		g_pModelRenderHook->Hook(21, reinterpret_cast<DrawModelExecute_t>(Hooked_DrawModelExecute));
	}

	void __stdcall Hooked_OverrideMouseInput(float* x, float* y)
	{
		g_fnOriginalOverrideMouseInput(Interfaces::ClientMode(), x, y);

		Container::Instance().Resolve<AimAssist>()->OverrideMouseInput_Post(x, y);
	}

	void __stdcall Hooked_EmitSound(IRecipientFilter& filter, int entIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, float attenuation, int flags, int pitch, const Vector* origin, const Vector* direction, void* utlVecOrigins, bool updatePositions, float soundTime, int speakerEntity, /*StartSoundParams_t&*/ void* params)
	{
		Container::Instance().Resolve<AutoAccept>()->EmitSound_Pre(filter, entIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, attenuation, flags, pitch, origin, direction, utlVecOrigins, updatePositions, soundTime, speakerEntity, params);

		g_fnOriginalEmitSound(Interfaces::EngineSound(), filter, entIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, attenuation, flags, pitch, origin, direction, utlVecOrigins, updatePositions, soundTime, speakerEntity, params);
	}

	void __stdcall Hooked_LockCursor()
	{
		if (Options::g_bMainWindowOpen)
		{
			Interfaces::MatSurface()->UnlockCursor();
			return;
		}
		g_fnOriginalLockCursor(Interfaces::MatSurface());
	}
}