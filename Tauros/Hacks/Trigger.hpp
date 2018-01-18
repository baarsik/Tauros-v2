#pragma once

class Trigger
{
public:
	Trigger()
	{
		m_pSignatureHelper = Container::Instance().Resolve<SignatureHelper>();
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		if (!IsEnabled(pLocal))
			return;

		if (!IsTargetInSight(pLocal, pCmd))
			return;

		auto pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon || pWeapon->GetClip() == 0)
			return;

		if (pWeapon->IsPistol())
			AutoPistol(pLocal, pWeapon, pCmd);
		else
			pCmd->buttons |= IN_ATTACK;
	}
private:
	std::shared_ptr<SignatureHelper> m_pSignatureHelper;

	void AutoPistol(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon, CUserCmd* pCmd) const
	{
		// Let AutoPistol class handle this situation
		if (Options::g_bAutoPistolEnabled && (GetKeyState(VK_LBUTTON) & 0x100) != 0)
			return;

		if (pWeapon->NextPrimaryAttack() > pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick)
			return;
		
		if (*pWeapon->ItemDefinitionIndex() == WEAPON_REVOLVER)
			pCmd->buttons |= IN_ATTACK2;
		else
			pCmd->buttons |= IN_ATTACK;
	}

	bool IsEnabled(C_CSPlayer* pLocal) const
	{
		if (!(Options::g_bTriggerEnabled || Options::g_bAimAssistAutoShoot) || pLocal == nullptr || !pLocal->IsAlive())
			return false;

		if (!GetAsyncKeyState(Options::KeysID[Options::g_iTriggerKey]) && !Options::g_bTriggerAlwaysActive && !Options::g_bAimAssistAutoShoot)
			return false;

		auto pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon)
			return false;

		const auto shouldBeScoped = Options::g_bTriggerSniperScopedOnly && pWeapon->IsSniper();
		if (shouldBeScoped && !pLocal->IsScoped())
			return false;

		return !(pWeapon->IsKnife() || pWeapon->IsGrenade() || pWeapon->IsC4());
	}

	bool IsTargetInSight(C_CSPlayer* pLocal, CUserCmd* pCmd) const
	{
		auto vTraceForward = Vector(0, 0, 0);
		auto vTraceAngles = pCmd->viewangles;
		if (!Options::g_bRCSEnabled)
		{
			const auto punchAngles = *pLocal->AimPunch() * 2.0f;
			if (punchAngles.x != 0.0f || punchAngles.y != 0)
			{
				vTraceAngles -= punchAngles;
				Utils::Clamp(vTraceAngles);
			}
		}
		Utils::AngleVectors(vTraceAngles, &vTraceForward);
		const auto vTraceStart = pLocal->GetEyePos();
		const auto vTraceEnd = vTraceStart + vTraceForward * 8192.0f;

		trace_t trace;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = pLocal;
		ray.Init(vTraceStart, vTraceEnd);
		Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);
		if (trace.fraction == 1.f)
			return false;

		auto target = static_cast<C_CSPlayer*>(trace.m_pEnt);
		if (!target || !target->IsAlive() || target->GetHealth() < 1 || target->IsImmune())
			return false;

		if (!Options::g_bTriggerIgnoreSmoke && m_pSignatureHelper->LineThroughSmoke(vTraceStart, trace.endpos))
			return false;

		if (target && target->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
			return false;

		return true;
	}
};