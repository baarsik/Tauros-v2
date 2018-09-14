#pragma once

#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "NetvarManager.hpp"
#include "XorStr.hpp"

class C_CSPlayer;

class C_BaseCombatWeapon : public IClientEntity
{
    template<class T>
    inline T GetFieldValue(int offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
    }

	template<class T>
	T* GetFieldPointer(int offset)
	{
		return reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
	}
public:
    C_CSPlayer* GetOwner()
    {
        static int m_hOwnerEntity = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"));
        return reinterpret_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<C_CSPlayer>>(m_hOwnerEntity)));
    }
    float NextPrimaryAttack()
    {
        static int m_flNextPrimaryAttack = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"));
        return GetFieldValue<float>(m_flNextPrimaryAttack);
    }
	int GetClip() {
		static int m_iClip = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"));
		return GetFieldValue<int>(m_iClip);
	}
	const char* GetName()
	{
		using GetName_t = const char*(__thiscall*)(void*);
		return CallVFunction<GetName_t>(this, 378)(this);
	}
	float C_BaseCombatWeapon::GetSpread()
	{
		using GetSpread_t = float(__thiscall*)(void*);
		return CallVFunction<GetSpread_t>(this, 436)(this);
	}
	CCSWeaponData* GetWeaponData()
	{
		using CCSWeaponInfo_t = CCSWeaponData*(__thiscall*)(void*);
		return CallVFunction<CCSWeaponInfo_t>(this, 444)(this);
	}
	float C_BaseCombatWeapon::GetInaccuracy()
	{
		using GetInaccuracy_t = float(__thiscall*)(void*);
		return CallVFunction<GetInaccuracy_t>(this, 467)(this);
	}
	void C_BaseCombatWeapon::UpdateAccuracyPenalty()
	{
		using UpdateAccuracyPenalty_t = void(__thiscall*)(void*);
		CallVFunction<UpdateAccuracyPenalty_t>(this, 468)(this);
	}

	std::string GetReadableName()
    {
	    switch (*ItemDefinitionIndex())
	    {
		case WEAPON_DEAGLE: return XorStr("Deagle");
		case WEAPON_ELITE: return XorStr("Dual Berettas");
		case WEAPON_FIVESEVEN: return XorStr("Five-Seven");
		case WEAPON_GLOCK: return XorStr("Glock-18");
		case WEAPON_AK47: return XorStr("AK-47");
		case WEAPON_AUG: return XorStr("AUG");
		case WEAPON_AWP: return XorStr("AWP");
		case WEAPON_FAMAS: return XorStr("FAMAS");
		case WEAPON_G3SG1: return XorStr("G3SG1");
		case WEAPON_GALILAR: return XorStr("Galil AR");
		case WEAPON_M249: return XorStr("M249");
		case WEAPON_M4A1: return XorStr("M4A4");
		case WEAPON_MAC10: return XorStr("MAC-10");
		case WEAPON_P90: return XorStr("P90");
		case WEAPON_MP5SD: return XorStr("MP5-SD");
		case WEAPON_UMP45: return XorStr("UMP-45");
		case WEAPON_XM1014: return XorStr("XM1014");
		case WEAPON_BIZON: return XorStr("PP-Bizon");
		case WEAPON_MAG7: return XorStr("MAG-7");
		case WEAPON_NEGEV: return XorStr("Negev");
		case WEAPON_SAWEDOFF: return XorStr("Sawed-Off");
		case WEAPON_TEC9: return XorStr("Tec-9");
		case WEAPON_TASER: return XorStr("Zeus x27");
		case WEAPON_HKP2000: return XorStr("P2000");
		case WEAPON_MP7: return XorStr("MP7");
		case WEAPON_MP9: return XorStr("MP9");
		case WEAPON_NOVA: return XorStr("Nova");
		case WEAPON_P250: return XorStr("P250");
		case WEAPON_SCAR20: return XorStr("SCAR-20");
		case WEAPON_SG556: return XorStr("SG 553");
		case WEAPON_SSG08: return XorStr("SSG 08");
		case WEAPON_FLASHBANG: return XorStr("Flashbang");
		case WEAPON_HEGRENADE: return XorStr("HE Grenade");
		case WEAPON_SMOKEGRENADE: return XorStr("Smoke Grenade");
		case WEAPON_MOLOTOV: return XorStr("Molotov");
		case WEAPON_DECOY: return XorStr("Decoy Grenade");
		case WEAPON_INCGRENADE: return XorStr("Incendiary Grenade");
		case WEAPON_C4: return XorStr("C4");
		case WEAPON_M4A1_SILENCER: return XorStr("M4A1-S");
		case WEAPON_USP_SILENCER: return XorStr("USP-S");
		case WEAPON_CZ75A: return XorStr("CZ75-Auto");
		case WEAPON_REVOLVER: return XorStr("R8 Revolver");
		case WEAPON_KNIFE: 
		case WEAPON_KNIFE_T:
		case WEAPON_KNIFE_BAYONET:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE_PUSH:
			return XorStr("Knife");
		default: return XorStr("Unknown");
	    }
    }
	
	// For skinchanger purpose
	int* FallbackPaintKit()
	{
		static int m_nFallbackPaintKit = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackPaintKit"));
		return GetFieldPointer<int>(m_nFallbackPaintKit);
	}
	int* FallbackSeed()
	{
		static int m_nFallbackSeed = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackSeed"));
		return GetFieldPointer<int>(m_nFallbackSeed);
	}
	float* FallbackWear()
	{
		static int m_flFallbackWear = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_flFallbackWear"));
		return GetFieldPointer<float>(m_flFallbackWear);
	}
	int* FallbackStatTrak()
	{
		static int m_nFallbackStatTrak = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_nFallbackStatTrak"));
		return GetFieldPointer<int>(m_nFallbackStatTrak);
	}
	short* ItemDefinitionIndex()
    {
	    static int m_iItemDefinitionIndex = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex"));
		return GetFieldPointer<short>(m_iItemDefinitionIndex);
    }
	int* EntityQuality()
	{
		static int m_iEntityQuality = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityQuality"));
		return GetFieldPointer<int>(m_iEntityQuality);
	}
	int* EntityLevel()
	{
		static int m_iEntityLevel = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iEntityLevel"));
		return GetFieldPointer<int>(m_iEntityLevel);
	}
	int* ItemIDHigh()
	{
		static int m_iItemIDHigh = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDHigh"));
		return GetFieldPointer<int>(m_iItemIDHigh);
	}
	int* ItemIDLow()
	{
		static int m_iItemIDLow = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemIDLow"));
		return GetFieldPointer<int>(m_iItemIDLow);
	}
	int* AccountID()
	{
		static int m_iAccountID = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iAccountID"));
		return GetFieldPointer<int>(m_iAccountID);
	}
	char* CustomName() {
		static int m_szCustomName = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_szCustomName"));
		return GetFieldPointer<char>(m_szCustomName);
	}

	// Weapon type checks
	bool IsPistol()
    {
		return GetWeaponData()->WeaponType == WEAPONTYPE_PISTOL;
    }
	bool IsGrenade()
    {
		return GetWeaponData()->WeaponType == WEAPONTYPE_GRENADE;
    }
	bool IsKnife()
    {
		return GetWeaponData()->WeaponType == WEAPONTYPE_KNIFE;
    }
	bool IsC4()
    {
		return GetWeaponData()->WeaponType == WEAPONTYPE_C4;
    }
	bool IsSniper()
    {
		return GetWeaponData()->WeaponType == WEAPONTYPE_SNIPER_RIFLE;
    }
};


class C_CSPlayer : public IClientEntity
{
    template<class T>
    inline T GetFieldValue(int offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
    }

    template<class T>
    T* GetFieldPointer(int offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<DWORD>(this) + offset);
    }
public:
    static C_CSPlayer* GetLocalPlayer()
    {
        return static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(Interfaces::Engine()->GetLocalPlayer()));
    }
    C_BaseCombatWeapon* GetActiveWeapon()
    {
        static int m_hActiveWeapon = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon"));
        return static_cast<C_BaseCombatWeapon*>(Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<IClientEntity>>(m_hActiveWeapon)));
    }
	float GetNextAttack()
	{
		static int m_flNextAttack = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseCombatCharacter"), XorStr("bcc_localdata"), XorStr("m_flNextAttack"));
		return GetFieldValue<float>(m_flNextAttack);
	}
    int GetHealth()
    {
        static int m_iHealth = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_iHealth"));
        return GetFieldValue<int>(m_iHealth);
    }
	int GetArmor()
	{
		static int m_ArmorValue = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_ArmorValue"));
		return GetFieldValue<int>(m_ArmorValue);
	}
	C_CSPlayer* GetObservingTarget()
    {
		if (this->IsAlive())
			return nullptr;
		
		static int m_hObserverTarget = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_hObserverTarget"));
		return static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CBaseHandle>(m_hObserverTarget)));
    }
	float* FlashMaxAlpha()
    {
		static int m_flFlashMaxAlpha = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_flFlashMaxAlpha"));
		return GetFieldPointer<float>(m_flFlashMaxAlpha);
    }
	bool IsScoped()
	{
		static int m_bIsScoped = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_bIsScoped"));
		return GetFieldValue<bool>(m_bIsScoped);
	}
	bool IsImmune()
	{
		static int m_bGunGameImmunity = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_bGunGameImmunity"));
		return GetFieldValue<bool>(m_bGunGameImmunity);
	}
    bool IsAlive()
    {
        static int m_lifeState = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_lifeState"));
        return GetFieldValue<int>(m_lifeState) == 0;
    }
    int GetTeamNum()
    {
        static int m_iTeamNum = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_iTeamNum"));
        return GetFieldValue<int>(m_iTeamNum);
    }
    int GetFlags()
    {
        static int m_fFlags = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_fFlags"));
        return GetFieldValue<int>(m_fFlags);
    }
    Vector GetViewOffset()
    {
        static int m_vecViewOffset = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]"));
        return GetFieldValue<Vector>(m_vecViewOffset);
    }
	Vector GetVelocity()
	{
		static int m_vecVelocity = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecVelocity[0]"));
		return GetFieldValue<Vector>(m_vecVelocity);
	}
    Vector GetOrigin()
    {
        static int m_vecOrigin = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_vecOrigin"));
        return GetFieldValue<Vector>(m_vecOrigin);
    }
    Vector GetEyePos()
    {
        return GetOrigin() + GetViewOffset();
    }
    Vector* ViewPunch()
    {
        static int m_viewPunchAngle = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle"));
        return GetFieldPointer<Vector>(m_viewPunchAngle);
    }
    Vector* AimPunch()
    {
        static int m_aimPunchAngle = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle"));
        return GetFieldPointer<Vector>(m_aimPunchAngle);
    }
	int GetShotsFired()
	{
		static int m_iShotsFired = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("cslocaldata"), XorStr("m_iShotsFired"));
		return GetFieldValue<int>(m_iShotsFired);
	}
	int GetTickBase()
	{
		static int m_nTickBase = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase"));
		return GetFieldValue<int>(m_nTickBase);
	}
	bool HasHelmet()
	{
		static int m_bHasHelmet = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_bHasHelmet"));
		return GetFieldValue<bool>(m_bHasHelmet);
	}
	int GetScoreboardKills()
	{
		static int m_iMatchStats_Kills = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iMatchStats_Kills"));
		return GetFieldValue<int>(m_iMatchStats_Kills);
	}
	int GetScoreboardDeaths()
	{
		static int m_iMatchStats_Deaths = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_iMatchStats_Deaths"));
		return GetFieldValue<int>(m_iMatchStats_Deaths);
	}
	float GetMaxSpeed()
	{
		static int m_flMaxspeed = NetvarManager::Instance()->GetOffset(XorStr("DT_BasePlayer"), XorStr("m_flMaxspeed"));
		return GetFieldValue<float>(m_flMaxspeed);
	}
	float GetLowerBodyYaw()
	{
		static int m_flLowerBodyYawTarget = NetvarManager::Instance()->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_flLowerBodyYawTarget"));
		return GetFieldValue<float>(m_flLowerBodyYawTarget);
	}
	float GetSimulationTime()
	{
		static int m_flSimulationTime = NetvarManager::Instance()->GetOffset(XorStr("DT_BaseEntity"), XorStr("m_flSimulationTime"));
		return GetFieldValue<float>(m_flSimulationTime);
	}
};
