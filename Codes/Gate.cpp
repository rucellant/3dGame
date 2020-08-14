#include "stdafx.h"
#include "..\Headers\Gate.h"
#include "Management.h"

USING(Client)

CGate::CGate(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CGate::CGate(const CGate & rhs)
	: CGameObject(rhs)
{
}

_matrix CGate::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CGate::Set_Position(_vec3 vData)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vData);
}

HRESULT CGate::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CGate::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(-160.f, -13.f, 26.f));

	m_bIsAlive = true;


	return NOERROR;
}

_int CGate::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CGate::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 10.f))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	//m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CGate::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGate::Add_Component(void * pArg)
{
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
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Gate", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGate::SetUp_ConstantTable()
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

HRESULT CGate::Render(_uint iPassIndex)
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

CGate * CGate::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGate* pInstance = new CGate(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGate::Clone_GameObject(void * pArg)
{
	CGate* pInstance = new CGate(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CGate");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGate::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
