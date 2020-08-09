#include "stdafx.h"
#include "..\Headers\Shield.h"
#include "Management.h"


USING(Client)


CShield::CShield(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CShield::CShield(const CShield & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShield::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CShield::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(0.4f, 0.4f, 0.4f));

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_bIsAlive = true;

	return NOERROR;
}

_int CShield::Update_GameObject(_double TimeDelta)
{
	_matrix matWorld = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_LEFTHAND);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&matWorld.m[3]);
	m_pTransformCom->Set_Scale(_vec3(0.4f, 0.4f, 0.4f));

	return _int();
}

_int CShield::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CShield::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CShield::Add_Component(void * pArg)
{
	// For. Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Weapon", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For. Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For. Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For. Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Shield", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CShield::SetUp_ConstantTable()
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

HRESULT CShield::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_ulong dwNumSubset = m_pMeshCom->Get_NumSubset();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		m_pShaderCom->Commit_Change();

		m_pMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CShield * CShield::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CShield* pInstance = new CShield(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShield::Clone_GameObject(void * pArg)
{
	CShield* pInstance = new CShield(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CShield");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShield::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	Safe_Release(m_pObserver);

	CGameObject::Free();
}
