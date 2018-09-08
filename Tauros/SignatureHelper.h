#pragma once

using SetClanTag_t          = void(__fastcall*)(const char*, const char*);
using SetLocalPlayerReady_t = bool(__stdcall*)(const char*);
using ServerRankRevealAll_t = bool(__cdecl*)(int*);
using LineThroughSmoke_t    = bool(__cdecl*)(Vector, Vector, int16_t);
using LoadNamedSky_t        = void(__fastcall*)(const char*);

class SignatureHelper
{
public:
	SignatureHelper();

	unsigned D3DDevice() const;
	void SetClanTag(const char* tag, const char* name) const;
	void AcceptCompetitiveMatch() const;
	void ServerRankRevealAll() const;
	bool LineThroughSmoke(Vector loc, Vector rem) const;
	CUserCmd*& CurrentCommand(C_CSPlayer* pPlayer) const;
	int* PredictionRandomSeed() const;
	const VMatrix& ViewMatrix() const;
	bool WorldToScreen(const Vector &point, Vector &out) const;

private:
	unsigned m_pD3DDevice;
	SetClanTag_t m_fnSetClanTag;
	SetLocalPlayerReady_t m_fnSetLocalPlayerReady;
	ServerRankRevealAll_t m_fnServerRankRevealAll;
	LineThroughSmoke_t m_fnLineThroughSmoke;
	unsigned m_pCurrentCommand;
	int* m_pPredictionRandomSeed;
	unsigned m_pViewMatrix;
};