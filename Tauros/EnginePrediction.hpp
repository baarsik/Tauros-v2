#pragma once

class PredictionSystem
{
public:
	PredictionSystem()
	{
		m_pPredictionRandomSeed = Container::Instance().Resolve<SignatureHelper>()->PredictionRandomSeed();
		m_flOldCurtime = Interfaces::GlobalVars()->curtime;
		m_flOldFrametime = Interfaces::GlobalVars()->frametime;
	}

	void StartPrediction(C_CSPlayer* pLocal, CUserCmd* pCmd)
	{
		Interfaces::MoveHelper()->SetHost(pLocal);
		Container::Instance().Resolve<SignatureHelper>()->CurrentCommand(pLocal) = pCmd;
		*m_pPredictionRandomSeed = pCmd->random_seed;

		m_flOldCurtime = Interfaces::GlobalVars()->curtime;
		m_flOldFrametime = Interfaces::GlobalVars()->frametime;

		Interfaces::GlobalVars()->curtime = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick;
		Interfaces::GlobalVars()->frametime = Interfaces::GlobalVars()->interval_per_tick;

		Interfaces::GameMovement()->StartTrackPredictionErrors(pLocal);

		memset(&m_MoveData, 0, sizeof m_MoveData);
		Interfaces::Prediction()->SetupMove(pLocal, pCmd, Interfaces::MoveHelper(), &m_MoveData);
		Interfaces::GameMovement()->ProcessMovement(pLocal, &m_MoveData);
		Interfaces::Prediction()->FinishMove(pLocal, pCmd, &m_MoveData);

	}
	void EndPrediction(C_CSPlayer* pLocal) const
	{
		Interfaces::GameMovement()->FinishTrackPredictionErrors(pLocal);
		Interfaces::GlobalVars()->curtime = m_flOldCurtime;
		Interfaces::GlobalVars()->frametime = m_flOldFrametime;
		*m_pPredictionRandomSeed = -1;
		Container::Instance().Resolve<SignatureHelper>()->CurrentCommand(pLocal) = nullptr;
		Interfaces::MoveHelper()->SetHost(nullptr);
	}

private:
	float m_flOldCurtime;
	float m_flOldFrametime;
	CMoveData m_MoveData;

	int* m_pPredictionRandomSeed;
};