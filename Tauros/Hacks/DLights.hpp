#pragma once

class DLights
{
public:
	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!Options::g_bDLightEnabled && !Options::g_bELightEnabled)
			return;

		PlayerLights(pLocal);
	}

private:
	void PlayerLights(C_CSPlayer* pLocal) const
	{
		if (!pLocal)
			return;

		for (auto i = 1; i < Interfaces::Engine()->GetMaxClients(); i++)
		{
			if (i == Interfaces::Engine()->GetLocalPlayer())
				continue;

			auto pTarget = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			if (!pTarget)
				continue;

			if (pTarget->GetClientClass()->m_ClassID == EClassIds::CCSPlayer)
			{
				if (!pTarget->IsAlive() || pTarget->IsDormant() || pTarget == pLocal || pLocal->GetObservingTarget() == pTarget)
					continue;

				if (Options::g_bELightEnabled)
					SpawnELight(i, pTarget, pLocal);

				if (Options::g_bDLightEnabled)
					SpawnDLight(i, pTarget, pLocal);
			}
		}
	}

	// Spawns ELight above pTarget head with small shift towards pLocal
	void SpawnELight(int index, C_CSPlayer* pTarget, C_CSPlayer* pLocal) const
	{
		const auto vLocal = pLocal->GetEyePos();
		const auto vTarget = Utils::GetEntityBone(pTarget, ECSPlayerBones::HEAD_0);
		auto elight = Interfaces::Effects()->CL_AllocElight(index);
		elight->origin = vTarget + Vector(vTarget.x > vLocal.x ? -10.f : 10.f, vTarget.y > vLocal.y ? -10.f : 10.f, 35.f);
		elight->radius = 75.f;
		if (Options::g_bDLightColorByTeam && pTarget->GetTeamNum() == 3 || !Options::g_bDLightColorByTeam && pTarget->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch) // CT / Ally
		{
			elight->color.r = static_cast<int>(Options::g_fDLightAllyCTColor[0] * 255.f);
			elight->color.g = static_cast<int>(Options::g_fDLightAllyCTColor[1] * 255.f);
			elight->color.b = static_cast<int>(Options::g_fDLightAllyCTColor[2] * 255.f);
		}
		else
		{
			elight->color.r = static_cast<int>(Options::g_fDLightEnemyTColor[0] * 255.f);
			elight->color.g = static_cast<int>(Options::g_fDLightEnemyTColor[1] * 255.f);
			elight->color.b = static_cast<int>(Options::g_fDLightEnemyTColor[2] * 255.f);
		}
		elight->color.exponent = 10;
		elight->decay = elight->radius / 5.f;
		elight->key = index;
		elight->m_Direction = vTarget;
		elight->die = Interfaces::GlobalVars()->curtime + 0.05f;
	}

	// Spawns DLight below pTarget
	void SpawnDLight(int index, C_CSPlayer* pTarget, C_CSPlayer* pLocal) const
	{
		const auto vOrigin = pTarget->GetOrigin();
		auto dlight = Interfaces::Effects()->CL_AllocDlight(index);
		dlight->origin = vOrigin;
		dlight->radius = 25.f;
		dlight->flags = DLIGHT_NO_MODEL_ILLUMINATION;
		if (Options::g_bDLightColorByTeam && pTarget->GetTeamNum() == 3 || !Options::g_bDLightColorByTeam && pTarget->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch) // CT / Ally
		{
			dlight->color.r = static_cast<int>(Options::g_fDLightAllyCTColor[0] * 255.f);
			dlight->color.g = static_cast<int>(Options::g_fDLightAllyCTColor[1] * 255.f);
			dlight->color.b = static_cast<int>(Options::g_fDLightAllyCTColor[2] * 255.f);
		}
		else
		{
			dlight->color.r = static_cast<int>(Options::g_fDLightEnemyTColor[0] * 255.f);
			dlight->color.g = static_cast<int>(Options::g_fDLightEnemyTColor[1] * 255.f);
			dlight->color.b = static_cast<int>(Options::g_fDLightEnemyTColor[2] * 255.f);
		}
		dlight->color.exponent = 10;
		dlight->decay = dlight->radius / 5.f;
		dlight->key = index;
		dlight->m_Direction = vOrigin;
		dlight->die = Interfaces::GlobalVars()->curtime + 0.05f;
	}
};