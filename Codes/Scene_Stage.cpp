#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Loading.h"
#include "Management.h"
#include "Scene_Boss.h"

USING(Client)

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	m_pLoading = CLoading::Create(Get_Graphic_Device(), SCENE_BOSS);
	if (m_pLoading == nullptr)
		return E_FAIL;

	return NOERROR;
}

_int CScene_Stage::Update_Scene(_double TimeDelta)
{
	return _int();
}

_int CScene_Stage::LateUpdate_Scene(_double TimeDelta)
{
	if ((GetKeyState(VK_SPACE) & 0x8000) && (m_pLoading->Get_Complete() == 1))
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (pManagement == nullptr)
			return -1;

		Safe_AddRef(pManagement);

		CScene*	pScene = CScene_Boss::Create(Get_Graphic_Device());
		if (pScene == nullptr)
			return -1;

		if (FAILED(pManagement->Ready_Current_Scene(pScene)))
			return -1;

		Safe_Release(pScene);

		Safe_Release(pManagement);

		return 0;
	}

	return _int();
}

HRESULT CScene_Stage::Render_Scene()
{
	return NOERROR;
}

CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Stage* pInstance = new CScene_Stage(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(g_hWnd, L"CScene_Stage::Create() is Failed!", nullptr, MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage::Free()
{
	Safe_Release(m_pLoading);

	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return;
	Safe_AddRef(pManagement);

	pManagement->Clear_Scene(SCENE_STAGE);

	Safe_Release(pManagement);

	g_eScene = SCENE_BOSS;

	CScene::Free();
}
