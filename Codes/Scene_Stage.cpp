#include "stdafx.h"
#include "..\Headers\Scene_Stage.h"
#include "Gate.h"
#include "HpBar.h"
#include "MpBar.h"
#include "Torch.h"
#include "Portal.h"
#include "Player.h"
#include "Icicle.h"
#include "Shield.h"
#include "Weapon.h"
#include "Balrog.h"
#include "Twister.h"
#include "Soldier.h"
#include "Terrain.h"
#include "Crystal.h"
#include "Loading.h"
#include "SK_Slot.h"
#include "Skeleton.h"
#include "IOManager.h"
#include "HpBar_Boss.h"
#include "Management.h"
#include "Scene_Boss.h"
#include "Effect_Hit.h"
#include "Torch_Fire.h"
#include "Effect_Buff.h"
#include "Camera_Free.h"
#include "Door_Trigger.h"
#include "CollisionMgr.h"
#include "Camera_Event.h"
#include "Particle_Dead.h"
#include "Camera_Player.h"
#include "HpBar_Monster.h"
#include "Particle_Buff.h"
#include "Effect_Tornado.h"
#include "Particle_Spread.h"
#include "MidBoss_Trigger.h"
#include "Effect_Shoulder.h"
#include "StageEnd_Trigger.h"
#include "Effect_Earthquake.h"
#include "Particle_Cohesion.h"

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

	if (FAILED(Ready_Layer_Trigger()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Interact()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Static()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect()))
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
	if (m_bNextStage/*(GetKeyState(VK_SPACE) & 0x8000)*/ && (m_pLoading->Get_Complete() == 1))
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

	// юс╫ц╥н ╨Ш
	D3DLIGHT9 LightDesc;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	//LightDesc.Position = _vec3(0.f, 0.f, 0.f);
	//LightDesc.Position = _vec3(0.f, 400.f, 0.f);
	LightDesc.Position = _vec3(175.f, 128.f, -121.f);
	LightDesc.Range = 1000.0f;

	if (FAILED(pManagement->Add_Light(pGraphic_Device, &LightDesc, SCENE_STATIC, L"Component_Transform", CLight::LIGHT_MAIN)))
		return E_FAIL;
	//

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

	// For. GameObject_Camera_Event
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Camera_Event", CCamera_Event::Create(pGraphic_Device))))
		return E_FAIL;

	CCamera::CAMERADESC tCameraDesc_Event;
	tCameraDesc_Event.vEye = _vec3(0.f, 0.f, -5.f);
	tCameraDesc_Event.vAt = _vec3(0.f, 0.f, 0.f);
	tCameraDesc_Event.vAxisY = _vec3(0.f, 1.f, 0.f);
	tCameraDesc_Event.fRotationPerSec = D3DXToRadian(90.f);
	tCameraDesc_Event.fSpeedPerSec = 60.f;
	tCameraDesc_Event.fFar = 500.f;
	tCameraDesc_Event.fNear = 0.2f;
	tCameraDesc_Event.fFovy = D3DXToRadian(60.f);
	tCameraDesc_Event.fAspect = g_iWinCX / _float(g_iWinCY);
	tCameraDesc_Event.iSceneID = SCENE_STAGE;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Camera", L"GameObject_Camera_Event", &tCameraDesc_Event)))
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
	CPlayer::PLAYERINFO tPlayerInfo;
	tPlayerInfo.iCurHp = 1000;
	tPlayerInfo.iMaxHp = 1000;
	tPlayerInfo.iCurMp = 100;
	tPlayerInfo.iMaxMp = 100;
	tPlayerInfo.iCurDef = 100;
	tPlayerInfo.iDefaultDef = 100;
	tPlayerInfo.iGold = 0;
	tPlayerInfo.iMaxDmg = 200;
	tPlayerInfo.iMinDmg = 150;
	vecLoad[0].tPlayerInfo = tPlayerInfo;

	//vecLoad[0].tPlayerInfo

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

	// For. GameObject_Skeleton
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Skeleton", CSkeleton::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Soldier
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Soldier", CSoldier::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CMonster::OBJDESC> vecLoad = *(vector<CMonster::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_MONSTER);

	for (auto& element : vecLoad)
	{
		if (!lstrcmp(element.szFileName, L"Skeleton_00.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Skeleton", &element)))
				return E_FAIL;
		}
		if (!lstrcmp(element.szFileName, L"Soldier_00.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Soldier", &element)))
				return E_FAIL;
		}
	}

	// For. GameObject_Balrog
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Balrog", CBalrog::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CBalrog::OBJDESC> vecLoad2 = *(vector<CBalrog::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_BALROG);

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Monster", L"GameObject_Balrog", &vecLoad2[0])))
		return E_FAIL;
	

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

	// For. GameObject_HpBar
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_HpBar", CHpBar::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_UI", L"GameObject_HpBar")))
		return E_FAIL;

	// For. GameObject_MpBar
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_MpBar", CMpBar::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_UI", L"GameObject_MpBar")))
		return E_FAIL;

	// For. GameObject_HpBar_Monster
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_HpBar_Monster", CHpBar_Monster::Create(pGraphic_Device))))
		return E_FAIL;

	list<CGameObject*>* pMonsterLayer = pManagement->Get_Layer(g_eScene, L"Layer_Monster");

	for (auto& pMonster : *pMonsterLayer)
	{
		if (((CMonster*)pMonster)->GetType() == CMonster::TYPE_SKELETON || ((CMonster*)pMonster)->GetType() == CMonster::TYPE_SOLDIER)
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_UI", L"GameObject_HpBar_Monster", pMonster)))
				return E_FAIL;
		}
		
	}

	// For. GameObject_HpBar_Boss
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_HpBar_Boss", CHpBar_Boss::Create(pGraphic_Device))))
		return E_FAIL;

	{
		list<CGameObject*>* pMonsterLayer = pManagement->Get_Layer(g_eScene, L"Layer_Monster");

		for (auto& pMonster : *pMonsterLayer)
		{
			if (((CMonster*)pMonster)->GetType() == CMonster::TYPE_BALROG)
			{
				if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_UI", L"GameObject_HpBar_Boss", pMonster)))
					return E_FAIL;
			}

		}
	}
	

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Interact()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Portal
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Portal", CPortal::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Interact", L"GameObject_Portal")))
		return E_FAIL;

	// For. GameObject_Crystal
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Crystal", CCrystal::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CCrystal::OBJDESC> vecLoad = *(vector<CCrystal::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_INTERACT);

	for (auto& element : vecLoad)
	{
		if (!lstrcmp(element.szFileName, L"Cristal_1.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Interact", L"GameObject_Crystal", &element)))
				return E_FAIL;
		}
	}

	// For. GameObject_Icicle
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Icicle", CIcicle::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CIcicle::OBJDESC> vecLoad2 = *(vector<CIcicle::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_ICICLE);

	for (auto& element : vecLoad2)
	{
		if (!lstrcmp(element.szFileName, L"Ice_Missile_2.X"))
		{
			if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Interact", L"GameObject_Icicle", &element)))
				return E_FAIL;
		}
	}

	// For. GameObject_Twister
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Twister", CTwister::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CTwister::OBJDESC> vecLoad3 = *(vector<CTwister::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_TWISTER);

	for (auto& element : vecLoad3)
	{
		if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Interact", L"GameObject_Twister", &element)))
			return E_FAIL;
	}

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Static()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Gate
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Gate", CGate::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Static", L"GameObject_Gate")))
		return E_FAIL;

	// For. GameObject_Torch
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Torch", CTorch::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Torch_Fire
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Torch_Fire", CTorch_Fire::Create(pGraphic_Device))))
		return E_FAIL;

	vector<CTorch::OBJDESC> vecLoad = *(vector<CTorch::OBJDESC>*)CIOManager::GetInstance()->Load(CIOManager::TYPE_TORCH);

	for (auto& element : vecLoad)
	{
		if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Torch", L"GameObject_Torch", &element)))
			return E_FAIL;

		_vec3 vLook = *(_vec3*)&element.matWorld.m[2];
		D3DXVec3Normalize(&vLook, &vLook);

		_vec3 vPosition = *(_vec3*)&element.matWorld.m[3];

		vPosition.y += 4.f;

		if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Torch", L"GameObject_Torch_Fire", &vPosition)))
			return E_FAIL;

		D3DLIGHT9 LightDesc;

		LightDesc.Type = D3DLIGHT_POINT;
		LightDesc.Diffuse = element.vColor;
		LightDesc.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
		LightDesc.Specular = LightDesc.Diffuse;
		LightDesc.Position = vPosition + _vec3(0.f, -2.f, 0.f);
		LightDesc.Range = 50.f;

		if (FAILED(pManagement->Add_Light(pGraphic_Device, &LightDesc, SCENE_STATIC, L"Component_Transform", CLight::LIGHT_POINT)))
			return E_FAIL;
	}

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Trigger()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Door_Trigger
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Door_Trigger", CDoor_Trigger::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Trigger", L"GameObject_Door_Trigger")))
		return E_FAIL;

	// For. GameObject_MidBoss_Trigger
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_MidBoss_Trigger", CMidBoss_Trigger::Create(pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Trigger", L"GameObject_MidBoss_Trigger")))
		return E_FAIL;

	// For. GameObject_StageEnd_Trigger
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_StageEnd_Trigger", CStageEnd_Trigger::Create(pGraphic_Device))))
		return E_FAIL;

	/*if (FAILED(pManagement->Add_GameObject_Clone(SCENE_STAGE, L"Layer_Trigger", L"GameObject_StageEnd_Trigger", &m_bNextStage)))
		return E_FAIL;*/

	Safe_Release(pManagement);
	Safe_Release(pGraphic_Device);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Layer_Effect()
{
	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return E_FAIL;
	Safe_AddRef(pManagement);

	LPDIRECT3DDEVICE9 pGraphic_Device = pManagement->Get_Graphic_Device();
	if (pGraphic_Device == nullptr)
		return E_FAIL;
	Safe_AddRef(pGraphic_Device);

	// For. GameObject_Effect_Tornado
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Effect_Tornado", CEffect_Tornado::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Effect_Hit
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Effect_Hit", CEffect_Hit::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Effect_Shoulder
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Effect_Shoulder", CEffect_Shoulder::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Effect_Earthquake
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Effect_Earthquake", CEffect_Earthquake::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Effect_Buff
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Effect_Buff", CEffect_Buff::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Particle_Dead
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Particle_Dead", CParticle_Dead::Create(pGraphic_Device))))
		return E_FAIL;

	// For. GameObject_Particle_Cohesion
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Particle_Cohesion", CParticle_Cohesion::Create(pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Particle_Spread
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Particle_Spread", CParticle_Spread::Create(pGraphic_Device))))
		return E_FAIL;

	// For.GameObject_Particle_Buff
	if (FAILED(pManagement->Add_GameObject_Prototype(L"GameObject_Particle_Buff", CParticle_Buff::Create(pGraphic_Device))))
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
