#pragma once
#include "ImGUI/IconsFontAwesome.h"
#include "Fonts.hpp"
#include "DrawManager.hpp"

enum Window : int
{
	Main = 0,
	Aim,
	Visuals,
	Skinchanger,
	Automation,
	Configs
};    

std::vector<std::string> TargetsAffected = { XorStr("None"), XorStr("Allies"), XorStr("Enemies"), XorStr("Allies and enemies") };

class GUI
{
public:
	explicit GUI(IDirect3DDevice9* pDevice)
	{
		UpdateSize(pDevice);  
		m_openedWindow = Window::Main;
		m_bIsClicked = false;
		m_bIsDown = false;
	}

	void SetStyle() const
	{
		auto style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15 * m_flAutoScale, 15 * m_flAutoScale);
		style->WindowRounding = 0.0f;
		style->FramePadding = ImVec2(5 * m_flAutoScale, 5 * m_flAutoScale);
		style->FrameRounding = 4.0f * m_flAutoScale;
		style->ItemSpacing = ImVec2(12 * m_flAutoScale, 8 * m_flAutoScale);
		style->ItemInnerSpacing = ImVec2(8 * m_flAutoScale, 6 * m_flAutoScale);
		style->IndentSpacing = 25.0f * m_flAutoScale;
		style->ScrollbarSize = 15.0f * m_flAutoScale;
		style->ScrollbarRounding = 9.0f * m_flAutoScale;
		style->GrabMinSize = 5.0f * m_flAutoScale;
		style->GrabRounding = 3.0f * m_flAutoScale;

		SetButtonStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.7f);
		style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2058f, 0.3066f, 0.4877f, 1.0f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1734f, 0.2995f, 0.5270f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.2330f, 0.3125f, 0.4545f, 1.00f);
		style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4("#bb3c3c");
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4("#c15454");
		style->Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	}

	void SetButtonStyle(bool currentWindow = false) const
	{
		const auto style = &ImGui::GetStyle();
		if (currentWindow)
		{
			style->Colors[ImGuiCol_Button] = ImVec4("#2e2327");
			style->Colors[ImGuiCol_ButtonHovered] = ImVec4("#352529");
			style->Colors[ImGuiCol_ButtonActive] = ImVec4("#2e2327");
			style->Colors[ImGuiCol_Text] = ImVec4("#fb4248");
		}
		else
		{
			style->Colors[ImGuiCol_Button] = ImVec4("#202125");
			style->Colors[ImGuiCol_ButtonHovered] = ImVec4("#2e2327");
			style->Colors[ImGuiCol_ButtonActive] = ImVec4("#2e2327");
			style->Colors[ImGuiCol_Text] = ImVec4("#b9babe");
		}
	}

	void DrawSpecialText(std::string text, std::string textBefore, bool sameLine = false, bool defaultColor = false) const
	{
		const auto style = &ImGui::GetStyle();
		if (defaultColor) ImGui::Text(text.c_str());
		else ImGui::TextColored(ImVec4("#ff0000"), text.c_str());
		if (sameLine) ImGui::SameLine(style->WindowPadding.x + ImGui::CalcTextSize((textBefore + text).c_str()).x);
	}

	void DrawSpecialText(std::string text, std::string textBefore, bool sameLine, ImVec4 color) const
	{
		const auto style = &ImGui::GetStyle();
		ImGui::TextColored(color, text.c_str());
		if (sameLine) ImGui::SameLine(style->WindowPadding.x + ImGui::CalcTextSize((textBefore + text).c_str()).x);
	}

	void Show()
	{
		if (!Options::g_bMainWindowOpen)
			return;

		const auto style = &ImGui::GetStyle();

		SetStyle();
		const auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_iClientWidth), 70 * m_flAutoScale));
		style->ItemSpacing = ImVec2(0, 0);
		ImGui::PushFont(m_menuButtonFont);
		ImGui::Begin(XorStr("Cerberus.Tauros"), nullptr, flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
		auto totalWidth = -1.f;
		std::vector<const char*> vecWindowNames = {
			XorStr(ICON_FA_INFO " Main"),
			XorStr(ICON_FA_CROSSHAIRS " Aim"),
			XorStr(ICON_FA_EYE " Visuals"),
			XorStr(ICON_FA_PAINT_BRUSH " Skin Changer"),
			XorStr(ICON_FA_COGS " Automation"),
			XorStr(ICON_FA_SLIDERS " Configs")
		};
		for (auto windowName : vecWindowNames)
		{
			totalWidth += ImGui::CalcTextSize(windowName).x + 48 * m_flAutoScale + 1;
		}
		const auto menuStart = ImVec2((m_iClientWidth - totalWidth) / 2.f, style->WindowPadding.y);
		const auto menuEnd = ImVec2(totalWidth + menuStart.x, menuStart.y + 48 * m_flAutoScale);
		ImGui::SameLine(menuStart.x);
		ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImVec2(menuStart.x - 1, menuStart.y - 1), ImVec2(menuEnd.x + 1, menuEnd.y + 1), ImGui::GetColorU32(ImVec4("#fb4248")), style->FrameRounding);
		for (int i = Window::Main; i <= Window::Configs; i++)
		{
			SetButtonStyle(m_openedWindow == i);
			const auto roundingCorners = i == Window::Main ? ~(2 | 4) : i == Window::Configs ? ~(1 | 8) : ~(1 | 2 | 4 | 8);
			if (ImGui::ButtonEx(vecWindowNames[i], ImVec2(ImGui::CalcTextSize(vecWindowNames[i]).x + 48 * m_flAutoScale, 48 * m_flAutoScale), 0, roundingCorners))
				m_openedWindow = Window(i);

			if (i != Window::Configs) ImGui::SameLine(0, 1.f);
		}
		ImGui::End();
		ImGui::PopFont();

		SetStyle();
		ImGui::SetNextWindowPos(ImVec2(0, 70 * m_flAutoScale));
		ImGui::SetNextWindowSize(ImVec2(static_cast<float>(m_iClientWidth), m_iClientHeight - 70.f * m_flAutoScale));
		ImGui::PushFont(nullptr);
		ImGui::Begin(XorStr(""), &Options::g_bMainWindowOpen, flags | ImGuiWindowFlags_NoTitleBar);
		if (m_openedWindow == Window::Main)
		{
			DrawSpecialText(XorStr("Cerberus"), XorStr(""), true, false);
			DrawSpecialText(XorStr("."), XorStr("Cerberus"), true, true); 
			DrawSpecialText(XorStr("Tauros"), XorStr("Cerberus. "), true, ImVec4(1.f, 138.f / 255.f, 0.f, 1.f));
			DrawSpecialText(std::string(XorStr("(")) + XorStr(__DATE__) + XorStr(")"), XorStr("Cerberus.Tauros "), false, true);

			ImGui::TextWrapped(XorStr("\nCerberus.Tauros hack is serving one purpose: provide better competetive matchmaking experiense by giving you extra information and enhancing your skills with software assistance. It is strictly recommended to play only legit settings and refuse from constant bunnyhop usage. Enjoy your legit Global Elite rankup."));

			ImGui::Text(XorStr(u8"\nCoded by Барс"));
			DrawSpecialText(XorStr("Skype: "), XorStr(""), true, true);
			DrawSpecialText(XorStr("baarsik2"), XorStr("Skype: "));

#ifdef _DEBUG
			DrawSpecialText(XorStr("\n[Debug info]"), XorStr(""), false, ImVec4(1.f, 138.f / 255.f, 0.f, 1.f));
			DrawSpecialText(XorStr("Resolution: "), XorStr(""), true, false);
			DrawSpecialText(std::to_string(m_iClientWidth) + XorStr("x") + std::to_string(m_iClientHeight), XorStr("Resolution: "), false, true);
			DrawSpecialText(XorStr("UI scale size: "), XorStr(""), true, false);
			DrawSpecialText(std::to_string(m_flAutoScale), XorStr("UI scale size: "), false, true);
#endif
		}
		else if (m_openedWindow == Window::Aim)
		{
			ImGui::Columns(3, nullptr, false);
			DrawSpecialText(XorStr("Aim Assist"), XorStr(""));
			ImGui::Checkbox(XorStr("Enabled"), &Options::g_bAimAssistEnabled);
			ImGui::Checkbox(XorStr("Deathmatch"), &Options::g_bDeathmatch);
			ImGui::Combo(XorStr("Lock target"), &Options::g_iAimAssistLockTarget, Options::g_szAimAssistLockTarget, IM_ARRAYSIZE(Options::g_szAimAssistLockTarget));
			ImGui::Checkbox(XorStr("Ignore obstacles"), &Options::g_bAimAssistIgnoreObstacles);
			ImGui::Checkbox(XorStr("Auto aim"), &Options::g_bAimAssistAutoShoot);
			ImGui::Checkbox(XorStr("Lock mouse"), &Options::g_bAimAssistLockMouse);
			ImGui::Checkbox(XorStr("Ignore smoke"), &Options::g_bAimAssistIgnoreSmoke);
			ImGui::Checkbox(XorStr("Scope required (Sniper)"), &Options::g_bAimAssistSniperScopedOnly);
			ImGui::Checkbox(XorStr("Distance based FOV"), &Options::g_bAimAssistDistanceBasedFov);
			ImGui::SliderFloat(XorStr("FOV (Rifle)"), &Options::g_fAimAssistFov, 0.f, 60.f, XorStr("%.2f"));
			ImGui::SliderFloat(XorStr("Smooth (Rifle)"), &Options::g_fAimAssistSmooth, 0.022f, 1.5f, XorStr("%.4f"));
			ImGui::SliderFloat(XorStr("FOV (Pistol)"), &Options::g_fAimAssistFovPistol, 0.f, 60.f, XorStr("%.2f"));
			ImGui::SliderFloat(XorStr("Smooth (Pistol)"), &Options::g_fAimAssistSmoothPistol, 0.022f, 1.5f, XorStr("%.4f"));
			ImGui::SliderFloat(XorStr("Vertical smooth multiplier"), &Options::g_fAimAssistVerticalSmoothMultiplier, 1.f, 5.f, XorStr("%.2f"));

			ImGui::NextColumn();
			DrawSpecialText(XorStr("Trigger Bot"), XorStr(""));
			ImGui::Checkbox(XorStr("Enabled "), &Options::g_bTriggerEnabled);
			ImGui::Checkbox(XorStr("Deathmatch "), &Options::g_bDeathmatch);
			ImGui::Combo(XorStr("Trigger key"), &Options::g_iTriggerKey, Options::Keys, IM_ARRAYSIZE(Options::Keys));
			ImGui::Checkbox(XorStr("Always active"), &Options::g_bTriggerAlwaysActive);
			ImGui::Checkbox(XorStr("Ignore smoke "), &Options::g_bTriggerIgnoreSmoke);
			ImGui::Checkbox(XorStr("Aim synergy [Requires AimAssist enabled]"), &Options::g_bTriggerAimSynergy);
			ImGui::Checkbox(XorStr("Scope required (Sniper) "), &Options::g_bTriggerSniperScopedOnly);

			ImGui::NextColumn();
			DrawSpecialText(XorStr("Misc"), XorStr(""));
			ImGui::Checkbox(XorStr("No recoil"), &Options::g_bRCSEnabled);
			ImGui::Checkbox(XorStr("Back tracking"), &Options::g_bBackTrackingEnabled);
		}
		else if (m_openedWindow == Window::Visuals)
		{
			ImGui::Columns(3, nullptr, false);
			DrawSpecialText(XorStr("ESP"), XorStr(""));
			ImGui::Checkbox(XorStr("Enabled"), &Options::g_bESPEnabled);
			ImGui::Checkbox(XorStr("Deathmatch"), &Options::g_bDeathmatch);
			ImGui::Combo(XorStr("Boxes"), &Options::g_iESPShowBoxes, TargetsAffected);
			ImGui::Combo(XorStr("Health"), &Options::g_iESPShowHealth, TargetsAffected);
			ImGui::Combo(XorStr("Bones"), &Options::g_iESPShowBones, TargetsAffected);
			ImGui::Combo(XorStr("Names"), &Options::g_iESPShowNames, TargetsAffected);
			ImGui::Combo(XorStr("Weapons"), &Options::g_iESPShowWeapon, TargetsAffected);
			ImGui::Combo(XorStr("Distance"), &Options::g_iESPShowDistance, TargetsAffected);
			ImGui::Text(XorStr(""));
			ImGui::ColorEdit3(XorStr("Enemy color"), Options::g_fESPEnemyColor);
			ImGui::ColorEdit3(XorStr("Ally color"), Options::g_fESPAllyColor);

			ImGui::NextColumn();
			DrawSpecialText(XorStr("Chams"), XorStr(""));
			ImGui::Checkbox(XorStr("Enabled "), &Options::g_bChamsEnabled);
			ImGui::Checkbox(XorStr("Deathmatch "), &Options::g_bDeathmatch);
			ImGui::Checkbox(XorStr("Show invisible models"), &Options::g_bChamsShowInvisible);
			ImGui::Combo(XorStr("Visible material"), &Options::g_iChamsVisibleMaterialId, MatHelper.GetMaterialNames(false));
			ImGui::ColorEdit3(XorStr("Visible color"), Options::g_fChamsVisibleColor);
			ImGui::Combo(XorStr("Invisible material"), &Options::g_iChamsInvisibleMaterialId, MatHelper.GetMaterialNames(true));
			ImGui::ColorEdit3(XorStr("Invisible color"), Options::g_fChamsInvisibleColor);
			ImGui::Text(XorStr(""));
			DrawSpecialText(XorStr("DLights"), XorStr(""));
			ImGui::Checkbox(XorStr("Enabled DLight"), &Options::g_bDLightEnabled);
			ImGui::Checkbox(XorStr("Enabled ELight"), &Options::g_bELightEnabled);
			ImGui::Checkbox(XorStr("Color by teams"), &Options::g_bDLightColorByTeam);
			if (Options::g_bDLightColorByTeam)
			{
				ImGui::ColorEdit3(XorStr("T color "), Options::g_fDLightEnemyTColor);
				ImGui::ColorEdit3(XorStr("CT color "), Options::g_fDLightAllyCTColor);
			}
			else
			{
				ImGui::ColorEdit3(XorStr("Enemy color "), Options::g_fDLightEnemyTColor);
				ImGui::ColorEdit3(XorStr("Ally color "), Options::g_fDLightAllyCTColor);
				ImGui::Checkbox(XorStr("Deathmatch  "), &Options::g_bDeathmatch);
			}

			ImGui::NextColumn();
			DrawSpecialText(XorStr("Misc"), XorStr(""));
			ImGui::Checkbox(XorStr("Clean screenshots"), &Options::g_bCleanScreenshot);
			ImGui::Checkbox(XorStr("C4 explode timer"), &Options::g_bC4TimerEnabled);
			ImGui::Checkbox(XorStr("Show damage"), &Options::g_bDamageInformerEnabled);
			ImGui::Checkbox(XorStr("Gray walls"), &Options::g_bGrayWallsEnabled);
			ImGui::Checkbox(XorStr("No smoke"), &Options::g_bNoSmokeEnabled);
			ImGui::Checkbox(XorStr("No flash"), &Options::g_bNoFlashEnabled);
			ImGui::SliderFloat(XorStr("Max flash"), &Options::g_fNoFlashAmount, 0.f, 100.f, XorStr("%.0f%%"));
			ImGui::Combo(XorStr("Hands style"), &Options::g_iHandsDisplay, Options::g_szHandsDisplay, IM_ARRAYSIZE(Options::g_szHandsDisplay));
			ImGui::Checkbox(XorStr("Reveal ranks"), &Options::g_bRankRevealerEnabled);
		}
		else if (m_openedWindow == Window::Skinchanger)
		{
			ImGui::Columns(3, nullptr, false);
			ImGui::Checkbox(XorStr("Enabled"), &Options::g_bSkinChangerEnabled);
		}
		else if (m_openedWindow == Window::Automation)
		{
			ImGui::Columns(3, nullptr, false);
			ImGui::Checkbox(XorStr("Auto pistol"), &Options::g_bAutoPistolEnabled);
			ImGui::Checkbox(XorStr("Bunny hop"), &Options::g_bBHopEnabled);
			ImGui::Checkbox(XorStr("Auto strafe"), &Options::g_bAutoStrafeEnabled);
			ImGui::Checkbox(XorStr("Auto accept"), &Options::g_bAutoAcceptEnabled);
		}
		else if (m_openedWindow == Window::Configs)
		{
			ImGui::Columns(3, nullptr, false);
		}
		ImGui::End();
		ImGui::PopFont();
	}

	bool CheckToggle(bool vecPressedKeys[256])
	{
		if (vecPressedKeys[VK_F11] || vecPressedKeys[VK_INSERT])
		{
			m_bIsClicked = false;
			m_bIsDown = true;
		}
		else if (!vecPressedKeys[VK_F11] && !vecPressedKeys[VK_INSERT] && m_bIsDown)
		{
			m_bIsClicked = true;
			m_bIsDown = false;

			Options::g_bMainWindowOpen = !Options::g_bMainWindowOpen;
			static auto cl_mouseenable = Interfaces::CVar()->FindVar(XorStr("cl_mouseenable"));
			cl_mouseenable->SetValue(!Options::g_bMainWindowOpen);
		}
		else
		{
			m_bIsClicked = false;
			m_bIsDown = false;
		}
		return m_bIsClicked;
	}

	void UpdateCursorVisibility() const
	{
		ImGui::GetIO().MouseDrawCursor = Options::g_bMainWindowOpen;
	}

	void UpdateSize(IDirect3DDevice9* pDevice)
	{
		D3DDEVICE_CREATION_PARAMETERS cparams;
		pDevice->GetCreationParameters(&cparams);

		RECT rect;
		GetWindowRect(cparams.hFocusWindow, &rect);
		m_iClientWidth = rect.right - rect.left;
		m_iClientHeight = rect.bottom - rect.top;
		m_flAutoScale = m_iClientWidth <= 1920 ? 1.f : m_iClientWidth / 1920.f;

		UpdateFonts();
	}

	int GetWidth() const { return m_iClientWidth; }
	int GetHeight() const { return m_iClientHeight; }
	float GetScale() const { return m_flAutoScale; }
private:
	int m_iClientWidth;
	int m_iClientHeight;
	float m_flAutoScale;
	Window m_openedWindow;
	ImFont* m_menuButtonFont;
	ImFont* m_iconFont;
	bool m_bIsDown;
	bool m_bIsClicked;

	void UpdateFonts()
	{
		auto& io = ImGui::GetIO();
		io.Fonts->Clear();

		ImFontConfig fontCfg; fontCfg.FontDataOwnedByAtlas = false;
		io.FontDefault = io.Fonts->AddFontFromMemoryTTF(&gostARawData, sizeof gostARawData, 14.f * m_flAutoScale, &fontCfg, io.Fonts->GetGlyphRangesCyrillic());
		m_menuButtonFont = io.Fonts->AddFontFromMemoryTTF(&gostARawData, sizeof gostARawData, 20.f * m_flAutoScale, &fontCfg, io.Fonts->GetGlyphRangesCyrillic());

		static const ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig iconsCfg; iconsCfg.MergeMode = true; iconsCfg.PixelSnapH = true; iconsCfg.FontDataOwnedByAtlas = false;
		m_iconFont = io.Fonts->AddFontFromMemoryTTF(&fontAwesomeRawData, sizeof fontAwesomeRawData, 22.f * m_flAutoScale, &iconsCfg, iconsRanges);
	}
};