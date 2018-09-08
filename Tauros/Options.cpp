#include "Options.hpp"

namespace Options
{
	const char* Keys[17] = {
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
		"F", "H", "J", "Z", "X", "C", "V"
	};
	const int KeysID[17] = {
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
		0x46, 0x48, 0x4A, 0x5A, 0x58, 0x43, 0x56
	};

    bool g_bMainWindowOpen = false;
	bool g_bDeathmatch = false;

	// Aim (1) - AimAssist
	const char* g_szAimAssistType[3] = {
		"Disabled",
		"Accelerate",
		"Decelerate"
	};
	int g_iAimAssistType = 2;
	float g_fAimAssistType2AcceleratePercentage = 5.f;
	float g_fAimAssistType2HorizontalPenalty = 25.f;
	float g_fAimAssistType2VerticalPenalty = 15.f;
	float g_fAimAssistType2DirectionBoost = 25.f;
	float g_fAimAssistType2FovBoost = 400.f;
	bool g_bAimAssistType2SniperAlwaysActive = false;
	const char* g_szAimAssistLockTarget[5] = {
		"Disabled",
		"+25% FOV",
		"+50% FOV",
		"+100% FOV",
		"Full lock"
	};
	int g_iAimAssistLockTarget = 1;
	bool g_bAimAssistIgnoreObstacles = false;
	bool g_bAimAssistAutoShoot = false;
	bool g_bAimAssistLockMouse = false;
	bool g_bAimAssistIgnoreSmoke = false;
	bool g_bAimAssistSniperScopedOnly = false;
	bool g_bAimAssistDistanceBasedFov = true;
	float g_fAimAssistFov = 2.f;
	float g_fAimAssistSmooth = 0.35f;
	float g_fAimAssistFovPistol = 1.2f;
	float g_fAimAssistSmoothPistol = 0.25f;
	float g_fAimAssistVerticalSmoothMultiplier = 1.75f;
	float g_fAimAssistRCSFailureChance = 10.0f;
	float g_fAimAssistRCSFailureAmount = 15.0f;
	// Aim (2) - Trigger
	bool g_bTriggerEnabled = true;
	int g_iTriggerKey = 10;
	bool g_bTriggerAlwaysActive = false;
	bool g_bTriggerIgnoreSmoke = false;
	bool g_bTriggerAimSynergy = true;
	bool g_bTriggerSniperScopedOnly = true;
	// Aim (3) - Misc
	bool g_bRCSEnabled = false;
	bool g_bBackTrackingEnabled = true;

	// Visuals (1) - ESP
    bool g_bESPEnabled = true;
    int g_iESPShowBoxes = 0;
	int g_iESPShowHealth = 1;
	int g_iESPShowBones = 0;
    int g_iESPShowNames = 1;
	int g_iESPShowDistance = 0;
	int g_iESPShowWeapon = 0;
	float g_fESPAllyColor[3] = { 37.f / 255, 159.f / 255, 1.f };
	float g_fESPEnemyColor[3] = { 1.f, 99.f / 255, 36.f / 255 };
	// Visuals (2) - Chams
	bool g_bChamsEnabled = true;
	bool g_bChamsShowInvisible = false;
	int g_iChamsVisibleMaterialId = 0;
	float g_fChamsVisibleColor[3] = { 152.f / 255, 79.f / 255, 0.f };
	int g_iChamsInvisibleMaterialId = 0;
	float g_fChamsInvisibleColor[3] = { 94.f / 255, 192.f / 255, 1.f };
	// Visuals (2) - DLight
	bool g_bDLightEnabled = true;
	bool g_bELightEnabled = true;
	bool g_bDLightColorByTeam = false;
	float g_fDLightEnemyTColor[3] = { 1.f, 99.f / 255, 36.f / 255 };
	float g_fDLightAllyCTColor[3] = { 37.f / 255, 159.f / 255, 1.f };
	// Visuals (3) - Misc
	bool g_bCleanScreenshot = true;
	bool g_bC4TimerEnabled = true;
	bool g_bDamageInformerEnabled = true;
	bool g_bGrayWallsEnabled = false;
	bool g_bNoFlashEnabled = false;
	float g_fNoFlashAmount = 30.0f;
	const char* g_szHandsDisplay[3] = {
		"Normal",
		"No hands",
		"Wireframe"
	};
	int g_iHandsDisplay = 0;
	bool g_bRankRevealerEnabled = true;

	// Automation
	bool g_bAutoPistolEnabled = true;
    bool g_bBHopEnabled = false;
	bool g_bAutoStrafeEnabled = false;
	bool g_bAutoAcceptEnabled = true;
}
