#include "SDK.hpp"

#include "../Utils.hpp"
#include "../XorStr.hpp"
#include "../Constants.h"

IVEngineClient*     Interfaces::m_pEngine = nullptr;
IBaseClientDLL*     Interfaces::m_pClient = nullptr;
IClientEntityList*  Interfaces::m_pEntityList = nullptr;
CGlobalVarsBase*    Interfaces::m_pGlobals = nullptr;
IPanel*             Interfaces::m_pVGuiPanel = nullptr;
ISurface*           Interfaces::m_pVGuiSurface = nullptr;
CInput*             Interfaces::m_pInput = nullptr;
IEngineTrace*       Interfaces::m_pEngineTrace = nullptr;
ICvar*              Interfaces::m_pCVar = nullptr;
IClientMode*        Interfaces::m_pClientMode = nullptr;
IVModelInfo*        Interfaces::m_pModelInfo = nullptr;
IVRenderView*       Interfaces::m_pRenderView = nullptr;
IVModelRender*      Interfaces::m_pModelRender = nullptr;
IVEffects*          Interfaces::m_pEffects = nullptr;
IMaterialSystem*    Interfaces::m_pMaterialSystem = nullptr;
IGameEventManager2* Interfaces::m_pGameEventManager2 = nullptr;
CMoveHelper*        Interfaces::m_pMoveHelper = nullptr;
CGameMovement*      Interfaces::m_pGameMovement = nullptr;
CPrediction*        Interfaces::m_pPrediction = nullptr;
CViewRender*        Interfaces::m_pViewRender = nullptr;
IInputSystem*       Interfaces::m_pInputSystem = nullptr;
IEngineSound*       Interfaces::m_pEngineSound = nullptr;

CreateInterfaceFn GetFactory(HMODULE hMod)
{
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(hMod, XorStr("CreateInterface")));
}

template<typename T>
T* CaptureInterface(CreateInterfaceFn f, const char* szInterfaceVersion)
{
	return reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));
}

IVEngineClient* Interfaces::Engine()
{
	if (!m_pEngine)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pEngine = CaptureInterface<IVEngineClient>(pfnFactory, Constants::Interfaces::VEngineClient);
	}
	return m_pEngine;
}
IBaseClientDLL* Interfaces::Client()
{
	if (!m_pClient)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::ClientDll));
		m_pClient = CaptureInterface<IBaseClientDLL>(pfnFactory, Constants::Interfaces::VClient);
	}
	return m_pClient;
}
IClientEntityList* Interfaces::EntityList()
{
	if (!m_pEntityList)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::ClientDll));
		m_pEntityList = CaptureInterface<IClientEntityList>(pfnFactory, Constants::Interfaces::VClientEntityList);
	}
	return m_pEntityList;
}
CGlobalVarsBase* Interfaces::GlobalVars()
{
	if (!m_pGlobals)
	{
		const auto pClientVFTable = *reinterpret_cast<uint32_t**>(Client());
		m_pGlobals = **reinterpret_cast<CGlobalVarsBase***>(pClientVFTable[0] + 0x1B);
	}
	return m_pGlobals;
}
IPanel* Interfaces::VGUIPanel()
{
	if (!m_pVGuiPanel)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::VGUIDll));
		m_pVGuiPanel = CaptureInterface<IPanel>(pfnFactory, Constants::Interfaces::VGUIPanel);
	}
	return m_pVGuiPanel;
}
ISurface* Interfaces::MatSurface()
{
	if (!m_pVGuiSurface)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::VGUIMatSurfaceDll));
		m_pVGuiSurface = CaptureInterface<ISurface>(pfnFactory, Constants::Interfaces::VGUISurface);
	}
	return m_pVGuiSurface;
}
CInput* Interfaces::Input()
{
	if (!m_pInput)
	{
		m_pInput = *Constants::Signatures::Input.Find<CInput**>();
	}
	return m_pInput;
}
IGameEventManager2* Interfaces::EventManager()
{
	if (!m_pGameEventManager2)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pGameEventManager2 = CaptureInterface<IGameEventManager2>(pfnFactory, Constants::Interfaces::GameEventsManager);
	}
	return m_pGameEventManager2;
}
IEngineTrace* Interfaces::EngineTrace()
{
	if (!m_pEngineTrace)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pEngineTrace = CaptureInterface<IEngineTrace>(pfnFactory, Constants::Interfaces::EngineTraceClient);
	}
	return m_pEngineTrace;
}
ICvar* Interfaces::CVar()
{
	if (!m_pCVar)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::VStdLibDll));
		m_pCVar = CaptureInterface<ICvar>(pfnFactory, Constants::Interfaces::VEngineCvar);
	}
	return m_pCVar;
}
IClientMode* Interfaces::ClientMode()
{
	if (!m_pClientMode)
	{
		const auto pClientVFTable = *reinterpret_cast<uint32_t**>(Client());
		m_pClientMode = **reinterpret_cast<IClientMode***>(pClientVFTable[10] + 0x5);
	}
	return m_pClientMode;
}
IVModelInfo* Interfaces::ModelInfo()
{
	if (!m_pModelInfo)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pModelInfo = CaptureInterface<IVModelInfo>(pfnFactory, Constants::Interfaces::VModelInfoClient);
	}
	return m_pModelInfo;
}
IVRenderView* Interfaces::RenderView()
{
	if (!m_pRenderView)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pRenderView = CaptureInterface<IVRenderView>(pfnFactory, Constants::Interfaces::VEngineRenderView);
	}
	return m_pRenderView;
}
IVModelRender* Interfaces::ModelRender()
{
	if (!m_pModelRender)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pModelRender = CaptureInterface<IVModelRender>(pfnFactory, Constants::Interfaces::VEngineModel);
	}
	return m_pModelRender;
}
IVEffects* Interfaces::Effects()
{
	if (!m_pEffects)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pEffects = CaptureInterface<IVEffects>(pfnFactory, Constants::Interfaces::VEngineEffects);
	}
	return m_pEffects;
}
IMaterialSystem* Interfaces::MaterialSystem()
{
	if (!m_pMaterialSystem)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::MaterialSystemDll));
		m_pMaterialSystem = CaptureInterface<IMaterialSystem>(pfnFactory, Constants::Interfaces::VMaterialSystem);
	}
	return m_pMaterialSystem;
}
IInputSystem* Interfaces::InputSystem()
{
	if (!m_pInputSystem)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::InputSystemDll));
		m_pInputSystem = CaptureInterface<IInputSystem>(pfnFactory, Constants::Interfaces::InputSystemVersion);
	}
	return m_pInputSystem;
}
IEngineSound* Interfaces::EngineSound()
{
	if (!m_pEngineSound)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::EngineDll));
		m_pEngineSound = CaptureInterface<IEngineSound>(pfnFactory, Constants::Interfaces::IEngineSoundClient);
	}
	return m_pEngineSound;
}
CMoveHelper* Interfaces::MoveHelper()
{
	if (!m_pMoveHelper)
	{
		m_pMoveHelper = **Constants::Signatures::MoveHelper.Find<CMoveHelper***>();
	}
	return m_pMoveHelper;
}
CGameMovement* Interfaces::GameMovement()
{
	if (!m_pGameMovement)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::ClientDll));
		m_pGameMovement = CaptureInterface<CGameMovement>(pfnFactory, Constants::Interfaces::GameMovement);
	}
	return m_pGameMovement;
}
CPrediction* Interfaces::Prediction()
{
	if (!m_pPrediction)
	{
		const auto pfnFactory = GetFactory(GetModuleHandleA(Constants::CsgoFiles::ClientDll));
		m_pPrediction = CaptureInterface<CPrediction>(pfnFactory, Constants::Interfaces::VClientPrediction);
	}
	return m_pPrediction;
}