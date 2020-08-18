#include "stdafx.h"
#include "..\Headers\Twister.h"
#include "Management.h"
#include "CollisionMgr.h"

USING(Client)

CTwister::CTwister(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTwister::CTwister(const CTwister & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTwister::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CTwister::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	CTransform::STATEDESC tStateDesc;
	tStateDesc.fRotationPerSec = D3DXToRadian(90.f);
	tStateDesc.fSpeedPerSec = 30.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_bIsAlive = true;

	return NOERROR;
}

_int CTwister::Update_GameObject(_double TimeDelta)
{
	if (m_bWay)
		m_pTransformCom->Move_Left(TimeDelta);
	else
		m_pTransformCom->Move_Right(TimeDelta);
	
	if (m_bWay && m_pTransformCom->Get_State(CTransform::STATE_POSITION).x <= 140.f)
		m_bWay = !m_bWay;
	if(!m_bWay && m_pTransformCom->Get_State(CTransform::STATE_POSITION).x >= 180.f)
		m_bWay = !m_bWay;

	if (FAILED(CCollisionMgr::GetInstance()->Collision_Twister_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox")))
		return E_FAIL;

	m_pDmgColliderCom->Update_Collider();

	return _int();
}

_int CTwister::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CTwister::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	m_pDmgColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CTwister::Add_Component(void * pArg)
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
	if (CGameObject::Add_Component(g_eScene, L"Component_Mesh_Twister", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_DmgBox
	CCollider::COLLIDER_DESC tDmgDesc;
	tDmgDesc.fRadius = 3.f;
	tDmgDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDmgDesc.vLocalScale = _vec3(0.f, 0.f, 0.f);

	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_DmgBox", (CComponent**)&m_pDmgColliderCom, &tDmgDesc))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTwister::SetUp_ConstantTable()
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

	return NOERROR;
}

HRESULT CTwister::Render(_uint iPassIndex)
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

CTwister * CTwister::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTwister* pInstance = new CTwister(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CTwister");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTwister::Clone_GameObject(void * pArg)
{
	CTwister* pInstance = new CTwister(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CTwister");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTwister::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDmgColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
