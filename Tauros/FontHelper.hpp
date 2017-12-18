#pragma once
#include "ImGUI/IconsFontAwesome.h"
#include "Fonts.hpp"
#include "ImGUI/imgui.h"

struct FontInfo
{
	std::string name;
	float size;
	float sizeOriginal;
	bool isFontAwesome;
	ImFont* font;
	void* ttfData;
};

class FontHelper
{
public:
	FontHelper(float autoScale)
	{
		m_flAutoScale = autoScale;
		SetupFonts();
	}

	ImFont* AddFont(std::string name, float size, void* ttfData, bool isFontAwesome)
	{
		return UpdateFont(name, size, ttfData, isFontAwesome);
	}

	ImFont* GetFont(std::string name)
	{
		for (auto f : m_vecFonts)
		{
			if (f.name == name)
				return f.font;
		}
		return nullptr;
	}

	void UpdateFonts(float autoScale)
	{
		if (m_flAutoScale == autoScale)
			return;

		m_flAutoScale = autoScale;
		for (auto f : m_vecFonts)
		{
			UpdateFont(f.name, f.size, f.ttfData, f.isFontAwesome);
		}
	}
private:
	float m_flAutoScale;
	std::vector<FontInfo> m_vecFonts;

	void SetupFonts()
	{
		auto& io = ImGui::GetIO();
		io.Fonts->Clear();

		io.FontDefault = UpdateFont(XorStr("Default"), 14.f, &gostARawData, false);
		UpdateFont(XorStr("Menu button font"), 20.f, &gostARawData, false);
		UpdateFont(XorStr("Menu icon font"), 22.f, &fontAwesomeRawData, true);

		uint8_t* pixel_data;

		int width,
			height,
			bytes_per_pixel;

		io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height, &bytes_per_pixel);
	}

	ImFont* UpdateFont(std::string name, float size, void* ttfData, bool isFontAwesome)
	{
		static const ImWchar iconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		auto& io = ImGui::GetIO();
		ImFontConfig fontCfg;
		fontCfg.FontDataOwnedByAtlas = false;
		if (isFontAwesome)
		{
			fontCfg.MergeMode = true;
			fontCfg.PixelSnapH = true;
		}
		FontInfo font;
		auto fontExists = false;
		for (auto& f : m_vecFonts)
		{
			if (f.name != name)
				continue;

			fontExists = true;
			font = f;
		}
		font.sizeOriginal = size;
		font.size = size * m_flAutoScale;
		font.isFontAwesome = isFontAwesome;
		font.font = io.Fonts->AddFontFromMemoryTTF(&ttfData, sizeof ttfData, font.size, &fontCfg, isFontAwesome ? iconsRanges : io.Fonts->GetGlyphRangesCyrillic());
		font.ttfData = &ttfData;
		if (!fontExists) m_vecFonts.push_back(font);
		return font.font;
	}
};
