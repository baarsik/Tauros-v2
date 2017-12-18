#pragma once

#include "Definitions.hpp"

class IPanel
{
public:
	bool IsVisible(VPANEL vguiPanel)
	{
		using IsEnabled_t = const char *(__thiscall*)(PVOID, VPANEL);
		return CallVFunction<IsEnabled_t>(this, 15)(this, vguiPanel);
	}

	const char* GetName(VPANEL vguiPanel)
	{
		using GetName_t = const char *(__thiscall*)(PVOID, VPANEL);
		return CallVFunction<GetName_t>(this, 36)(this, vguiPanel);
	}
#ifdef GetClassName
#undef GetClassName
#endif
	const char* GetClassName(VPANEL vguiPanel)
	{
		using GetClassName_t = const char *(__thiscall*)(PVOID, VPANEL);
		return CallVFunction<GetClassName_t>(this, 37)(this, vguiPanel);
	}
};