#pragma once

class NoFlash
{
public:
	void FrameStageNotify_Pre(ClientFrameStage_t stage) const
	{
		if (!Interfaces::Engine()->IsInGame())
			return;

		if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
			return;

		auto pLocal = C_CSPlayer::GetLocalPlayer();
		if (!pLocal)
			return;

		if (Options::g_bNoFlashEnabled)
			*pLocal->FlashMaxAlpha() = Options::g_fNoFlashAmount * 2.55f;
		else
			*pLocal->FlashMaxAlpha() = 255.0f;
	}
};
class NoSmoke
{
public:
	NoSmoke()
	{
		m_vecSmokeMaterials = {
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust"
		};
	}

	void RenderSmokeOverlay_Pre(bool unk) const
	{
		if (!Options::g_bNoSmokeEnabled)
			Hooks::g_fnOriginalRenderSmokeOverlay(Interfaces::ViewRender(), unk);
	}

	void FrameStageNotify_Pre(ClientFrameStage_t stage) const
	{
		if (stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
			return;

		for (auto materialName : m_vecSmokeMaterials)
		{
			auto mat = Interfaces::MaterialSystem()->FindMaterial(materialName, XorStr(TEXTURE_GROUP_OTHER));
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Options::g_bNoSmokeEnabled);
		}
	}
private:
	std::vector<const char*> m_vecSmokeMaterials;
};

class Hands
{
public:
	void DrawModelExecute_Pre(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld) const
	{
		if (Options::g_iHandsDisplay == 0 || !pInfo.pModel)
			return;
		
		if (strstr(pInfo.pModel->name, XorStr("arms")) == nullptr)
			return;

		auto hands = Interfaces::MaterialSystem()->FindMaterial(pInfo.pModel->name, XorStr(TEXTURE_GROUP_MODEL));
		hands->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Options::g_iHandsDisplay == 1);
		hands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Options::g_iHandsDisplay == 2);
		Interfaces::ModelRender()->ForcedMaterialOverride(hands);
	}
};

class GrayWalls
{
public:
	void OverrideConfig(MaterialSystem_Config_t* config, bool forceUpdate) const
	{
		config->m_bDrawGray = Options::g_bGrayWallsEnabled;
	}
};
