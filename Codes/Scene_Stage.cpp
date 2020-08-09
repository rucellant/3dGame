#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Player.h"
#include "Shield.h"
#include "Weapon.h"
#include "Terrain.h"
#include "Loading.h"
#include "SK_Slot.h"
#include "IOManager.h"
#include "Management.h"
#include "Scene_Boss.h"
#include "Camera_Free.h"
#include "Babegazi_Axe.h"
#include "Babegazi_Bow.h"
#include "Knole_Warrior.h"
#include "Camera_Player.h"
#include "Knole_Commander.h"
#include "Babegazi_Warrior.h"

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

	if (FAILED(Ready_Layer_Terrain()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	CIOManager::GetInstance()->Clear();

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

	CCamera::CAMERADESC tCameraDesc_Free;
	tCameraDesc_Free.vEye = _vec3(0.f, 0.f, -5.f);
	tCameraDesc_Free.vAt = _vec3(0.f, 0.f, 0.f);
	tCameraDesc_Free.vAxisY = _vec3(0.f, 1.f, 0.f);
	tCameraDesc_Free.fRotationPerSec = D3DXToRadian(90.f);
	tCameraDesc_Free.fSpeedPerSec = 60.f;
	tCameraDesc_Free.fFar = 500.f;
	tCameraDesc_Free.fNear = 0.2f;
	tCameraDesc_Free.fFovy = D3DXToRadian(60.f);
	tCameraDesc_Free.fAspect = g_iWinCX / _float(g_iWinCY);
	tCameraDesc_Free.iSceneID = SCENE_STAGE;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Camera", L"GameObject_Camera_Free", &tCameraDesc_Free)))
		return E_FAIL;

	// For. GameObject_Camera_Player
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Camera_Player", CCamera_Player::Create(pGraphic_Device))))
		return E_FAIL;

	CCamera::CAMERADESC tCameraDesc_Player;
	tCameraDesc_Player.vEye = _vec3(0.f, 0.f, -5.f);
	tCameraDesc_Player.vAt = _vec3(0.f, 0.f, 0.f);
	tCameraDesc_Player.vAxisY = _vec3(0.f, 1.f, 0.f);
	tCameraDesc_Player.fRotationPerSec = D3DXToRadian(90.f);
	tCameraDesc_Player.fSpeedPerSec = 60.f;
	tCameraDesc_Player.fFar = 500.f;
	tCameraDesc_Player.fNear = 0.2f;
	tCameraDesc_Player.fFovy = D3DXToRadian(60.f);
	tCameraDesc_Player.fAspect = g_iWinCX / _float(g_iWinCY);
	tCameraDesc_Player.iSceneID = SCENE_STAGE;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Camera", L"GameObject_Camera_Player", &tCameraDesc_Player)))
		return E_FAIL;

	// Play Camera
	pManagement->Play_Camera(SCENE_STAGE, L"Camera_Player");

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Player()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Player
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Player", CPlayer::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CPlayer::OBJDESC> vecLoad = *(vector<CPlayer::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_PLAYER);

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Player", L"GameObject_Player", &vecLoad[0])))
		return E_FAIL;

	// For. GameObject_Weapon
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Weapon", CWeapon::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Player", L"GameObject_Weapon")))
		return E_FAIL;

	// For. GameObject_Shield
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Shield", CShield::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Player", L"GameObject_Shield")))
		return E_FAIL;

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Monster()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Babegazi_Axe
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Babegazi_Axe", CBabegazi_Axe::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Babegazi_Bow
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Babegazi_Bow", CBabegazi_Bow::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Babegazi_Warrior
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Babegazi_Warrior", CBabegazi_Warrior::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Knole_Commander
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Knole_Commander", CKnole_Commander::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Knole_Warrior
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Knole_Warrior", CKnole_Warrior::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CMonster::OBJDESC> vecLoad = *(vector<CMonster::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_MONSTER);

	for (auto& element : vecLoad)
	{
		if (!lstrcmp(element.szFileName, L"Babegazi_Axe.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Babegazi_Axe", &element)))
				return E_FAIL;
		}
		if (!lstrcmp(element.szFileName, L"Babegazi_Bow.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Babegazi_Bow", &element)))
				return E_FAIL;
		}
		if (!lstrcmp(element.szFileName, L"Babegazi_Warrior.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Babegazi_Warrior", &element)))
				return E_FAIL;
		}
		if (!lstrcmp(element.szFileName, L"Knole_Commander.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Knole_Commander", &element)))
				return E_FAIL;
		}
		if (!lstrcmp(element.szFileName, L"Knole_Warrior.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Knole_Warrior", &element)))
				return E_FAIL;
		}
	}

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_UI()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_SK_Slot
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_SK_Slot", CSK_Slot::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_UI", L"GameObject_SK_Slot")))
		return E_FAIL;

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
