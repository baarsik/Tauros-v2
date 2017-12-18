#pragma once

struct LBYRecords
{
	int tickCount;
	float lby;
	Vector headPosition;
};

struct BacktrackData
{
	float simTime;
	Vector aimTargetPos;
};

class BackTrack
{
public:
	BackTrack()
	{
		m_iLatestTick = Interfaces::GlobalVars()->tickcount;
	}

	void CreateMove_Post(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		if (!Options::g_bBackTrackingEnabled || !pLocal || !pLocal->IsAlive())
			return;

		const auto bestTargetIndex = UpdateBestTarget(pLocal, pCmd);
		if (bestTargetIndex == -1)
			return;

		BacktrackData* bestBacktrackData = nullptr;
		auto tempFloat = FLT_MAX;
		const auto viewDirection = pCmd->viewangles + *pLocal->AimPunch() * 2.f;
		for (auto t = 0; t < 12; ++t)
		{
			const auto tempFovDistance = Utils::DistancePointToLine(m_headPositions[bestTargetIndex][t].aimTargetPos, pLocal->GetEyePos(), viewDirection);
			if (tempFloat > tempFovDistance && m_headPositions[bestTargetIndex][t].simTime > pLocal->GetSimulationTime() - 1)
			{
				tempFloat = tempFovDistance;
				bestBacktrackData = &m_headPositions[bestTargetIndex][t];
			}
		}
		if (pCmd->buttons & IN_ATTACK && bestBacktrackData)
		{
			pCmd->tick_count = static_cast<int>(bestBacktrackData->simTime / Interfaces::GlobalVars()->interval_per_tick);
		}
	}

	void FrameStageNotify_Post(ClientFrameStage_t stage)
	{
		if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;

		m_iLatestTick = Interfaces::GlobalVars()->tickcount;
		for (auto i = 0; i < 64; i++)
			UpdateRecord(i);
	}

	void OnLocalPlayerSpawn(bool inrestart)
	{
		memset(&m_headPositions[0][0], 0, sizeof BacktrackData * 768);
	}
private:
	int m_iLatestTick;
	LBYRecords m_records[64];
	BacktrackData m_headPositions[64][12];

	bool IsTickValid(int tick) const
	{
		const auto deltaTime = (m_iLatestTick - tick) * Interfaces::GlobalVars()->interval_per_tick;
		return fabs(deltaTime) <= 0.2f;
	}

	void UpdateRecord(int i)
	{
		auto pEntity = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
		if (pEntity && pEntity->IsAlive() && !pEntity->IsDormant())
		{
			const auto lby = pEntity->GetLowerBodyYaw();
			if (lby != m_records[i].lby)
			{
				m_records[i].tickCount = m_iLatestTick;
				m_records[i].lby = lby;
				m_records[i].headPosition = Utils::GetEntityBone(pEntity, ECSPlayerBones::HEAD_0);
			}
		}
		else
		{
			m_records[i].tickCount = 0;
		}
	}

	int UpdateBestTarget(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		if (!pLocal)
			return -1;

		auto bestTargetIndex = -1;
		auto bestFov = FLT_MAX;
		player_info_t info;
		for (auto i = 0; i < Interfaces::Engine()->GetMaxClients(); i++)
		{
			auto pTarget = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			if (!pTarget || pTarget == pLocal || pTarget->IsDormant() || !pTarget->IsAlive() || pTarget->IsImmune())
				continue;

			if (!Interfaces::Engine()->GetPlayerInfo(i, &info))
				continue;

			if (pTarget->GetTeamNum() == pLocal->GetTeamNum() && !Options::g_bDeathmatch)
				continue;

			const auto hitboxPos = Utils::GetEntityBone(pTarget, ECSPlayerBones::HEAD_0);
			m_headPositions[i][pCmd->command_number % 13] = BacktrackData
			{
				pTarget->GetSimulationTime(),
				hitboxPos
			};
			const auto viewDirection = pCmd->viewangles + *pLocal->AimPunch() * 2.f;
			const auto fovDistance = Utils::DistancePointToLine(hitboxPos, pLocal->GetEyePos(), viewDirection);

			if (bestFov > fovDistance)
			{
				bestFov = fovDistance;
				bestTargetIndex = i;
			}
		}
		return bestTargetIndex;
	}
};