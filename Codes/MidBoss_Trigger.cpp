#include "stdafx.h"
#include "..\Headers\MidBoss_Trigger.h"
#include "Management.h"

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
	m_pColliderCom->Update_Collider();

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
	tDesc.vLocalScale = _vec3(12.f, 12.f, 12.f);

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
