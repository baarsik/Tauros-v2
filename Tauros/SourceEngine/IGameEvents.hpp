#pragma once

#define MAX_EVENT_NAME_LENGTH	32		// max game event name length
#define MAX_EVENT_BITS			9		// max bits needed for an event index
#define MAX_EVENT_NUMBER		(1<<MAX_EVENT_BITS)		// max number of events allowed
#define MAX_EVENT_BYTES			1024	// max size in bytes for a serialized event

class IGameEvent
{
public:
	virtual ~IGameEvent() {};
	virtual const char* GetName() const = 0;

	virtual bool  IsReliable() const = 0; // if event handled reliable
	virtual bool  IsLocal() const = 0; // if event is never networked
	virtual bool  IsEmpty(const char* keyName = nullptr) = 0; // check if data field exists

	virtual bool  GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
	virtual int   GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
	virtual unsigned long long GetUint64(char const* keyName = nullptr, unsigned long long defaultValue = 0) = 0;
	virtual float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;

	virtual const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;
	virtual const wchar_t* GetWString(char const* keyName = nullptr, const wchar_t* defaultValue = L"") = 0;
	virtual const void* GetPtr(const char* keyname = nullptr, const void* default_values = nullptr) = 0;

	virtual void SetBool(const char* keyName, bool value) = 0;
	virtual void SetInt(const char* keyName, int value) = 0;
	virtual void SetUInt64(const char* keyName, unsigned long long value) = 0;
	virtual void SetFloat(const char* keyName, float value) = 0;
	virtual void SetString(const char* keyName, const char* value) = 0;
	virtual void SetWString(const char* keyName, const wchar_t* value) = 0;
	virtual void SetPtr(const char* keyname, const void* value) = 0;
};


class IGameEventListener2
{
public:
	virtual	~IGameEventListener2(void) {};
	virtual void FireGameEvent(IGameEvent* event) = 0;
	virtual int GetEventDebugID(void) = 0;
};

class IGameEventManager2
{
public:
	virtual	~IGameEventManager2(void) {};
	virtual int LoadEventsFromFile(const char* filename) = 0;
	virtual void Reset() = 0;
	virtual bool AddListener(IGameEventListener2* listener, const char* name, bool bServerSide) = 0;
	virtual bool FindListener(IGameEventListener2* listener, const char* name) = 0;
	virtual void RemoveListener(IGameEventListener2* listener) = 0;
	virtual void AddListenerGlobal(IGameEventListener2* listener, bool bIsServerSide) = 0;
	virtual IGameEvent* CreateEvent(const char* name, bool bForce = false) = 0;
	virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
	virtual bool FireEventClientSide(IGameEvent* event) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
	virtual void FreeEvent(IGameEvent* event) = 0;
	virtual bool SerializeEvent(IGameEvent* event, bf_write* buf) = 0;
	virtual IGameEvent* UnserializeEvent(bf_read* buf) = 0; // create new KeyValues, must be deleted
};