#pragma once
#include "XorStr.hpp"
#include "Signature.hpp"
#include "Constants.h"

const char* Constants::CsgoFiles::ClientDll         = "client_panorama.dll";
const char* Constants::CsgoFiles::DX9Dll            = "shaderapidx9.dll";
const char* Constants::CsgoFiles::EngineDll         = "engine.dll";
const char* Constants::CsgoFiles::InputSystemDll    = "inputsystem.dll";
const char* Constants::CsgoFiles::MaterialSystemDll = "materialsystem.dll";
const char* Constants::CsgoFiles::VGUIDll           = "vgui2.dll";
const char* Constants::CsgoFiles::VGUIMatSurfaceDll = "vguimatsurface.dll";
const char* Constants::CsgoFiles::VStdLibDll        = "vstdlib.dll";

const Signature Constants::Signatures::CurrentCommand(CsgoFiles::ClientDll, "89 BE ? ? ? ? E8 ? ? ? ? 85 FF", 0x2);
const Signature Constants::Signatures::D3DDevice(CsgoFiles::DX9Dll, "A1 ? ? ? ? 6A 00 53", 0x1);
const Signature Constants::Signatures::Input(CsgoFiles::ClientDll, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10", 0x1);
const Signature Constants::Signatures::LineThroughSmokeFn(CsgoFiles::ClientDll, "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
const Signature Constants::Signatures::MoveHelper(CsgoFiles::ClientDll, "8B 0D ? ? ? ? 8B 46 08 68", 0x2);
const Signature Constants::Signatures::PredictionRandomSeed(CsgoFiles::ClientDll, "A3 ? ? ? ? 66 0F 6E 86", 0x1);
const Signature Constants::Signatures::ServerRankRevealAllFn(CsgoFiles::ClientDll, "55 8B EC 8B 0D ? ? ? ? 85 C9 75 ? A1 ? ? ? ? 68 ? ? ? ? 8B 08 8B 01 FF 50 ? 85 C0 74 ? 8B C8 E8 ? ? ? ? 8B C8 EB ? 33 C9 89 0D ? ? ? ? 8B 45 ? FF 70 ? E8 ? ? ? ? B0 ? 5D");
const Signature Constants::Signatures::SetClanTagFn(CsgoFiles::EngineDll, "53 56 57 8B DA 8B F9 FF 15");
const Signature Constants::Signatures::SetLocalPlayerReadyFn(CsgoFiles::ClientDll, "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12");
const Signature Constants::Signatures::ViewMatrix(CsgoFiles::ClientDll, "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9", 0x3);

const char* Constants::Interfaces::EngineTraceClient = "EngineTraceClient004";
const char* Constants::Interfaces::GameEventsManager = "GAMEEVENTSMANAGER002";
const char* Constants::Interfaces::GameMovement = "GameMovement001";
const char* Constants::Interfaces::IEngineSoundClient = "IEngineSoundClient003";
const char* Constants::Interfaces::InputSystemVersion = "InputSystemVersion001";
const char* Constants::Interfaces::VClient = "VClient018";
const char* Constants::Interfaces::VClientEntityList = "VClientEntityList003";
const char* Constants::Interfaces::VClientPrediction = "VClientPrediction001";
const char* Constants::Interfaces::VEngineClient = "VEngineClient014";
const char* Constants::Interfaces::VEngineCvar = "VEngineCvar007";
const char* Constants::Interfaces::VEngineEffects = "VEngineEffects001";
const char* Constants::Interfaces::VEngineModel = "VEngineModel016";
const char* Constants::Interfaces::VEngineRenderView = "VEngineRenderView014";
const char* Constants::Interfaces::VGUIPanel = "VGUI_Panel009";
const char* Constants::Interfaces::VGUISurface = "VGUI_Surface031";
const char* Constants::Interfaces::VMaterialSystem = "VMaterialSystem080";
const char* Constants::Interfaces::VModelInfoClient = "VModelInfoClient004";
