#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Terrain.h"
#include "Loading.h"
#include "IOManager.h"
#include "Management.h"
#include "Scene_Boss.h"

USING(Client)

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Stage::Ready_Scene()
{
	m_pLoading = CLoading::Create(Get_Graphic_Device(), CManagement::GetInstance(), SCENE_BOSS);
	if (m_pLoading == nullptr)
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain()))
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

		g_eScene = SCENE_BOSS;

		pManagement->Clear_Scene(SCENE_STAGE);

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

HRESULT CScene_Stage::Ready_Layer_Terrain()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	_tchar szMeshTag[MAX_STR];
	lstrcpy(szMeshTag, L"Component_Mesh_Terrain_Stage");

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Terrain", L"GameObject_Terrain", szMeshTag)))
		return E_FAIL;

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Camera()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Camera_Free
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Camera_Free", CCamera_Free::Create(pGraphic_Device))))
		return E_FAIL;

	CCamera::CAMERADESC tCameraDesc;
	tCameraDesc.vEye = _vec3(0.f, 0.f, -5.f);
	tCameraDesc.vAt = _vec3(0.f, 0.f, 0.f);
	tCameraDesc.vAxisY = _vec3(0.f, 1.f, 0.f);
	tCameraDesc.fRotationPerSec = D3DXToRadian(90.f);
	tCameraDesc.fSpeedPerSec = 60.f;
	tCameraDesc.fFar = 500.f;
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFovy = D3DXToRadian(60.f);
	tCameraDesc.fAspect = g_iWinCX / _float(g_iWinCY);

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Camera", L"GameObject_Camera_Free", &tCameraDesc)))
		return E_FAIL;

	pManagement->Play_Camera(L"Camera_Free");

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

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

	CScene::Free();
}
