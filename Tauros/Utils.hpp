#pragma once

#include <Windows.h>
#include <Psapi.h>
#include "SourceEngine/SDK.hpp"
#include "CSGOStructs.hpp"
#include <d3d9.h>

extern HMODULE g_hLib;
extern HWND g_hWnd;

inline float DEG2RAD(float deg)
{
	return deg * 3.14159265358979323846f / 180.0f;
}

struct FontSize
{
	int width;
	int height;
};

class Utils
{
public:
	static std::string GetDllDir()
	{
		auto path = GetDllDirW();
		return std::string(path.begin(), path.end());
	}

	static std::wstring GetDllDirW()
	{
		wchar_t path[1024];
		GetModuleFileNameW(g_hLib, path, 1024);
		std::wstring str(path);
		const auto pos = str.rfind('\\');
		if (pos == std::string::npos)
			return nullptr;

		str.erase(pos + 1, str.length() - pos - 1);
		return str;
	}

	static std::string GetGameDir()
	{
		auto path = GetGameDirW();
		return std::string(path.begin(), path.end());
	}

	static std::wstring GetGameDirW()
	{
		wchar_t path[1024];
		GetModuleFileNameW(nullptr, path, 1024);
		std::wstring str(path);
		const auto pos = str.rfind('\\');
		if (pos == std::string::npos)
			return nullptr;

		str.erase(pos + 1, str.length() - pos - 1);
		return str;
	}

	static void FlashTaskbarIcon()
	{
		FLASHWINFO fi;
		fi.cbSize = sizeof(FLASHWINFO);
		fi.hwnd = g_hWnd;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx(&fi);
	}

	static Vector GetEntityBone(IClientEntity* pEntity, ECSPlayerBones Bone)
	{
		matrix3x4_t boneMatrix[128];
		if(!pEntity->SetupBones(boneMatrix, 128, 0x100, Interfaces::Engine()->GetLastTimeStamp()))
			return Vector();

		auto hitbox = boneMatrix[Bone];
		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}

	static bool ScreenTransform(const Vector& point, Vector& screen)
	{
		const auto& w2sMatrix = Interfaces::Engine()->WorldToScreenMatrix();
		screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
		screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
		screen.z = 0.0f;

		const auto w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];
		if(w < 0.001f)
		{
			screen.x *= 100000;
			screen.y *= 100000;
			return true;
        }

		const auto invw = 1.f / w;
		screen.x *= invw;
		screen.y *= invw;
		return false;
	}

	static bool WorldToScreen(const Vector &origin, Vector &screen)
	{
		if(!ScreenTransform(origin, screen)) {
			int iScreenWidth, iScreenHeight;
			Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);
			screen.x = iScreenWidth / 2.f + screen.x * iScreenWidth / 2;
			screen.y = iScreenHeight / 2.f - screen.y * iScreenHeight / 2;
			return true;
		}
		return false;
	}

	static float DistancePointToLine(Vector point, Vector lineOrigin, Vector direction)
	{
		auto pointDir = point - lineOrigin;
		const auto tempOffset = pointDir.Dot(direction) / (direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
		if (tempOffset < 0.000001f)
			return FLT_MAX;

		const auto perpendicularPoint = lineOrigin + direction * tempOffset;
		return (point - perpendicularPoint).Length();
	}

	static uint64_t FindSignature(const char* szModule, const char* szSignature)
	{
		//CREDITS: learn_more
		#define INRANGE(x,a,b)  (x >= a && x <= b) 
		#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
		#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
		const auto startAddress = reinterpret_cast<DWORD>(modInfo.lpBaseOfDll);
		const auto endAddress = startAddress + modInfo.SizeOfImage;
		auto pat = szSignature;
		DWORD firstMatch = 0;
		for(auto pCur = startAddress; pCur < endAddress; pCur++) {
			if(!*pat) return firstMatch;
			if(*(PBYTE)pat == '\?' || *reinterpret_cast<BYTE*>(pCur) == getByte(pat)) {
				if(!firstMatch) firstMatch = pCur;
				if(!pat[2]) return firstMatch;
				if(*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			} else {
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

    static bool Clamp(QAngle &angles)
    {
	    auto a = angles;
        Normalize(a);
        ClampAngles(a);

        if(isnan(a.x) || isinf(a.x) || isnan(a.y) || isinf(a.y) || isnan(a.z) || isinf(a.z))
            return false;
		
    	angles = a;
        return true;
    }

	static void AngleVectors(const Vector &angles, Vector *forward)
	{
		const auto cp = cos(DEG2RAD(angles[0]));
		forward->x = cp * cos(DEG2RAD(angles[1]));
		forward->y = cp * sin(DEG2RAD(angles[1]));
		forward->z = -sin(DEG2RAD(angles[0]));
	}

	static QAngle CalcAngle(Vector src, Vector dst)
	{
		QAngle angles;
		auto delta = src - dst;
		VectorAngles(delta, angles);
		Clamp(delta);
		return angles;
	}

	static void VectorAngles(const Vector& forward, QAngle &angles)
	{
		if (forward[1] == 0.0f && forward[0] == 0.0f)
		{
			angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f; // Pitch (up/down)
			angles[1] = 0.0f;                                 // Yaw left/right
		}
		else
		{
			angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / 3.14159265358979323846f;
			angles[1] = atan2(forward[1], forward[0]) * 180 / 3.14159265358979323846f;

			if (angles[1] > 90) angles[1] -= 180;
			else if (angles[1] < 90) angles[1] += 180;
			else if (angles[1] == 90) angles[1] = 0;
		}
		angles[2] = 0.0f;
	}

	static float GetTraceFractionWorldProps(Vector start, Vector end)
	{
		Ray_t ray;
		CGameTrace tr;
		CTraceFilterWorldAndPropsOnly filter;
		ray.Init(start, end);
		Interfaces::EngineTrace()->TraceRay(ray, MASK_SOLID, &filter, &tr);
		return tr.fraction;
	}

	static Vector Direction(Vector vector)
	{
		return Vector(cos(DEG2RAD(vector.y)) * cos(DEG2RAD(vector.x)), sin(DEG2RAD(vector.y)) * cos(DEG2RAD(vector.x)), sin(DEG2RAD(-vector.x))).Normalized();
	}
private:
    static void Normalize(QAngle& angle)
    {
        while(angle.x > 89.0f) {
            angle.x -= 180.f;
        }
        while(angle.x < -89.0f) {
            angle.x += 180.f;
        }
        while(angle.y > 180.f) {
            angle.y -= 360.f;
        }
        while(angle.y < -180.f) {
            angle.y += 360.f;
        }
    }

    static void ClampAngles(QAngle &angles)
    {
        if(angles.y > 180.0f)
            angles.y = 180.0f;
        else if(angles.y < -180.0f)
            angles.y = -180.0f;

        if(angles.x > 89.0f)
            angles.x = 89.0f;
        else if(angles.x < -89.0f)
            angles.x = -89.0f;

        angles.z = 0;
    }
};