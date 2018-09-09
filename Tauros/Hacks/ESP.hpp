#pragma once
#include "CSGOStructs.hpp"
#include "Options.hpp"
#include "Utils.hpp"

class ESP
{
public:
	ESP()
	{
		m_pDrawManager = Container::Instance().Resolve<DrawManager>();
		m_pGUI = Container::Instance().Resolve<GUI>();
		m_pSignatureHelper = Container::Instance().Resolve<SignatureHelper>();
	}

	void EndScene_Pre(IDirect3DDevice9* pDevice)
	{
		if (!Options::g_bESPEnabled || !Interfaces::Engine()->IsInGame())
			return;

		const auto pLocal = C_CSPlayer::GetLocalPlayer();
		PlayerESP(pDevice, pLocal);
	}

private:
	std::shared_ptr<DrawManager> m_pDrawManager;
	std::shared_ptr<GUI> m_pGUI;
	std::shared_ptr<SignatureHelper> m_pSignatureHelper;
	matrix3x4_t m_pBoneToWorldOut[128];

	void PlayerESP(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal)
	{
		if (!pLocal)
			return;

		for (auto i = 1; i < Interfaces::Engine()->GetMaxClients(); i++)
		{
			if (i == Interfaces::Engine()->GetLocalPlayer())
				continue;

			auto pTarget = static_cast<C_CSPlayer*>(Interfaces::EntityList()->GetClientEntity(i));
			if (!pTarget)
				continue;

			if (pTarget->GetClientClass()->m_ClassID == EClassIds::CCSPlayer)
			{
				if (!pTarget->IsAlive() || pTarget->IsDormant() || pLocal->GetObservingTarget() == pTarget)
					continue;

				const auto vOrigin = pTarget->GetOrigin();
				auto vHead = Utils::GetEntityBone(pTarget, ECSPlayerBones::HEAD_0);
				vHead.z += 15.0f;

				Vector vScreenOrigin, vScreenHead;
				if (!m_pSignatureHelper->WorldToScreen(vHead, vScreenHead))
					continue;

				if (!m_pSignatureHelper->WorldToScreen(vOrigin, vScreenOrigin))
					continue;

				RenderHealth(pDevice, pLocal, pTarget, vScreenHead, vScreenOrigin);
				RenderBoxes(pDevice, pLocal, pTarget, vScreenHead, vScreenOrigin);
				RenderBones(pDevice, pLocal, pTarget);
				RenderTop(pDevice, pLocal, pTarget, vScreenHead);
				RenderBottom(pDevice, pLocal, pTarget, vScreenOrigin);
			}
		}
	}

	void RenderHealth(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal, C_CSPlayer* pTarget, Vector vScreenHead, Vector vScreenOrigin) const
	{
		if (Options::g_iESPShowHealth == 0)
			return;

		const auto isEnemy = pLocal->GetTeamNum() != pTarget->GetTeamNum() || Options::g_bDeathmatch;
		if (Options::g_iESPShowHealth == (isEnemy ? 1 : 2))
			return;

		const auto width = abs(vScreenHead.y - vScreenOrigin.y) * 0.65f;
		const auto height = 4 * m_pGUI->GetScale();
		const auto x = vScreenHead.x - width / 2.f;
		const auto y = vScreenHead.y - height * 2.f;

		const auto healthNormalized = pTarget->GetHealth() >= 100 ? 1.0f : pTarget->GetHealth() / 100.0f;

		const auto foreground = isEnemy
			? ImColor(Options::g_fESPEnemyColor[0], Options::g_fESPEnemyColor[1], Options::g_fESPEnemyColor[2])
			: ImColor(Options::g_fESPAllyColor[0], Options::g_fESPAllyColor[1], Options::g_fESPAllyColor[2]);
		const auto background = isEnemy // foreground / 2.5f
			? ImColor(Options::g_fESPEnemyColor[0] * 0.4f, Options::g_fESPEnemyColor[1] * 0.4f, Options::g_fESPEnemyColor[2] * 0.4f)
			: ImColor(Options::g_fESPAllyColor[0] * 0.4f, Options::g_fESPAllyColor[1] * 0.4f, Options::g_fESPAllyColor[2] * 0.4f);

		m_pDrawManager->AddRectFilled(ImVec2(x - 2, y - 1), ImVec2(x + width + 2, y + height + 1), ImColor(0.f, 0.f, 0.f, 0.5f));
		m_pDrawManager->AddRectFilled(ImVec2(x - 1, y), ImVec2(x + width + 1, y + height), background);
		m_pDrawManager->AddRectFilled(ImVec2(x - 1, y), ImVec2(x + width * healthNormalized + 1, y + height), foreground);
	}

	void RenderBoxes(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal, C_CSPlayer* pTarget, Vector vScreenHead, Vector vScreenOrigin) const
	{
		if (Options::g_iESPShowBoxes == 0)
			return;

		const auto isEnemy = pLocal->GetTeamNum() != pTarget->GetTeamNum() || Options::g_bDeathmatch;
		if (Options::g_iESPShowBoxes == (isEnemy ? 1 : 2))
			return;

		const auto height = abs(vScreenHead.y - vScreenOrigin.y);
		const auto width = height * 0.65f;
		const auto lineColor = isEnemy
			? ImColor(Options::g_fESPEnemyColor[0], Options::g_fESPEnemyColor[1], Options::g_fESPEnemyColor[2])
			: ImColor(Options::g_fESPAllyColor[0], Options::g_fESPAllyColor[1], Options::g_fESPAllyColor[2]);

		DrawOutlinedRect(vScreenHead.x - width / 2.f, vScreenHead.y, width, height, lineColor);
	}

	void RenderTop(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal, C_CSPlayer* pTarget, Vector vScreenHead) const
	{
		if (Options::g_iESPShowNames == 0)
			return;

		const auto isEnemy = pLocal->GetTeamNum() != pTarget->GetTeamNum() || Options::g_bDeathmatch;
		if (Options::g_iESPShowNames == (isEnemy ? 1 : 2))
			return;

		int y;
		if (Options::g_iESPShowHealth == 0 || Options::g_iESPShowHealth == (isEnemy ? 1 : 2))
			y = int(vScreenHead.y);
		else 
			y = int(vScreenHead.y - 4 * 2 * m_pGUI->GetScale()); // 4 -> healthBox height

		player_info_t pInfo;
		Interfaces::Engine()->GetPlayerInfo(pTarget->EntIndex(), &pInfo);
		DrawString(int(vScreenHead.x), y, 255, 255, 255, 255, true, pInfo.szName, true);
	}
	
	void RenderBottom(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal, C_CSPlayer* pTarget, Vector vScreenOrigin) const
	{
		if (Options::g_iESPShowDistance == 0 && Options::g_iESPShowWeapon == 0)
			return;

		const auto isEnemy = pLocal->GetTeamNum() != pTarget->GetTeamNum() || Options::g_bDeathmatch;
		const auto shouldDrawDistance = Options::g_iESPShowDistance == 3 || Options::g_iESPShowDistance == (isEnemy ? 2 : 1);
		const auto shouldDrawWeapon = Options::g_iESPShowWeapon == 3 || Options::g_iESPShowWeapon == (isEnemy ? 2 : 1);
		if (!shouldDrawDistance && !shouldDrawWeapon)
			return;

		const auto vOrigin = pTarget->GetOrigin();
		const auto distance = pLocal->GetOrigin().DistTo(vOrigin);
		auto pWeapon = pTarget->GetActiveWeapon();
		char szText[36] = "";
		if (pWeapon && shouldDrawWeapon)
		{
			sprintf(szText, "%s", pWeapon->GetReadableName().c_str());
			if (shouldDrawDistance)
				sprintf(szText, "%s (%.0fm)", szText, ceil(distance * 0.0254f));
		}
		else if (shouldDrawDistance)
			sprintf(szText, "%s%.0fm", szText, ceil(distance * 0.0254f));
		DrawString(int(vScreenOrigin.x), int(vScreenOrigin.y), 255, 255, 255, 255, true, szText);
	}

	void RenderBones(IDirect3DDevice9* pDevice, C_CSPlayer* pLocal, C_CSPlayer* pTarget)
	{
		if (Options::g_iESPShowBones == 0)
			return;

		const auto isEnemy = pLocal->GetTeamNum() != pTarget->GetTeamNum() || Options::g_bDeathmatch;
		if (Options::g_iESPShowBones == (isEnemy ? 1 : 2))
			return;

		const auto pStudioModel = Interfaces::ModelInfo()->GetStudioModel(pTarget->GetModel());
		if (!pStudioModel)
			return;
		
		if (pTarget->SetupBones(m_pBoneToWorldOut, 128, 256, Interfaces::Engine()->GetLastTimeStamp()))
		{
			for (auto i = 0; i < pStudioModel->numbones; i++)
			{
				const auto pBone = pStudioModel->pBone(i);
				if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
					continue;

				Vector vBonePos1;
				auto hitbox = m_pBoneToWorldOut[i];
				if (!m_pSignatureHelper->WorldToScreen(Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]), vBonePos1))
					continue;

				Vector vBonePos2;
				hitbox = m_pBoneToWorldOut[pBone->parent];
				if (!m_pSignatureHelper->WorldToScreen(Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]), vBonePos2))
					continue;

				const auto color = isEnemy
					? ImColor(Options::g_fESPEnemyColor[0], Options::g_fESPEnemyColor[1], Options::g_fESPEnemyColor[2])
					: ImColor(Options::g_fESPAllyColor[0], Options::g_fESPAllyColor[1], Options::g_fESPAllyColor[2]);
				m_pDrawManager->AddLine(ImVec2(vBonePos1.x, vBonePos1.y), ImVec2(vBonePos2.x, vBonePos2.y), color);
			}
		}
	}

	// Helpers
	void DrawOutlinedRect(float x, float y, float width, float height, ImColor color) const
	{
		const auto x2 = x + width;
		const auto y2 = y + height;
		const auto scale = static_cast<int>(round(m_pGUI->GetScale()));

		for (auto i = 0; i <= scale - 1; i++)
		{
			m_pDrawManager->AddRect(ImVec2(x - i, y - i), ImVec2(x2 + i, y2 + i), color);
			m_pDrawManager->AddRect(ImVec2(x - scale - i, y - scale - i), ImVec2(x2 + scale + i, y2 + scale + i), ImColor(0.f, 0.f, 0.f, 0.5f));
			m_pDrawManager->AddRect(ImVec2(x + 1 + i, y + 1 + i), ImVec2(x2 - 1 - i, y2 - 1 - i), ImColor(0.f, 0.f, 0.f, 0.5f));
		}
	}

	ImVec2 DrawString(int x, int y, int r, int g, int b, int a, bool bCenter, const char *pszText, bool bVerticalAlignTop = false) const
	{
		if (pszText == nullptr)
			return ImVec2(-1, -1);

		const auto textSize = ImGui::CalcTextSize(pszText);
		m_pDrawManager->AddText(ImVec2(x - (bCenter ? textSize.x / 2.f : 0), y - (bVerticalAlignTop ? textSize.y : 0)), ImColor(1.f, 1.f, 1.f), text_flags::outline, pszText);
		return textSize;
	}
};
