#pragma once

class Bhop
{
public:
	Bhop()
	{
		m_bLastJumped = false;
		m_bShouldFake = false;
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		DoStrafe(pLocal, pCmd);
		DoBhop(pLocal, pCmd);
	}
private:
	bool m_bLastJumped;
	bool m_bShouldFake;

	void DoBhop(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		if (!Options::g_bBHopEnabled || !pLocal->IsAlive())
			return;

		if (!m_bLastJumped && m_bShouldFake)
		{
			m_bShouldFake = false;
			pCmd->buttons |= IN_JUMP;
		}
		else if (pCmd->buttons & IN_JUMP)
		{
			if (pLocal->GetFlags() & static_cast<int>(EntityFlags::FL_ONGROUND))
			{
				m_bLastJumped = true;
				m_bShouldFake = true;
			}
			else
			{
				pCmd->buttons &= ~IN_JUMP;
				m_bLastJumped = false;
			}
		}
		else
		{
			m_bLastJumped = false;
			m_bShouldFake = false;
		}
	}

	void DoStrafe(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!Options::g_bAutoStrafeEnabled)
			return;

		if (pCmd->mousedx <= 1 && pCmd->mousedx >= -1)
			return;

		if (pLocal->GetFlags() & static_cast<int>(EntityFlags::FL_ONGROUND))
			return;
		
		if (pCmd->buttons & IN_JUMP)
			pCmd->sidemove = pCmd->mousedx < 0 ? -450.f : 450.f;
	}
};