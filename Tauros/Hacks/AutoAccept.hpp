#pragma once

class AutoAccept
{
public:
	void EmitSound_Pre(IRecipientFilter& filter, int entIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, float attenuation, int flags, int pitch, const Vector* origin, const Vector* direction, void* utlVecOrigins, bool updatePositions, float soundTime, int speakerEntity, /*StartSoundParams_t&*/ void* params) const
	{
		if (!Options::g_bAutoAcceptEnabled || Interfaces::Engine()->IsInGame())
			return;

		if (strcmp(soundEntry, "UIPanorama.popup_accept_match_beep"))
			return;

		Container::Instance().Resolve<SignatureHelper>()->AcceptCompetitiveMatch();
		Utils::FlashTaskbarIcon();
	}
};