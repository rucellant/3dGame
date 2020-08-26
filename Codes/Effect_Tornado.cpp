#include "stdafx.h"
#include "..\Headers\Effect_Tornado.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CEffect_Tornado::CEffect_Tornado(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Tornado::CEffect_Tornado(const CEffect_Tornado & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Tornado::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Tornado::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Tornado", this);

	return NOERROR;
}

_int CEffect_Tornado::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta += TimeDelta;

	return _int();
}

_int CEffect_Tornado::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CEffect_Tornado::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(1)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Tornado::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);
	
	matTarget.m[3][1] -= 3.f;

	_float fScale = D3DXVec3Length((_vec3*)&matTarget.m[0]);

	_vec3 vRight = *(_vec3*)&matTarget.m[0];
	_vec3 vUp = *(_vec3*)&matTarget.m[1];
	_vec3 vLook = *(_vec3*)&matTarget.m[2];
	_vec3 vPosition = *(_vec3*)&matTarget.m[3] + _vec3(0.f, 4.f, 0.f);

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK , vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pTransformCom->Move_Left(m_TimeDelta * 5.0);

	return NOERROR;
}

HRESULT CEffect_Tornado::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Frustum
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Tornado", L"Com_Texture", (CComponent**)&m_pTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(SCENE_STAGE, L"Component_Mesh_Effect_Tornado", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Tornado::SetUp_ConstantTable()
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

HRESULT CEffect_Tornado::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_ulong dwNumSubset = m_pMeshCom->Get_NumSubset();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture(0))))
			return E_FAIL;

		/*if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;*/

		m_pShaderCom->Commit_Change();

		m_pMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CEffect_Tornado * CEffect_Tornado::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Tornado* pInstance = new CEffect_Tornado(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Tornado");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Tornado::Clone_GameObject(void * pArg)
{
	CEffect_Tornado* pInstance = new CEffect_Tornado(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Tornado");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tornado::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	//Safe_Release(m_pObserver);

	CEffect::Free();
}
