#include "stdafx.h"
#include "..\Headers\WitchBlade.h"
#include "Management.h"

USING(Client)

CWitchBlade::CWitchBlade(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWitchBlade::CWitchBlade(const CWitchBlade & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWitchBlade::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CWitchBlade::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(47)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&m_tObjDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&m_tObjDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&m_tObjDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	m_bIsAlive = true;

	return NOERROR;
}

_int CWitchBlade::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	return _int();
}

_int CWitchBlade::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CWitchBlade::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pMeshCom->Play_AnimationSet(m_TimeDelta)))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWitchBlade::Add_Component(void * pArg)
{
	m_tObjDesc = *(OBJDESC*)pArg;

	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_WitchBlade", L"Com_Shader", (CComponent**)&m_pShaderCom))
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

	// For. Com_Navigation
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Navigation_Stage", L"Com_Navigation", (CComponent**)&m_pNavigationCom))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_WitchBlade", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWitchBlade::SetUp_ConstantTable()
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

HRESULT CWitchBlade::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (size_t i = 0; i < iNumMeshContainer; ++i)
	{
		_ulong dwNumSubset = m_pMeshCom->Get_NumSubset(_ulong(i));

		m_pMeshCom->Update_SkinnedMesh(_ulong(i));

		for (_ulong j = 0; j < dwNumSubset; ++j)
		{
			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(_ulong(i), _ulong(j), MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Set_Texture("g_NormalTexture", m_pMeshCom->Get_MaterialTexture(_ulong(i), _ulong(j), MESHTEXTURE::TYPE_NORMAL))))
				return E_FAIL;

			m_pShaderCom->Commit_Change();

			m_pMeshCom->Render_Mesh(_ulong(i), _ulong(j));
		}
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	m_pNavigationCom->Render_Navigation();

	return NOERROR;
}

CWitchBlade * CWitchBlade::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWitchBlade* pInstance = new CWitchBlade(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CWitchBlade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWitchBlade::Clone_GameObject(void * pArg)
{
	CWitchBlade* pInstance = new CWitchBlade(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CWitchBlade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWitchBlade::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
