#pragma once
#include "Options.hpp"

struct EconomyItemCfg {
	int nFallbackPaintKit = 0;
	int nFallbackSeed = 0;
	int nFallbackStatTrak = -1;
	int iEntityQuality = 4;
	char* szCustomName = nullptr;
	float flFallbackWear = 0.00001f;
};

class SkinChanger
{
public:
	void FrameStageNotify_Pre(ClientFrameStage_t stage)
	{
		if (!Options::g_bSkinChangerEnabled)
			return;

		InitSkins();
		if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;
		
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!pLocal || !pLocal->IsAlive())
			return;

		const auto pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon)
			return;

		ApplySkin(pWeapon);
	}

private:
	std::unordered_map<int, EconomyItemCfg> m_skinChangerCfg;

	bool ApplySkin(C_BaseCombatWeapon* pWeapon) {
		if (!pWeapon)
			return false;

		const auto nWeaponIndex = *pWeapon->ItemDefinitionIndex();
		if (m_skinChangerCfg.find(nWeaponIndex) == m_skinChangerCfg.end())
			return false;

		// Apply our changes to the fallback variables.
		*pWeapon->FallbackPaintKit() = m_skinChangerCfg[nWeaponIndex].nFallbackPaintKit;
		*pWeapon->EntityQuality() = m_skinChangerCfg[nWeaponIndex].iEntityQuality;
		*pWeapon->FallbackSeed() = m_skinChangerCfg[nWeaponIndex].nFallbackSeed;
		*pWeapon->FallbackStatTrak() = m_skinChangerCfg[nWeaponIndex].nFallbackStatTrak;
		*pWeapon->FallbackWear() = m_skinChangerCfg[nWeaponIndex].flFallbackWear;

		if (m_skinChangerCfg[nWeaponIndex].szCustomName)
			sprintf_s(pWeapon->CustomName(), 32, "%s", m_skinChangerCfg[nWeaponIndex].szCustomName);

		// Edit "m_iItemIDHigh" so fallback values will be used.
		*pWeapon->ItemIDHigh() = -1;
		return true;
	}

	void InitSkins()
	{
		m_skinChangerCfg[WEAPON_AWP].nFallbackPaintKit = 344;           // Dragon Lore
		m_skinChangerCfg[WEAPON_AK47].nFallbackPaintKit = 302;          // Vulkan
		m_skinChangerCfg[WEAPON_M4A1].nFallbackPaintKit = 309;          // Howl
		m_skinChangerCfg[WEAPON_M4A1_SILENCER].nFallbackPaintKit = 587; // Mecha Industries (M4A1-S)
		m_skinChangerCfg[WEAPON_SSG08].nFallbackPaintKit = 222;         // Blood in the Water
		m_skinChangerCfg[WEAPON_P90].nFallbackPaintKit = 359;           // Asiimov (P90)
		m_skinChangerCfg[WEAPON_GLOCK].nFallbackPaintKit = 38;          // Fade (Glock-18)
		m_skinChangerCfg[WEAPON_USP_SILENCER].nFallbackPaintKit = 313;  // Orion
		m_skinChangerCfg[WEAPON_P250].nFallbackPaintKit = 551;          // Asiimov (P250)
		m_skinChangerCfg[WEAPON_SCAR20].nFallbackPaintKit = 312;        // Cyrex
		m_skinChangerCfg[WEAPON_MAG7].nFallbackPaintKit = 39;           // Bulldozer
		m_skinChangerCfg[WEAPON_MP9].nFallbackPaintKit = 39;            // Bulldozer
		m_skinChangerCfg[WEAPON_G3SG1].nFallbackPaintKit = 493;         // Bulldozer
		m_skinChangerCfg[WEAPON_MP7].nFallbackPaintKit = 102;           // Whiteout
		m_skinChangerCfg[WEAPON_AUG].nFallbackPaintKit = 455;           // Akihabara Accept
		m_skinChangerCfg[WEAPON_FAMAS].nFallbackPaintKit = 626;         // Mecha Industries (FAMAS)
		m_skinChangerCfg[WEAPON_NOVA].nFallbackPaintKit = 62;           // Bloomstick
		m_skinChangerCfg[WEAPON_DEAGLE].nFallbackPaintKit = 351;        // Conspiracy
		m_skinChangerCfg[WEAPON_REVOLVER].nFallbackPaintKit = 522;      // Fade (Revolver)
	}
};