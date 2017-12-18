#pragma once

using SetClanTag_t          = void(__fastcall*)(const char*, const char*);
using IsReady_t             = void(__cdecl*)();
using ServerRankRevealAll_t = bool(__cdecl*)(int*);
using LineThroughSmoke_t    = bool(__cdecl*)(Vector, Vector, int16_t);
using LoadNamedSky_t        = void(__fastcall*)(const char*);

class SignatureHelper
{
public:
	SignatureHelper()
	{
		m_pD3DDevice = **reinterpret_cast<uint32_t**>(Utils::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 6A 00 53")) + 0x1);
		m_fnSetClanTag = reinterpret_cast<SetClanTag_t>(Utils::FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15")));
		m_fnIsReady = reinterpret_cast<IsReady_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("81 66 70 00 00 F0 FF C7 46 6C 00 00 00 00 C7 46 74 00 00 00 00 5E E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57")) + 0x28);
		m_fnServerRankRevealAll = reinterpret_cast<ServerRankRevealAll_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 8B 0D ? ? ? ? 68")));
		m_fnLineThroughSmoke = reinterpret_cast<LineThroughSmoke_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0")));
		m_pCurrentCommand = *reinterpret_cast<uint32_t*>(Utils::FindSignature(XorStr("client.dll"), XorStr("89 BE ? ? ? ? E8 ? ? ? ? 85 FF")) + 2);
		m_pPredictionRandomSeed = *reinterpret_cast<int**>(Utils::FindSignature(XorStr("client.dll"), XorStr("A3 ? ? ? ? 66 0F 6E 86")) + 1);
		m_pViewMatrix = *reinterpret_cast<unsigned*>(Utils::FindSignature(XorStr("client.dll"), XorStr("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")) + 0x3) + 176;
	}

	unsigned D3DDevice() const
	{
		return m_pD3DDevice;
	}

	void SetClanTag(std::string tag, std::string name) const
	{
		m_fnSetClanTag(tag.c_str(), name.c_str());
	}

	void IsReady() const
	{
		m_fnIsReady();
	}

	void ServerRankRevealAll() const
	{
		int param[3] = { 0, 0, 0 };
		m_fnServerRankRevealAll(param);
	}

	bool LineThroughSmoke(Vector loc, Vector rem) const
	{
		return m_fnLineThroughSmoke(loc, rem, true);
	}

	CUserCmd*& CurrentCommand(C_CSPlayer* pPlayer) const
	{
		return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(pPlayer) + m_pCurrentCommand);
	}

	int* PredictionRandomSeed() const
	{
		return m_pPredictionRandomSeed;
	}

	const VMatrix& ViewMatrix() const
	{
		return *reinterpret_cast<const VMatrix*>(m_pViewMatrix);
	}

	bool WorldToScreen(const Vector &point, Vector &out) const
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
private:
	unsigned m_pD3DDevice;
	SetClanTag_t m_fnSetClanTag;
	IsReady_t m_fnIsReady;
	ServerRankRevealAll_t m_fnServerRankRevealAll;
	LineThroughSmoke_t m_fnLineThroughSmoke;
	unsigned m_pCurrentCommand;
	int* m_pPredictionRandomSeed;
	unsigned m_pViewMatrix;
};