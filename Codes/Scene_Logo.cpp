#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"
#include "Loading.h"
#include "Management.h"
#include "Scene_Stage.h"
#include "Loading_Icon.h"
#include "Loading_Background.h"

USING(Client)

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_GameObject_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_Loading")))
		return E_FAIL;

	m_pLoading = CLoading::Create(Get_Graphic_Device(), CManagement::GetInstance(), SCENE_STAGE);
	if (m_pLoading == nullptr)
		return E_FAIL;

	return NOERROR;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	return _int();
}

_int CScene_Logo::LateUpdate_Scene(_double TimeDelta)
{
	if ((GetKeyState(VK_SPACE) & 0x8000) && (m_pLoading->Get_Complete() == 1))
	{
		CManagement* pManagement = CManagement::GetInstance();
		if (pManagement == nullptr)
			return -1;

		Safe_AddRef(pManagement);

		g_eScene = SCENE_STAGE;

		CScene*	pScene = CScene_Stage::Create(Get_Graphic_Device());
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

HRESULT CScene_Logo::Render_Scene()
{
	return NOERROR;
}

HRESULT CScene_Logo::Ready_GameObject_Prototype()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	// For. GameObject_Loading_Background
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Loading_Background", CLoading_Background::Create(pManagement->Get_Graphic_Device()))))
		return E_FAIL;

	// For. GameObject_Loading_Icon
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Loading_Icon", CLoading_Icon::Create(pManagement->Get_Graphic_Device()))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_GameObject_Clone(g_eScene, pLayerTag, L"GameObject_Loading_Background")))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(g_eScene, pLayerTag, L"GameObject_Loading_Icon")))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(g_hWnd, L"CScene_Logo::Create() is Failed!", nullptr, MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	Safe_Release(m_pLoading);

	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return;
	Safe_AddRef(pManagement);

	pManagement->Clear_Scene(SCENE_LOGO);

	Safe_Release(pManagement);

	CScene::Free();
}
