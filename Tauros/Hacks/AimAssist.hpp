#pragma once
#include "SkinChanger.hpp"

class AimAssist
{
public:
	AimAssist()
	{
		m_pSignatureHelper = Container::Instance().Resolve<SignatureHelper>();
		m_bIsAttacking = false;
		m_pAimLockedTarget = nullptr;
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		m_bIsAttacking = pCmd->buttons & IN_ATTACK;
	}

	void OverrideMouseInput_Post(float* x, float* y)
	{
		if (Options::g_iAimAssistType == 0)
			return;

		auto pLocal = C_CSPlayer::GetLocalPlayer();
		auto pWeapon = pLocal->GetActiveWeapon();
		if (!IsValidData(pLocal, pWeapon))
		{
			m_pAimLockedTarget = nullptr;
			return;
		}

		if (m_pAimLockedTarget != nullptr && !m_pAimLockedTarget->IsAlive())
			m_pAimLockedTarget = nullptr;

		Vector qDelta;
		if (!UpdateTarget(pLocal, pWeapon, qDelta))
			return;

		// Adjust cursor position (0.022f = no smoothing)
		const auto xSmooth = pWeapon->IsPistol() ? Options::g_fAimAssistSmoothPistol : Options::g_fAimAssistSmooth;
		const auto ySmooth = xSmooth / Options::g_fAimAssistVerticalSmoothMultiplier;
		qDelta.x /= xSmooth >= 0.022f ? xSmooth : 0.022f;
		qDelta.y /= ySmooth >= 0.022f ? ySmooth : 0.022f;

		auto vMouse = Options::g_bAimAssistLockMouse && Options::g_iAimAssistType != 2
			? Vector(0, 0, 0)
			: Vector(*static_cast<float*>(x), *static_cast<float*>(y), 0.0f);

		auto vDelta = Vector(qDelta.y, -qDelta.x, 0.0f);

		// Slow down if going wrong, accelerate if going right
		// 'Hacky' acceleration should be greatly reduced
		if (Options::g_iAimAssistType == 2)
		{
			const auto boost = 1 + Options::g_fAimAssistType2DirectionBoost / 100.f;

			if (vMouse.x > 0 && vDelta.x < 0 || vMouse.x < 0 && vDelta.x > 0)
				vMouse.x *= 1 - Options::g_fAimAssistType2HorizontalPenalty / 100.f;
			else
				vMouse.x *= boost;

			if (vMouse.y > 0 && vDelta.y < 0 || vMouse.y < 0 && vDelta.y > 0)
				vMouse.y *= 1 - Options::g_fAimAssistType2VerticalPenalty / 100.f;
			else
				vMouse.x *= boost;

			vDelta *= Options::g_fAimAssistType2AcceleratePercentage / 100.f;
		}

		vDelta.x = (vDelta.x + vMouse.x) / 2.0f;
		vDelta.y = (vDelta.y + vMouse.y) / 2.0f;
		*static_cast<float*>(x) = vDelta.x;
		*static_cast<float*>(y) = vDelta.y;
	}
private:
	std::shared_ptr<SignatureHelper> m_pSignatureHelper;
	bool m_bIsAttacking;
	C_CSPlayer* m_pAimLockedTarget;

	bool IsValidData(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon) const
	{
		if (!pLocal || !pLocal->IsAlive())
			return false;

		if (!pWeapon || pWeapon->IsKnife() || pWeapon->IsGrenade() || pWeapon->IsC4())
			return false;

		if (!m_bIsAttacking && !IsTriggerEnabled(pLocal, pWeapon))
		{
			if (Options::g_iAimAssistType != 2 && !Options::g_bAimAssistAutoShoot)
				return false;

			if (Options::g_iAimAssistType == 2 && (!Options::g_bAimAssistType2SniperAlwaysActive || !pLocal->IsScoped() || !pWeapon->IsSniper()))
				return false;
		}

		static auto emptyClip = false;
		const auto nextAttackTime = pWeapon->NextPrimaryAttack() - Interfaces::GlobalVars()->curtime;
		if (nextAttackTime >= 1.f || pWeapon->GetClip() == 0) emptyClip = true;
		else if (nextAttackTime <= 0.1f) emptyClip = false;

		const auto shouldBeScoped = Options::g_bAimAssistSniperScopedOnly && pWeapon->IsSniper();
		if (emptyClip || shouldBeScoped && !pLocal->IsScoped())
			return false;

		return true;
	}

	bool UpdateTarget(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon, Vector& qDelta)
	{
		auto bone = HEAD_0;
		auto fov = pWeapon->IsPistol()
			? Options::g_fAimAssistFovPistol
			: Options::g_fAimAssistFov;

		// Decelerate aim requires much higher FOV to be legit
		if (Options::g_iAimAssistType == 2)
			fov *= 1 + Options::g_fAimAssistType2FovBoost / 100.f;

		while (true)
		{
			const auto pTarget = m_pAimLockedTarget == nullptr
				? GetClosestPlayer(pLocal, fov, bone)
				: Options::g_iAimAssistLockTarget == 1
				? GetClosestPlayer(pLocal, fov * 1.25f, bone)
				: Options::g_iAimAssistLockTarget == 2
				? GetClosestPlayer(pLocal, fov * 1.5f, bone)
				: Options::g_iAimAssistLockTarget == 3
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
				return false;
			}
		}
		return true;
	}

	// Returns true if trigger is enabled and all requirements are met
	bool IsTriggerEnabled(C_CSPlayer* pLocal, C_BaseCombatWeapon* pWeapon) const
	{
		const auto shouldBeScoped = Options::g_bTriggerSniperScopedOnly && pWeapon->IsSniper();
		if (shouldBeScoped && !pLocal->IsScoped())
			return false;

		return Options::g_bTriggerEnabled && Options::g_bTriggerAimSynergy && (GetAsyncKeyState(Options::KeysID[Options::g_iTriggerKey]) || Options::g_bTriggerAlwaysActive);
	}

	bool TraceBone(C_CSPlayer* pLocal, C_CSPlayer* pTarget, const ECSPlayerBones bone) const
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
		if (!Options::g_bAimAssistIgnoreSmoke && m_pSignatureHelper->LineThroughSmoke(vEyePos, vTraceEndPos))
			return false;

		auto target = static_cast<C_CSPlayer*>(trace.m_pEnt);
		if (!target || !target->IsAlive() || target->GetHealth() < 1 || target->IsImmune() || target->IsDormant())
			return false;

		if (target->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
			return false;

		return true;
	}

	C_CSPlayer* GetClosestPlayer(C_CSPlayer* pLocal, float fov, ECSPlayerBones bone) const
	{
		C_CSPlayer* pTarget = nullptr;
		auto maxFov = fov;

		QAngle aimAngles;
		Interfaces::Engine()->GetViewAngles(aimAngles);
		aimAngles += GetAimPunchCorrection(pLocal);

		const auto eyePos = pLocal->GetEyePos();

		for (auto i = 1; i <= Interfaces::Engine()->GetMaxClients(); i++)
		{
			const auto pPotentialTarget = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			
			if (!IsTargetValid(pLocal, pPotentialTarget))
				continue;

			if (!IsTargetBetter(pPotentialTarget, aimAngles, eyePos, maxFov, bone))
				continue;

			pTarget = pPotentialTarget;
		}
		return pTarget;
	}

	bool IsTargetValid(C_CSPlayer* pLocal, C_CSPlayer* pTarget) const
	{
		if (!pTarget || pTarget == pLocal)
			return false;

		if (!pTarget->IsAlive() || pTarget->IsDormant() || pTarget->IsImmune())
			return false;

		if (pTarget->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
			return false;

		return true;
	}

	bool IsTargetBetter(C_CSPlayer* pPotentialTarget, Vector aimAngles, Vector eyePos, float& maxFov, ECSPlayerBones bone) const
	{
		const auto vTargetBone = Utils::GetEntityBone(pPotentialTarget, bone);
		const auto fTargetFov = Options::g_bAimAssistDistanceBasedFov
			? GetFovFraction(aimAngles, Utils::CalcAngle(eyePos, vTargetBone))
			: GetFov(aimAngles, Utils::CalcAngle(eyePos, vTargetBone));

		if (fTargetFov > maxFov)
			return false;

		maxFov = fTargetFov;
		return true;
	}

	Vector GetAimPunchCorrection(C_CSPlayer* pLocal) const
	{
		if (Options::g_bRCSEnabled)
			return Vector(0, 0, 0);

		auto aimPunchCorrection = *pLocal->AimPunch() * 2.f;
		if (Options::g_fAimAssistRCSFailureChance < rand() % 100)
			return aimPunchCorrection;

		const auto failureAmount = rand() % 2 == 1
			? 1 + Options::g_fAimAssistRCSFailureAmount / 100.f
			: 1 - Options::g_fAimAssistRCSFailureAmount / 100.f;

		return aimPunchCorrection * failureAmount;
	}

	Vector GetDelta(C_CSPlayer* pLocal, C_CSPlayer* pTarget, ECSPlayerBones bone) const
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

	float GetFov(const QAngle& viewAngle, const QAngle& aimAngle) const
	{
		auto delta = aimAngle - viewAngle;
		Utils::Clamp(delta);
		return sqrtf(powf(delta.x, 2.f) + powf(delta.y, 2.f));
	}

	float GetFovFraction(const QAngle& viewAngle, const QAngle& aimAngle) const
	{
		auto delta = aimAngle - viewAngle;
		Utils::Clamp(delta);
		return sin(DEG2RAD(delta.Length()) / 2.f) * 180.f;
	}
};
