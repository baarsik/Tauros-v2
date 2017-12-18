#pragma once

class AimAssist
{
public:

	explicit AimAssist()
	{
		m_bIsAttacking = false;
		m_pAimLockedTarget = nullptr;
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		m_bIsAttacking = pCmd->buttons & IN_ATTACK;
	}

	void OverrideMouseInput_Post(float* x, float* y)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!IsEnabled(pLocal))
		{
			m_pAimLockedTarget = nullptr;
			return;
		}

		auto pWeapon = pLocal->GetActiveWeapon();
		if (!pWeapon || pWeapon->IsKnife() || pWeapon->IsGrenade() || pWeapon->IsC4())
		{
			m_pAimLockedTarget = nullptr;
			return;
		}

		if (!m_bIsAttacking && !Options::g_bAimAssistAutoShoot && !IsTriggerEnabled(pLocal, pWeapon))
		{
			m_pAimLockedTarget = nullptr;
			return;
		}

		static auto emptyClip = false;
		const auto nextAttackTime = pWeapon->NextPrimaryAttack() - Interfaces::GlobalVars()->curtime;
		if (nextAttackTime >= 1.f || pWeapon->GetClip() == 0) emptyClip = true;
		else if (nextAttackTime <= 0.1f) emptyClip = false;
		const auto shouldBeScoped = Options::g_bAimAssistSniperScopedOnly && pWeapon->IsSniper();
		if (emptyClip || shouldBeScoped && !pLocal->IsScoped())
		{
			m_pAimLockedTarget = nullptr;
			return;
		}

		if (m_pAimLockedTarget != nullptr && !m_pAimLockedTarget->IsAlive())
			m_pAimLockedTarget = nullptr;

		auto bone = HEAD_0;
		Vector qDelta;
		const auto fov = pWeapon->IsPistol() ? Options::g_fAimAssistFovPistol : Options::g_fAimAssistFov;
		while (true)
		{
			const auto pTarget = m_pAimLockedTarget == nullptr
				? GetClosestPlayer(pLocal, fov, bone)
				: Options::g_iAimAssistLockTarget == 1
					? GetClosestPlayer(pLocal, fov * 1.5f, bone)
					: Options::g_iAimAssistLockTarget == 2
						? GetClosestPlayer(pLocal, fov * 2.f, bone)
						: m_pAimLockedTarget;

			if (pTarget && TraceBone(pLocal, pTarget, bone))
			{
				qDelta = GetDelta(pLocal, pTarget, bone);
				if (Options::g_iAimAssistLockTarget > 0)
					m_pAimLockedTarget = pTarget;

				break;
			}

			if (bone == HEAD_0) bone = NECK_0;
			else if (bone == NECK_0) bone = SPINE_3;
			else if (bone == SPINE_3) bone = SPINE_2;
			else if (bone == SPINE_2) bone = SPINE_1;
			else if (bone == SPINE_1) bone = SPINE_0;
			else
			{
				m_pAimLockedTarget = nullptr;
				return;
			}
		}

		// Adjust cursor position (0.022f = no smoothing)
		const auto xSmooth = pWeapon->IsPistol() ? Options::g_fAimAssistSmoothPistol : Options::g_fAimAssistSmooth;
		const auto ySmooth = xSmooth / Options::g_fAimAssistVerticalSmoothMultiplier;
		qDelta.x /= xSmooth >= 0.022f ? xSmooth : 0.022f;
		qDelta.y /= ySmooth >= 0.022f ? ySmooth : 0.022f;

		auto vMouse = Options::g_bAimAssistLockMouse ? Vector(0, 0, 0) : Vector(*static_cast<float*>(x), *static_cast<float*>(y), 0.0f);
		auto vDelta = Vector(qDelta.y, -qDelta.x, 0.0f);
		vDelta.x = (vDelta.x + vMouse.x) / 2.0f;
		vDelta.y = (vDelta.y + vMouse.y) / 2.0f;
		*static_cast<float*>(x) = vDelta.x;
		*static_cast<float*>(y) = vDelta.y;
	}
private:
	bool m_bIsAttacking;
	C_CSPlayer* m_pAimLockedTarget;

	static bool IsEnabled(C_CSPlayer* pLocal)
	{
		return pLocal && Options::g_bAimAssistEnabled && pLocal->IsAlive();
	}

	static bool IsTriggerEnabled(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon)
	{
		const auto shouldBeScoped = Options::g_bTriggerSniperScopedOnly && pWeapon->IsSniper();
		if (shouldBeScoped && !pLocal->IsScoped())
			return false;

		return Options::g_bTriggerEnabled && Options::g_bTriggerAimSynergy && (GetAsyncKeyState(Options::KeysID[Options::g_iTriggerKey]) || Options::g_bTriggerAlwaysActive);
	}

	static bool TraceBone(C_CSPlayer* pLocal, C_CSPlayer* pTarget, ECSPlayerBones bone)
	{
		if (Options::g_bAimAssistIgnoreObstacles)
			return true;

		trace_t trace;
		Ray_t ray;
		CTraceFilter filter;
		auto vTraceForward = Vector(0, 0, 0);
		const auto vEyePos = pLocal->GetEyePos();
		const auto vAimBone = Utils::GetEntityBone(pTarget, bone);
		const auto qAimAngles = Utils::CalcAngle(vEyePos, vAimBone);
		Utils::AngleVectors(qAimAngles, &vTraceForward);
		ray.Init(vEyePos, vEyePos + vTraceForward * 8192.0f);
		filter.pSkip = pLocal;
		Interfaces::EngineTrace()->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);
		if (trace.fraction == 1.f || !trace.m_pEnt)
			return false;

		const auto vTraceEndPos = trace.endpos;
		if (!Options::g_bAimAssistIgnoreSmoke && Container::Instance().Resolve<SignatureHelper>()->LineThroughSmoke(vEyePos, vTraceEndPos))
			return false;

		auto target = static_cast<C_CSPlayer*>(trace.m_pEnt);
		if (!target || !target->IsAlive() || target->GetHealth() < 1 || target->IsImmune() || target->IsDormant())
			return false;

		if (target->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
			return false;

		return true;
	}

	static C_CSPlayer* GetClosestPlayer(C_CSPlayer* pLocal, float fov, ECSPlayerBones bone)
	{
		C_CSPlayer* pTarget = nullptr;
		auto maxFov = fov;

		QAngle aimAngles;
		Interfaces::Engine()->GetViewAngles(aimAngles);
		if (!Options::g_bRCSEnabled)
			aimAngles += *pLocal->AimPunch() * 2.f;

		const auto vEyePos = pLocal->GetEyePos();

		for (auto i = 1; i <= Interfaces::Engine()->GetMaxClients(); i++)
		{
			auto pPotentialTarget = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			if (!pPotentialTarget || pPotentialTarget == pLocal)
				continue;

			if (!pPotentialTarget->IsAlive() || pPotentialTarget->IsDormant() || pPotentialTarget->IsImmune())
				continue;

			if (pPotentialTarget->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
				continue;

			const auto vTargetBone = Utils::GetEntityBone(pPotentialTarget, bone);
			const auto fTargetFov = Options::g_bAimAssistDistanceBasedFov
				? GetFovFraction(aimAngles, Utils::CalcAngle(vEyePos, vTargetBone))
				: GetFov(aimAngles, Utils::CalcAngle(vEyePos, vTargetBone));
			if (fTargetFov > maxFov)
				continue;

			pTarget = pPotentialTarget;
			maxFov = fTargetFov;
		}
		return pTarget;
	}

	static Vector GetDelta(C_CSPlayer* pLocal, C_CSPlayer* pTarget, ECSPlayerBones bone)
	{
		QAngle qViewAngles;
		Interfaces::Engine()->GetViewAngles(qViewAngles);
		if (!Options::g_bRCSEnabled)
			qViewAngles += *pLocal->AimPunch() * 2.0f;
		Utils::Clamp(qViewAngles);
		const auto vEyePos = pLocal->GetEyePos();
		const auto vAimBone = Utils::GetEntityBone(pTarget, bone);
		const auto qAimAngles = Utils::CalcAngle(vEyePos, vAimBone);
		auto qDelta = qViewAngles - qAimAngles;
		Utils::Clamp(qDelta);
		return qDelta;
	}

	static float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		auto delta = aimAngle - viewAngle;
		Utils::Clamp(delta);
		return sqrtf(powf(delta.x, 2.f) + powf(delta.y, 2.f));
	}

	static float GetFovFraction(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		auto delta = aimAngle - viewAngle;
		Utils::Clamp(delta);
		return sin(DEG2RAD(delta.Length()) / 2.f) * 180.f;
	}
};