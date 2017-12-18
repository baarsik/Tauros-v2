#pragma once

#include "Definitions.hpp"

class IClientNetworkable;
class IClientEntity;

class IClientEntityList
{
public:
	virtual IClientNetworkable*   GetClientNetworkable(int entindex) = 0;
	virtual IClientNetworkable*   GetClientNetworkableFromHandle(CBaseHandle handle) = 0;
	virtual IClientUnknown*       GetClientUnknownFromHandle(CBaseHandle handle) = 0;
	virtual IClientEntity*        GetClientEntity(int entindex) = 0;
	virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int                   GetHighestEntityIndex(void) = 0;
	virtual void                  SetMaxEntities(int maxEnts) = 0;
	virtual int                   GetMaxEntities() = 0;
};