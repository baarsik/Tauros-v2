#pragma once

class EventListener : public IGameEventListener2
{
public:
	EventListener()
	{
		Interfaces::EventManager()->AddListener(this, XorStr("player_hurt"), false);
		Interfaces::EventManager()->AddListener(this, XorStr("player_spawned"), false);
		Interfaces::EventManager()->AddListener(this, XorStr("round_start"), false);
		Interfaces::EventManager()->AddListener(this, XorStr("bomb_planted"), false);
		Interfaces::EventManager()->AddListener(this, XorStr("bomb_defused"), false);
		Interfaces::EventManager()->AddListener(this, XorStr("bomb_exploded"), false);
	}
	~EventListener()
	{
		Interfaces::EventManager()->RemoveListener(this);
	}

	void FireGameEvent(IGameEvent* event) override
	{
		if (!strcmp(event->GetName(), XorStr("player_hurt")))
		{
			const auto pVictim = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt(XorStr("userid")))));
			const auto pAttacker = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt(XorStr("attacker")))));
			const auto health = event->GetInt(XorStr("health"));
			const auto armor = event->GetInt(XorStr("armor"));
			const auto weapon = event->GetString(XorStr("weapon"));
			const auto dmg_health = event->GetInt(XorStr("dmg_health"));
			const auto dmg_armor = event->GetInt(XorStr("dmg_armor"));
			const auto hitgroup = event->GetInt(XorStr("hitgroup")); // 1 = hs 2 = upper torso 3 = lower torso 4 = left arm 5 = right arm 6 = left leg 7 = right leg
			Container::Instance().Resolve<DamageInformer>()->OnPlayerHurt(pVictim, pAttacker, health, armor, weapon, dmg_health, dmg_armor, hitgroup);
		}
		else if (!strcmp(event->GetName(), XorStr("player_spawned")))
		{
			const auto pLocal = C_CSPlayer::GetLocalPlayer();
			const auto pSpawned = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt(XorStr("userid")))));
			const auto inrestart = event->GetBool(XorStr("inrestart"));
			if (pLocal != pSpawned)
				return;

			Container::Instance().Resolve<C4Timer>()->OnLocalPlayerSpawn(inrestart);
			Container::Instance().Resolve<DamageInformer>()->OnLocalPlayerSpawn(inrestart);
			Container::Instance().Resolve<BackTrack>()->OnLocalPlayerSpawn(inrestart);
		}
		else if (!strcmp(event->GetName(), XorStr("round_start")))
		{
			const auto timelimit = event->GetInt(XorStr("timelimit"));
			const auto fraglimit = event->GetInt(XorStr("fraglimit"));
			const auto objective = event->GetString(XorStr("objective"));
			Container::Instance().Resolve<C4Timer>()->OnRoundStart(timelimit, fraglimit, objective);
		}
		else if (!strcmp(event->GetName(), XorStr("bomb_planted")))
		{
			const auto pPlanter = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt("userid"))));
			const auto site = event->GetInt(XorStr("site"));
			Container::Instance().Resolve<C4Timer>()->OnBombPlanted(pPlanter, site);
		}
		else if (!strcmp(event->GetName(), XorStr("bomb_defused")))
		{
			const auto pPlanter = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt("userid"))));
			const auto site = event->GetInt(XorStr("site"));
			Container::Instance().Resolve<C4Timer>()->OnBombDefused(pPlanter, site);
		}
		else if (!strcmp(event->GetName(), XorStr("bomb_exploded")))
		{
			const auto pPlanter = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetPlayerForUserID(event->GetInt("userid"))));
			const auto site = event->GetInt(XorStr("site"));
			Container::Instance().Resolve<C4Timer>()->OnBombExploded(pPlanter, site);
		}
	}

	int GetEventDebugID() override
	{
		return 42;
	}
};
