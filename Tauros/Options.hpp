#pragma once

namespace Options
{
	extern const char* Keys[17];
	extern const int KeysID[17];

    extern bool g_bMainWindowOpen;
	extern bool g_bDeathmatch;

	// Aim (1) - AimAssist
	extern bool g_bAimAssistEnabled;
	extern const char* g_szAimAssistLockTarget[4];
	extern int g_iAimAssistLockTarget;
	extern bool g_bAimAssistIgnoreObstacles;
	extern bool g_bAimAssistAutoShoot;
	extern bool g_bAimAssistLockMouse;
	extern bool g_bAimAssistIgnoreSmoke;
	extern bool g_bAimAssistSniperScopedOnly;
	extern bool g_bAimAssistDistanceBasedFov;
	extern float g_fAimAssistFov;
	extern float g_fAimAssistSmooth;
	extern float g_fAimAssistFovPistol;
	extern float g_fAimAssistSmoothPistol;
	extern float g_fAimAssistVerticalSmoothMultiplier;
	// Aim (2) - Trigger
	extern bool g_bTriggerEnabled;
	extern int g_iTriggerKey;
	extern bool g_bTriggerAlwaysActive;
	extern bool g_bTriggerIgnoreSmoke;
	extern bool g_bTriggerAimSynergy;
	extern bool g_bTriggerSniperScopedOnly;
	// Aim (3) - Misc
	extern bool g_bRCSEnabled;
	extern bool g_bBackTrackingEnabled;

	// Visuals (1) - ESP
    extern bool g_bESPEnabled;
    extern int g_iESPShowBoxes;
	extern int g_iESPShowHealth;
	extern int g_iESPShowBones;
    extern int g_iESPShowNames;
	extern int g_iESPShowDistance;
	extern int g_iESPShowWeapon;
	extern float g_fESPAllyColor[3];
	extern float g_fESPEnemyColor[3];
	// Visuals (2) - Chams
	extern bool g_bChamsEnabled;
	extern bool g_bChamsShowInvisible;
	extern int g_iChamsVisibleMaterialId;
	extern float g_fChamsVisibleColor[3];
	extern int g_iChamsInvisibleMaterialId;
	extern float g_fChamsInvisibleColor[3];
	// Visuals (2) - DLight
	extern bool g_bDLightEnabled;
	extern bool g_bELightEnabled;
	extern bool g_bDLightColorByTeam;
	extern float g_fDLightEnemyTColor[3];
	extern float g_fDLightAllyCTColor[3];
	// Visuals (3) - Misc
	extern bool g_bCleanScreenshot;
	extern bool g_bC4TimerEnabled;
	extern bool g_bDamageInformerEnabled;
	extern bool g_bGrayWallsEnabled;
	extern bool g_bNoSmokeEnabled;
	extern bool g_bNoFlashEnabled;
	extern float g_fNoFlashAmount;
	extern const char* g_szHandsDisplay[3];
	extern int g_iHandsDisplay;
	extern bool g_bRankRevealerEnabled;

	// Skin
	extern bool g_bSkinChangerEnabled;

	// Automation
	extern bool g_bAutoPistolEnabled;
	extern bool g_bBHopEnabled;
	extern bool g_bAutoStrafeEnabled;
	extern bool g_bAutoAcceptEnabled;
}

