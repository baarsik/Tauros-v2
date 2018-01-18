#pragma once


class C4Timer
{
public:
	C4Timer()
	{
		m_pGui = Container::Instance().Resolve<GUI>();
		m_bIsPlanted = false;
		m_bIsDefused = false;
		m_iTimeLeft = 0;
		m_flExplodesIn = 0.f;
	}

	void PaintTraverse_Post()
	{
		if (!Options::g_bC4TimerEnabled || !m_bIsPlanted)
			return;

		if (!m_bIsDefused)
			m_iTimeLeft = static_cast<int>(m_flExplodesIn - C_CSPlayer::GetLocalPlayer()->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick);

		int width, height;
		Interfaces::MatSurface()->GetScreenSize(width, height);
		const auto textSize1 = GetTextSize(XorStr("EXPLODES IN"));
		const auto textSize2 = GetTextSizeHeavy(std::to_string(m_iTimeLeft).c_str());
		const auto x1 = width - 140 * m_pGui->GetScale();
		const auto y1 = static_cast<int>(height / 1.5f) - 15 * m_pGui->GetScale() - (textSize1.height + textSize2.height) / 2;
		const auto x2 = width;
		const auto y2 = y1 + 30 * m_pGui->GetScale() + textSize1.height + textSize2.height;

		Interfaces::MatSurface()->DrawSetColor(0, 0, 0, 200);
		Interfaces::MatSurface()->DrawFilledRect(int(x1), int(y1), int(x2), int(y2));
		auto x = (x1 + x2) / 2;
		auto y = y1 + 10 * m_pGui->GetScale();
		DrawString(int(x), int(y), 255, 255, 255, 255, XorStr("EXPLODES IN"));
		y += textSize1.height + 10 * m_pGui->GetScale();
		int r, g, b;
		if (m_bIsDefused) { r = 0; g = 255; b = 0; }
		else if (m_iTimeLeft < 5) { r = 255; g = 0; b = 0; }
		else { r = 255; g = 255; b = 255; }
		DrawStringHeavy(int(x), int(y), r, g, b, 255, std::to_string(m_iTimeLeft).c_str());
	}

	void OnBombPlanted(C_CSPlayer* planter, int site)
	{
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		m_flExplodesIn = pLocal->GetTickBase() * Interfaces::GlobalVars()->interval_per_tick + Interfaces::CVar()->FindVar(XorStr("mp_c4timer"))->GetInt() + 1;
		m_bIsDefused = false;
		m_bIsPlanted = true;
	}

	void OnBombExploded(C_CSPlayer* planter, int site)
	{
		m_bIsPlanted = false;
	}

	void OnBombDefused(C_CSPlayer* planter, int site)
	{
		m_bIsDefused = true;
	}

	void OnRoundStart(int timelimit, int fraglimit, const char* objective)
	{
		m_bIsPlanted = false;
	}

	void OnLocalPlayerSpawn(bool inrestart)
	{
		m_bIsPlanted = false;
	}
private:
	std::shared_ptr<GUI> m_pGui;
	bool m_bIsPlanted;
	bool m_bIsDefused;
	int m_iTimeLeft;
	float m_flExplodesIn;

	FontSize GetTextSize(const char* szText) const
	{
		if (szText == nullptr)
			return FontSize{ -1, -1 };

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(16 * m_pGui->GetScale()),
				FW_NORMAL, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_NONE)
			);
			fontInitialized = true;
		}

		int width, height;
		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, szText, -1, szString, 64);
		Interfaces::MatSurface()->GetTextSize(font, szString, width, height);
		return FontSize { width, height };
	}

	FontSize GetTextSizeHeavy(const char* szText) const
	{
		if (szText == nullptr)
			return FontSize{ -1, -1 };

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(36 * m_pGui->GetScale()),
				FW_HEAVY, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_NONE)
			);
			fontInitialized = true;
		}

		int width, height;
		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, szText, -1, szString, 64);
		Interfaces::MatSurface()->GetTextSize(font, szString, width, height);
		return FontSize{ width, height };
	}

	FontSize DrawString(int x, int y, int r, int g, int b, int a, const char* szText) const
	{
		if (szText == nullptr)
			return FontSize{ -1, -1 };

		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, szText, -1, szString, 64);

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(16 * m_pGui->GetScale()),
				FW_NORMAL, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_NONE)
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

	FontSize DrawStringHeavy(int x, int y, int r, int g, int b, int a, const char* szText) const
	{
		if (szText == nullptr)
			return FontSize{ -1, -1 };

		wchar_t szString[64];
		MultiByteToWideChar(CP_UTF8, 0, szText, -1, szString, 64);

		static HFont font;
		static auto fontInitialized = false;
		if (!fontInitialized)
		{
			Interfaces::MatSurface()->SetFontGlyphSet(
				font = Interfaces::MatSurface()->CreateFont(),
				XorStr("Tahoma"), int(36 * m_pGui->GetScale()),
				FW_HEAVY, 0, 0,
				static_cast<int>(FontFlags::FONTFLAG_NONE)
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