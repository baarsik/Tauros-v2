#pragma once

class AutoPistol
{
public:
	AutoPistol()
	{
		m_bLMBDown = false;
		m_bRMBDown = false;
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!Options::g_bAutoPistolEnabled || Options::g_bMainWindowOpen)
			return;

		auto pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon || !pWeapon->IsPistol() || pWeapon->GetClip() == 0)
			return;

		const auto isRevolver = *pWeapon->ItemDefinitionIndex() == WEAPON_REVOLVER;
		if (isRevolver && !m_bRMBDown || !isRevolver && !m_bLMBDown)
			return;

		const auto curtime = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick;
		if (pWeapon->NextPrimaryAttack() >= curtime || pLocal->GetNextAttack() >= curtime)
			pCmd->buttons &= isRevolver ? ~IN_ATTACK2 : ~IN_ATTACK;
		else
			pCmd->buttons |= isRevolver ? IN_ATTACK2 : IN_ATTACK;
	}

	void CheckToggle(bool vecPressedKeys[256])
	{
		m_bLMBDown = vecPressedKeys[VK_LBUTTON];
		m_bRMBDown = vecPressedKeys[VK_RBUTTON];
	}

private:
	bool m_bLMBDown;
	bool m_bRMBDown;
};