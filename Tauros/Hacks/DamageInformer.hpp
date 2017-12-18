#pragma once
#include "Options.hpp"
#include "Utils.hpp"

struct DamageIndicator_t
{
	int iDamage;
	int iDrawPosOffset;
	bool bInitialized;
	float flEraseTime;
	float flLastUpdate;
	C_CSPlayer * Player;
	Vector Position;
};

class DamageInformer
{
public:
	DamageInformer()
	{
		m_pGui = Container::Instance().Resolve<GUI>();
	}

	void PaintTraverse_Post()
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		const auto flCurrentTime = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick;
		for (size_t i = 0; i < m_vecDamageIndicators.size(); i++)
		{
			if (m_vecDamageIndicators[i].flEraseTime < flCurrentTime)
			{
				m_vecDamageIndicators.erase(m_vecDamageIndicators.begin() + i);
				continue;
			}

			if (!m_vecDamageIndicators[i].bInitialized)
			{
				m_vecDamageIndicators[i].Position = Utils::GetEntityBone(m_vecDamageIndicators[i].Player, HEAD_0);
				m_vecDamageIndicators[i].Position.z -= m_vecDamageIndicators[i].iDrawPosOffset;
				m_vecDamageIndicators[i].bInitialized = true;
			}

			if (flCurrentTime - m_vecDamageIndicators[i].flLastUpdate > 0.0001f)
			{
				m_vecDamageIndicators[i].Position.z -= 0.1f * (flCurrentTime - m_vecDamageIndicators[i].flEraseTime);
				m_vecDamageIndicators[i].flLastUpdate = flCurrentTime;
			}

			Vector vScreenPosition;
			if (!Utils::WorldToScreen(m_vecDamageIndicators[i].Position, vScreenPosition))
				continue;

			if (m_vecDamageIndicators[i].iDamage >= 100)
				DrawStringExtraBold(static_cast<int>(vScreenPosition.x), static_cast<int>(vScreenPosition.y), 255, 75, 75, 255, std::to_string(m_vecDamageIndicators[i].iDamage).c_str());
			else
				DrawString(static_cast<int>(vScreenPosition.x), static_cast<int>(vScreenPosition.y), 55, 175, 255, 255, std::to_string(m_vecDamageIndicators[i].iDamage).c_str());
		}
	}

	void OnPlayerHurt(C_CSPlayer* pVictim, C_CSPlayer* pAttacker, int health, int armor, const char* weapon, int dmg_health, int dmg_armor, int hitgroup)
	{
		if (!Options::g_bDamageInformerEnabled)
			return;

		auto pLocal = C_CSPlayer::GetLocalPlayer();

		if (pVictim != pLocal && pAttacker == pLocal)
		{
			auto drawPosOffset = 0;
			for (auto const& indicator : m_vecDamageIndicators)
			{
				if (indicator.Player == pVictim)
					drawPosOffset += 16;
			}
			DamageIndicator_t damageIndicator;
			damageIndicator.iDamage = dmg_health;
			damageIndicator.iDrawPosOffset = drawPosOffset;
			damageIndicator.Player = pVictim;
			damageIndicator.flEraseTime = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick + 3.f;
			damageIndicator.bInitialized = false;
			m_vecDamageIndicators.push_back(damageIndicator);
		}
	}

	void OnLocalPlayerSpawn(bool inrestart)
	{
		m_vecDamageIndicators.clear();
	}
private:
	std::shared_ptr<GUI> m_pGui;
	std::vector<DamageIndicator_t> m_vecDamageIndicators;

	FontSize DrawString(int x, int y, int r, int g, int b, int a, const char *pszText) const
	{
		if (pszText == nullptr)
			return FontSize{ -1, -1 };

		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, pszText, -1, szString, 64);

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(18 * m_pGui->GetScale()),
				FW_BOLD, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_OUTLINE)
			);
			fontInitialized = true;
		}

		int width, height;
		Interfaces::MatSurface()->GetTextSize(font, szString, width, height);
		Interfaces::MatSurface()->DrawSetTextFont(font);
		Interfaces::MatSurface()->DrawSetTextPos(x - width / 2, y);
		Interfaces::MatSurface()->DrawSetTextColor(Color(r, g, b, a));
		Interfaces::MatSurface()->DrawPrintText(szString, wcslen(szString));
		return FontSize{ width, height };
	}

	FontSize DrawStringExtraBold(int x, int y, int r, int g, int b, int a, const char *pszText) const
	{
		if (pszText == nullptr)
			return FontSize{ -1, -1 };

		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, pszText, -1, szString, 64);

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(24 * m_pGui->GetScale()),
				FW_HEAVY, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_OUTLINE)
			);
			fontInitialized = true;
		}

		int width, height;
		Interfaces::MatSurface()->GetTextSize(font, szString, width, height);
		Interfaces::MatSurface()->DrawSetTextFont(font);
		Interfaces::MatSurface()->DrawSetTextPos(x - width / 2, y);
		Interfaces::MatSurface()->DrawSetTextColor(Color(r, g, b, a));
		Interfaces::MatSurface()->DrawPrintText(szString, wcslen(szString));
		return FontSize{ width, height };
	}
};