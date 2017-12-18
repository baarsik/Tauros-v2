#pragma once

class RCS
{
public:
	bool CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!IsValid(pLocal))
			return true;

		const auto punchAngles = *pLocal->AimPunch() * 2.0f;
		if (punchAngles.x != 0.0f || punchAngles.y != 0)
		{
			pCmd->viewangles -= punchAngles;
			Utils::Clamp(pCmd->viewangles);
			return false;
		}
		return true;
	}
	void OverrideView_Pre(CViewSetup* pViewSetup) const
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!IsValid(pLocal))
			return;

		pViewSetup->angles -= *pLocal->AimPunch() * 2.0f * 0.45f;
		Utils::Clamp(pViewSetup->angles);
	}
	void FrameStageNotify_Pre(ClientFrameStage_t stage)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!IsValid(pLocal))
			return;

		const auto aimPunch = pLocal->AimPunch();
		const auto viewPunch = pLocal->ViewPunch();
		m_vAimPunchOld = Vector(0, 0, 0);
		m_vViewPunchOld = Vector(0, 0, 0);
		if (aimPunch && viewPunch)
		{
			m_vAimPunchOld = *aimPunch;
			m_vViewPunchOld = *viewPunch;
			*aimPunch = Vector(0, 0, 0);
			*viewPunch = Vector(0, 0, 0);
		}
	}
	void FrameStageNotify_Post(ClientFrameStage_t stage) const
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!IsValid(pLocal))
			return;

		const auto aimPunch = pLocal->AimPunch();
		const auto viewPunch = pLocal->ViewPunch();
		if (aimPunch && viewPunch)
		{
			*aimPunch = m_vAimPunchOld;
			*viewPunch = m_vViewPunchOld;
		}
	}
private:
	Vector m_vAimPunchOld;
	Vector m_vViewPunchOld;

	bool IsValid(C_CSPlayer* pLocal) const
	{
		return Options::g_bRCSEnabled && pLocal != nullptr && pLocal->IsAlive();
	}
};