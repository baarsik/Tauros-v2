#include "SourceEngine/SDK.hpp"
#include "SignatureHelper.h"
#include "Constants.h"

SignatureHelper::SignatureHelper()
{
	m_pD3DDevice = **Constants::Signatures::D3DDevice.Find<uint32_t**>();
	m_fnSetClanTag = Constants::Signatures::SetClanTagFn.Find<SetClanTag_t>();
	m_fnSetLocalPlayerReady = Constants::Signatures::SetLocalPlayerReadyFn.Find<SetLocalPlayerReady_t>();
	m_fnServerRankRevealAll = Constants::Signatures::ServerRankRevealAllFn.Find<ServerRankRevealAll_t>();
	m_fnLineThroughSmoke = Constants::Signatures::LineThroughSmokeFn.Find<LineThroughSmoke_t>();
	m_pCurrentCommand = *Constants::Signatures::CurrentCommand.Find<uint32_t*>();
	m_pPredictionRandomSeed = *Constants::Signatures::PredictionRandomSeed.Find<int**>();
	m_pViewMatrix = *Constants::Signatures::ViewMatrix.Find<uint32_t*>() + 176;
}

unsigned SignatureHelper::D3DDevice() const
{
	return m_pD3DDevice;
}

void SignatureHelper::SetClanTag(const char* tag, const char* name) const
{
	m_fnSetClanTag(tag, name);
}

void SignatureHelper::AcceptCompetitiveMatch() const
{
	m_fnSetLocalPlayerReady("");
}

void SignatureHelper::ServerRankRevealAll() const
{
	int param[3] = { 0, 0, 0 };
	m_fnServerRankRevealAll(param);
}

bool SignatureHelper::LineThroughSmoke(Vector loc, Vector rem) const
{
	return m_fnLineThroughSmoke(loc, rem, true);
}

CUserCmd*& SignatureHelper::CurrentCommand(C_CSPlayer* pPlayer) const
{
	return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(pPlayer) + m_pCurrentCommand);
}

int* SignatureHelper::PredictionRandomSeed() const
{
	return m_pPredictionRandomSeed;
}

const VMatrix& SignatureHelper::ViewMatrix() const
{
	return *reinterpret_cast<const VMatrix*>(m_pViewMatrix);
}

bool SignatureHelper::WorldToScreen(const Vector &point, Vector &out) const
{
	int width, height;
	Interfaces::Engine()->GetScreenSize(width, height);
	const auto& worldToScreen = ViewMatrix();
	const auto w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
	out.z = 0;
	if (w > 0.01)
	{
		const auto w1 = 1 / w;
		out.x = static_cast<vec_t>(width / 2 + (0.5 * ((worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3]) * w1) * width + 0.5));
		out.y = static_cast<vec_t>(height / 2 - (0.5 * ((worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3]) * w1) * height + 0.5));
		return true;
	}
	return false;
}