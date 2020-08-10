#include "stdafx.h"
#include "..\Headers\GameObject_Static.h"
#include "Management.h"

USING(Client)

CGameObject_Static::CGameObject_Static(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CGameObject_Static::CGameObject_Static(const CGameObject_Static & rhs)
	: CGameObject(rhs)
{
}

_matrix CGameObject_Static::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CGameObject_Static::Set_Position(_vec3 vData)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vData);
}

HRESULT CGameObject_Static::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CGameObject_Static::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_bIsAlive = true;


	return NOERROR;
}

_int CGameObject_Static::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CGameObject_Static::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tStaticDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	//m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CGameObject_Static::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGameObject_Static::Add_Component(void * pArg)
{
	m_tStaticDesc = *(STATICDESC*)pArg;

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
	if (CGameObject::Add_Component(SCENE_STATIC, m_tStaticDesc.szFileName, L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGameObject_Static::SetUp_ConstantTable()
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

HRESULT CGameObject_Static::Render(_uint iPassIndex)
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

CGameObject_Static * CGameObject_Static::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGameObject_Static* pInstance = new CGameObject_Static(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CGameObject_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGameObject_Static::Clone_GameObject(void * pArg)
{
	CGameObject_Static* pInstance = new CGameObject_Static(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CGameObject_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameObject_Static::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
