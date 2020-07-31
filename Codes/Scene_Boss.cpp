#include "stdafx.h"
#include "..\Headers\Scene_Boss.h"
#include "Loading.h"
#include "Management.h"

USING(Client)

CScene_Boss::CScene_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Boss::Ready_Scene()
{
	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain()))
		return E_FAIL;

	return NOERROR;
}

_int CScene_Boss::Update_Scene(_double TimeDelta)
{
	return _int();
}

_int CScene_Boss::LateUpdate_Scene(_double TimeDelta)
{
	return _int();
}

HRESULT CScene_Boss::Render_Scene()
{
	return NOERROR;
}

HRESULT CScene_Boss::Ready_Layer_Terrain()
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
	lstrcpy(szMeshTag, L"Component_Mesh_Terrain_Boss");

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_BOSS, L"Layer_Terrain", L"GameObject_Terrain", szMeshTag)))
		return E_FAIL;

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Boss::Ready_Layer_Camera()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

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
	tCameraDesc.iSceneID = SCENE_BOSS;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_BOSS, L"Layer_Camera", L"GameObject_Camera_Free", &tCameraDesc)))
		return E_FAIL;

	pManagement->Play_Camera(SCENE_BOSS, L"Camera_Free");

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

CScene_Boss * CScene_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Boss* pInstance = new CScene_Boss(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(g_hWnd, L"CScene_Boss::Create() is Failed!", nullptr, MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Boss::Free()
{
	Safe_Release(m_pLoading);

	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return;
	Safe_AddRef(pManagement);

	pManagement->Clear_Scene(SCENE_BOSS);

	Safe_Release(pManagement);

	g_eScene = SCENE_END;

	CScene::Free();
}
