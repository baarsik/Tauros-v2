// Basic stuff
#include "XorStr.hpp"
#include "Container.hpp"
#include "VFTableHook.hpp"
#include "MaterialHelper.hpp"
#include "SignatureHelper.hpp"
#include "Hooks.hpp"
#include "Options.hpp"
#include "Utils.hpp"
#include "EnginePrediction.hpp"
#include "BinaryReader.hpp"

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
#include "Hacks/SkinChanger.hpp"
#include "Hacks/Trigger.hpp"
#include "Hacks/VisualMisc.hpp"

// Helper classes
#include "EventListener.hpp"

using namespace std;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
	unique_ptr<VFTableHook>            g_pViewRenderHook = nullptr;

	EndScene_t                         g_fnOriginalEndScene = nullptr;
	Reset_t                            g_fnOriginalReset = nullptr;
	CreateMove_t                       g_fnOriginalCreateMove = nullptr;
	PlaySound_t                        g_fnOriginalPlaySound = nullptr;
	PaintTraverse_t                    g_fnOriginalPaintTraverse = nullptr;
	FrameStageNotify_t                 g_fnOriginalFrameStageNotify = nullptr;
	DispatchUserMessage_t              g_fnOriginalDispatchUserMessage = nullptr;
	OverrideView_t                     g_fnOriginalOverrideView = nullptr;
	DrawModelExecute_t                 g_fnOriginalDrawModelExecute = nullptr;
	OverrideMouseInput_t               g_fnOriginalOverrideMouseInput = nullptr;
	OverrideConfig_t                   g_fnOriginalOverrideConfig = nullptr;
	RenderSmokeOverlay_t               g_fnOriginalRenderSmokeOverlay = nullptr;
	RenderView_t                       g_fnOriginalRenderView = nullptr;
	DoPostScreenEffects_t              g_fnOriginalDoPostScreenEffects = nullptr;

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
		Container::Instance().Register(std::make_shared<SignatureHelper>());
		//NetvarManager::Instance()->Dump(Utils::GetDllDir() + XorStr("netvar_dump.txt"));

		g_pD3DDevice9Hook = make_unique<VFTableHook>(Container::Instance().Resolve<SignatureHelper>()->D3DDevice());
		g_pClientHook = make_unique<VFTableHook>(Interfaces::Client());
		g_pClientModeHook = make_unique<VFTableHook>(Interfaces::ClientMode());
		g_pMatSurfaceHook = make_unique<VFTableHook>(Interfaces::MatSurface());
		g_pVGUIPanelHook = make_unique<VFTableHook>(Interfaces::VGUIPanel());
		g_pModelRenderHook = make_unique<VFTableHook>(Interfaces::ModelRender());
		g_pEventManagerHook = make_unique<VFTableHook>(Interfaces::EventManager());
		g_pMaterialSystemHook = make_unique<VFTableHook>(Interfaces::MaterialSystem());
		g_pViewRenderHook = make_unique<VFTableHook>(Interfaces::ViewRender());

		g_pOldWindowProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Hooked_WndProc)));

		g_fnOriginalReset = g_pD3DDevice9Hook->Hook(16, Hooked_Reset);          // IDirect3DDevice9::Reset
		g_fnOriginalEndScene = g_pD3DDevice9Hook->Hook(42, Hooked_EndScene);    // IDirect3DDevice9::EndScene
    }

	void FillContainer()
	{
		auto container = &Container::Instance();
		container->Register(std::make_shared<AimAssist>());
		container->Register(std::make_shared<AutoAccept>());
		container->Register(std::make_shared<AutoPistol>());
		container->Register(std::make_shared<BackTrack>());
		container->Register(std::make_shared<Bhop>());
		container->Register(std::make_shared<C4Timer>());
		container->Register(std::make_shared<Chams>());
		container->Register(std::make_shared<DamageInformer>());
		container->Register(std::make_shared<DLights>());
		container->Register(std::make_shared<ESP>());
		container->Register(std::make_shared<GrayWalls>());
		container->Register(std::make_shared<Hands>());
		container->Register(std::make_shared<EventListener>());
		container->Register(std::make_shared<NoFlash>());
		container->Register(std::make_shared<NoSmoke>());
		container->Register(std::make_shared<RankRevealer>());
		container->Register(std::make_shared<RCS>());
		container->Register(std::make_shared<SkinChanger>());
		container->Register(std::make_shared<Trigger>());
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
    }

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		//Initializes the GUI and the renderer
		ImGui_ImplDX9_Init(g_hWnd, pDevice);
		Container::Instance().Register(std::make_shared<GUI>(pDevice));
		Container::Instance().Register(std::make_shared<DrawManager>(pDevice));
		Container::Instance().Resolve<DrawManager>()->CreateObjects(Container::Instance().Resolve<GUI>()->GetScale());
		FillContainer();

		g_fnOriginalCreateMove = g_pClientHook->Hook(21, reinterpret_cast<CreateMove_t>(Hooked_CreateMove_Proxy));                          // IBaseClientDLL::CreateMove
		g_fnOriginalFrameStageNotify = g_pClientHook->Hook(36, reinterpret_cast<FrameStageNotify_t>(Hooked_FrameStageNotify));              // IBaseClientDLL::FrameStageNotify
		g_fnOriginalDispatchUserMessage = g_pClientHook->Hook(37, reinterpret_cast<DispatchUserMessage_t>(Hooked_DispatchUserMessage));     // IBaseClientDLL::DispatchUserMessage
		g_fnOriginalOverrideView = g_pClientModeHook->Hook(18, reinterpret_cast<OverrideView_t>(Hooked_OverrideView));                      // IClientMode::OverrideView
		g_fnOriginalOverrideMouseInput = g_pClientModeHook->Hook(23, reinterpret_cast<OverrideMouseInput_t>(Hooked_OverrideMouseInput));    // IClientMode::OverrideMouseInput
		g_fnOriginalDoPostScreenEffects = g_pClientModeHook->Hook(44, reinterpret_cast<DoPostScreenEffects_t>(Hooked_DoPostScreenEffects)); // IClientMode::DoPostScreenEffects
		g_fnOriginalOverrideConfig = g_pMaterialSystemHook->Hook(21, reinterpret_cast<OverrideConfig_t>(Hooked_OverrideConfig));            // IMaterialSystem::OverrideConfig
		g_fnOriginalPaintTraverse = g_pVGUIPanelHook->Hook(41, reinterpret_cast<PaintTraverse_t>(Hooked_PaintTraverse));                    // IPanel::PaintTraverse
		g_fnOriginalPlaySound = g_pMatSurfaceHook->Hook(82, reinterpret_cast<PlaySound_t>(Hooked_PlaySound));                               // ISurface::PlaySound
		g_fnOriginalDrawModelExecute = g_pModelRenderHook->Hook(21, reinterpret_cast<DrawModelExecute_t>(Hooked_DrawModelExecute));         // IVModelRender::DrawModelExecute
		g_fnOriginalRenderView = g_pViewRenderHook->Hook(6, reinterpret_cast<RenderView_t>(Hooked_RenderView));                             // CViewRender::RenderView
		g_fnOriginalRenderSmokeOverlay = g_pViewRenderHook->Hook(40, reinterpret_cast<RenderSmokeOverlay_t>(Hooked_RenderSmokeOverlay));    // CViewRender::RenderSmokeOverlay

		g_bWasInitialized = true;
	}

	HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		if (!g_bWasInitialized)
		{
			GUI_Init(pDevice);
			return g_fnOriginalEndScene(pDevice);
		}
		
		const auto gui = Container::Instance().Resolve<GUI>();
		gui->UpdateCursorVisibility();
		ImGui_ImplDX9_NewFrame();
		gui->Show();

		auto drawManager = Container::Instance().Resolve<DrawManager>();
		drawManager->BeginRendering();

		Container::Instance().Resolve<ESP>()->EndScene_Pre(pDevice);

		ImGui::Render();
		drawManager->EndRendering();

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

        if(g_bWasInitialized && Options::g_bMainWindowOpen && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;

		return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	void __stdcall Hooked_CreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		g_fnOriginalCreateMove(Interfaces::Client(), sequence_number, input_sample_frametime, active);
		const auto pLocal = C_CSPlayer::GetLocalPlayer();

		const auto pCmd = Interfaces::Input()->GetUserCmd(sequence_number);
		auto verified = Interfaces::Input()->GetVerifiedUserCmd(sequence_number);

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

	__declspec(naked) void __stdcall Hooked_CreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::Hooked_CreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t stage)
	{
		Container::Instance().Resolve<NoSmoke>()->FrameStageNotify_Pre(stage);
		Container::Instance().Resolve<NoFlash>()->FrameStageNotify_Pre(stage);
		Container::Instance().Resolve<RCS>()->FrameStageNotify_Pre(stage);
		Container::Instance().Resolve<SkinChanger>()->FrameStageNotify_Pre(stage);

		g_fnOriginalFrameStageNotify(Interfaces::Client(), stage);

		Container::Instance().Resolve<RCS>()->FrameStageNotify_Post(stage);
		Container::Instance().Resolve<BackTrack>()->FrameStageNotify_Post(stage);
	}
	
	void __stdcall Hooked_DispatchUserMessage(ECstrike15UserMessages type, unsigned int a3, unsigned int length, const void* msgData)
	{
		g_fnOriginalDispatchUserMessage(Interfaces::Client(), type, a3, length, msgData);
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

	void __stdcall Hooked_PlaySound(const char* szFileName)
	{
		g_fnOriginalPlaySound(Interfaces::MatSurface(), szFileName);

		Container::Instance().Resolve<AutoAccept>()->PlaySound_Post(szFileName);
	}

	void __stdcall Hooked_RenderSmokeOverlay(bool unk)
	{
		Container::Instance().Resolve<NoSmoke>()->RenderSmokeOverlay_Pre(unk);
	}

	void __stdcall Hooked_RenderView(CViewSetup &view, CViewSetup &hud, int nClearFlags, int whatToDraw)
	{
		g_fnOriginalRenderView(Interfaces::ViewRender(), view, hud, nClearFlags, whatToDraw);
	}

	int __stdcall Hooked_DoPostScreenEffects(int unk)
	{
		return g_fnOriginalDoPostScreenEffects(Interfaces::ClientMode(), unk);
	}
}