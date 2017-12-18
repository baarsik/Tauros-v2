#pragma once

class AutoAccept
{
public:
	void PlaySound_Post(const char* szFileName) const
	{
		if (!Options::g_bAutoAcceptEnabled || Interfaces::Engine()->IsInGame())
			return;

		if (strcmp(szFileName, "!UI/competitive_accept_beep.wav"))
			return;

		Container::Instance().Resolve<SignatureHelper>()->IsReady();
		Utils::FlashTaskbarIcon();
	}
};