#include "stdafx.h"
#include "..\Headers\MidBoss_Trigger.h"
#include "Balrog.h"
#include "Player.h"
#include "Monster.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Camera_Event.h"

USING(Client)

CMidBoss_Trigger::CMidBoss_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMidBoss_Trigger::CMidBoss_Trigger(const CMidBoss_Trigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMidBoss_Trigger::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CMidBoss_Trigger::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(239.f, 56.f, -124.f));

	m_bIsAlive = true;

	return NOERROR;
}

_int CMidBoss_Trigger::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive && m_TimeAcc <= 5.0)
	{
		// 여기서 충돌검ㅅ ㅏ요청
		if (FAILED(CCollisionMgr::GetInstance()->Collision_MidBoss_Trigger_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox")))
			return E_FAIL;

		m_pColliderCom->Update_Collider();
	}
	else if (m_bIsActive && m_TimeAcc < 5.0)
	{
		m_TimeAcc += TimeDelta;

		if (m_TimeAcc >= 5.0)
		{
			list<CGameObject*>* pMonsterLayer = m_pManagement->Get_Layer(g_eScene, L"Layer_Monster");

			for (auto& pMonster : *pMonsterLayer)
			{
				if (((CMonster*)pMonster)->GetType() == CMonster::TYPE_BALROG)
				{
					CTransform* pTransform = (CTransform*)pMonster->Get_Component(L"Com_Transform");

					_vec3 vMonsterPosition = pTransform->Get_State(CTransform::STATE_POSITION);
					_vec3 vMonsterLook = pTransform->Get_State(CTransform::STATE_LOOK);
					D3DXVec3Normalize(&vMonsterLook, &vMonsterLook);

					_vec3 vCameraPosition = vMonsterPosition + vMonsterLook * 30.f;
					vCameraPosition.y += 10.f;

					CCamera_Event* pCamera_Event = (CCamera_Event*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Camera", 2);
					pCamera_Event->Set_Position(vCameraPosition);
					pCamera_Event->Set_Target(pTransform);

					m_pManagement->Play_Camera(g_eScene, L"Camera_Event");

					((CBalrog*)pMonster)->GetReady();

					CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

					_vec3 vPlayerPosition = vMonsterPosition + vMonsterLook * 20.f;

					pPlayer->Set_Position(vPlayerPosition);
					pPlayer->Set_Look(vMonsterPosition);
					pPlayer->SetIsControl(false);

					// 3. UI 다 집어 넣음
					list<CGameObject*>* pUILayer = m_pManagement->Get_Layer(g_eScene, L"Layer_UI");

					for (auto& pUI : *pUILayer)
						m_pManagement->Push_GameObject(g_eScene, L"Layer_UI", pUI);

					break;
				}
			}
		}
	}

	return _int();
}

_int CMidBoss_Trigger::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CMidBoss_Trigger::Render_GameObject()
{
	m_pColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CMidBoss_Trigger::Add_Component(void * pArg)
{
	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Collider
	CCollider::COLLIDER_DESC tDesc;
	tDesc.fRadius = 0.f;
	tDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDesc.vLocalScale = _vec3(5.f, 5.f, 5.f);

	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_Collider", (CComponent**)&m_pColliderCom, &tDesc))
		return E_FAIL;

	return NOERROR;
}

CMidBoss_Trigger * CMidBoss_Trigger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMidBoss_Trigger* pInstance = new CMidBoss_Trigger(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CMidBoss_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMidBoss_Trigger::Clone_GameObject(void * pArg)
{
	CMidBoss_Trigger* pInstance = new CMidBoss_Trigger(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CMidBoss_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMidBoss_Trigger::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
