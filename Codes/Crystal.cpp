#include "stdafx.h"
#include "..\Headers\Crystal.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Door_Trigger.h"

USING(Client)

CCrystal::CCrystal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCrystal::CCrystal(const CCrystal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCrystal::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCrystal::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(1.1f, 1.1f, 1.1f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	m_bIsAlive = true;

	CDoor_Trigger* pTrigger = (CDoor_Trigger*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Trigger");

	pTrigger->Register_GameObject(this);

	m_TimeShader = 1.0;

	return NOERROR;
}

_int CCrystal::Update_GameObject(_double TimeDelta)
{
	// 여기서 충돌 검사 요청함
	CCollisionMgr::GetInstance()->Collision_Crystal_Player(g_eScene, this, L"Layer_Player", L"Com_DmgBox", &m_bIsHit);

	if (m_iHitCount >= 3)
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Crystal", this);

	if (m_iHitCount == 1)
	{
		if (m_TimeShader <= 1.5)
			m_TimeShader += TimeDelta;
	}

	if (m_iHitCount == 2)
	{
		if (m_TimeShader <= 2.0)
			m_TimeShader += TimeDelta;
	}

	m_pColliderCom->Update_Collider();

	return _int();
}

_int CCrystal::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CCrystal::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(1)))
		return E_FAIL;

	m_pColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CCrystal::Add_Component(void * pArg)
{
	m_tObjDesc = *(OBJDESC*)pArg;

	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Mesh_Static", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Frustum
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(g_eScene, L"Component_Mesh_Crystal", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_Collider
	CCollider::COLLIDER_DESC tColliderDesc;
	tColliderDesc.fRadius = 0.f;
	tColliderDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tColliderDesc.vLocalPosition = _vec3(0.f, 3.f, 0.f);
	tColliderDesc.vLocalScale = _vec3(2.f, 8.f, 2.f);

	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_Collider", (CComponent**)&m_pColliderCom, &tColliderDesc))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCrystal::SetUp_ConstantTable()
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
		return E_FAIL;

	_float fTimeShader = _float(m_TimeShader);
	if (FAILED(m_pShaderCom->Set_Value("g_fTimeDelta", &fTimeShader, sizeof(_float))))
		return E_FAIL;


	return NOERROR;
}

HRESULT CCrystal::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_ulong dwNumSubset = m_pMeshCom->Get_NumSubset();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_NormalTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_NORMAL))))
			return E_FAIL;

		m_pShaderCom->Commit_Change();

		m_pMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CCrystal * CCrystal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCrystal* pInstance = new CCrystal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrystal::Clone_GameObject(void * pArg)
{
	CCrystal* pInstance = new CCrystal(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CCrystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrystal::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
