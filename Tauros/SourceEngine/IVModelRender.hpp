#pragma once

#include "Vector.hpp"

typedef unsigned short ModelInstanceHandle_t;

struct model_t
{
	char name[255];
};

struct ModelRenderInfo_t
{
	Vector origin;
	QAngle angles;
	char pad0[0x04];
	void *pRenderable;
	const model_t *pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector *pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;
	ModelRenderInfo_t()
	{
		pModelToWorld = nullptr;
		pLightingOffset = nullptr;
		pLightingOrigin = nullptr;
	}
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

struct studiohwdata_t;
struct DrawModelState_t
{
	studiohdr_t*			m_pStudioHdr;
	studiohwdata_t*			m_pStudioHWData;
	IClientRenderable*		m_pRenderable;
	const matrix3x4_t		*m_pModelToWorld;
	StudioDecalHandle_t		m_decals;
	int						m_drawFlags;
	int						m_lod;
};

class IVModelRender
{
public:
	void *ForcedMaterialOverride(IMaterial* mat, OverrideType_t nOverrideType = OVERRIDE_NORMAL, int unk = 0)
	{
		typedef void*(__thiscall* OrigFn)(void*, IMaterial*, OverrideType_t, int);
		return CallVFunction<OrigFn>(this, 1)(this, mat, nOverrideType, unk);
	}
	void *DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld = nullptr)
	{
		typedef void*(__thiscall* OrigFn)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
		return CallVFunction<OrigFn>(this, 21)(this, ctx, state, pInfo, pCustomBoneToWorld);
	}
};
