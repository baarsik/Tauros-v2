#pragma once

#include <cassert>

#include "Definitions.hpp"

#include "CRC.hpp"
#include "Vector.hpp"
#include "Vector2D.hpp"
#include "Vector4D.hpp"
#include "QAngle.hpp"
#include "CHandle.hpp"
#include "CGlobalVarsBase.hpp"
#include "ClientClass.hpp"
#include "Color.hpp"
#include "IBaseClientDll.hpp"
#include "IClientEntity.hpp"
#include "IClientEntityList.hpp"
#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientThinkable.hpp"
#include "IClientUnknown.hpp"
#include "IPanel.hpp"
#include "ISurface.hpp"
#include "IMaterialSystem.hpp"
#include "IVEngineClient.hpp"
#include "IVModelInfo.hpp"
#include "IVRenderView.hpp"
#include "IVModelRender.hpp"
#include "IVEffects.hpp"
#include "IEngineTrace.hpp"
#include "PlayerInfo.hpp"
#include "Recv.hpp"
#include "VMatrix.hpp"
#include "IClientMode.hpp"
#include "CInput.hpp"
#include "ICvar.hpp"
#include "IGameEvents.hpp"
#include "Convar.hpp"
#include "CCSWeaponData.hpp"
#include "GameMovement.hpp"

class Interfaces
{
public:
	static IVEngineClient*        Engine();
	static IBaseClientDLL*        Client();
	static IClientEntityList*     EntityList();
	static CGlobalVarsBase*       GlobalVars();
	static IPanel*                VGUIPanel();
	static ISurface*              MatSurface();
	static CInput*                Input();
	static IEngineTrace*          EngineTrace();
	static ICvar*                 CVar();
	static IClientMode*           ClientMode();
	static IVModelInfo*           ModelInfo();
	static IVRenderView*          RenderView();
	static IVModelRender*         ModelRender();
	static IVEffects*             Effects();
	static IMaterialSystem*       MaterialSystem();
	static IGameEventManager2*    EventManager();
	static CMoveHelper*           MoveHelper();
	static CGameMovement*         GameMovement();
	static CPrediction*           Prediction();
	static CViewRender*           ViewRender();
	static IInputSystem*		  InputSystem();

private:
	static IVEngineClient*        m_pEngine;
	static IBaseClientDLL*        m_pClient;
	static IClientEntityList*     m_pEntityList;
	static CGlobalVarsBase*       m_pGlobals;
	static IPanel*                m_pVGuiPanel;
	static ISurface*              m_pVGuiSurface;
	static CInput*                m_pInput;
	static IEngineTrace*          m_pEngineTrace;
	static ICvar*                 m_pCVar;
	static IClientMode*           m_pClientMode;
	static IVModelInfo*           m_pModelInfo;
	static IVRenderView*          m_pRenderView;
	static IVModelRender*         m_pModelRender;
	static IVEffects*             m_pEffects;
	static IMaterialSystem*       m_pMaterialSystem;
	static IGameEventManager2*    m_pGameEventManager2;
	static CMoveHelper*           m_pMoveHelper;
	static CGameMovement*         m_pGameMovement;
	static CPrediction*           m_pPrediction;
	static CViewRender*           m_pViewRender;
	static IInputSystem*		  m_InputSystem;
};