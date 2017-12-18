#pragma once

class RankRevealer
{
public:
	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!Options::g_bRankRevealerEnabled || !(pCmd->buttons & IN_SCORE))
			return;

		Container::Instance().Resolve<SignatureHelper>()->ServerRankRevealAll();
	}
};