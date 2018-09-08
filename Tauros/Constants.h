#pragma once
#include "Signature.hpp"

struct Constants
{
	struct CsgoFiles
	{
		static const char* ClientDll;
		static const char* DX9Dll;
		static const char* EngineDll;
		static const char* InputSystemDll;
		static const char* MaterialSystemDll;
		static const char* VGUIDll;
		static const char* VGUIMatSurfaceDll;
		static const char* VStdLibDll;
	};

	struct Signatures
	{
		static const Signature CurrentCommand;
		static const Signature D3DDevice;
		static const Signature Input;
		static const Signature LineThroughSmokeFn;
		static const Signature MoveHelper;
		static const Signature PredictionRandomSeed;
		static const Signature ServerRankRevealAllFn;
		static const Signature SetClanTagFn;
		static const Signature SetLocalPlayerReadyFn;
		static const Signature ViewMatrix;
	};

	struct Interfaces
	{
		static const char* EngineTraceClient;
		static const char* GameEventsManager;
		static const char* GameMovement;
		static const char* IEngineSoundClient;
		static const char* InputSystemVersion;
		static const char* VClient;
		static const char* VClientEntityList;
		static const char* VClientPrediction;
		static const char* VEngineClient;
		static const char* VEngineCvar;
		static const char* VEngineEffects;
		static const char* VEngineModel;
		static const char* VEngineRenderView;
		static const char* VGUIPanel;
		static const char* VGUISurface;
		static const char* VMaterialSystem;
		static const char* VModelInfoClient;
	};
};
