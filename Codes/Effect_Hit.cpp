#include "stdafx.h"
#include "..\Headers\Effect_Hit.h"
#include "Management.h"


USING(Client)


CEffect_Hit::CEffect_Hit(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Hit::CEffect_Hit(const CEffect_Hit & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Hit::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Hit::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_bIsAlive = false;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Hit", this);

	return NOERROR;
}

_int CEffect_Hit::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += (TimeDelta * 5.0);

	return _int();
}

_int CEffect_Hit::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	Set_Billboard();
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CEffect_Hit::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(2)))
		return E_FAIL;

	if (m_TimeAcc >= 1.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Hit", this);
	}

	return NOERROR;
}

void CEffect_Hit::Set_Position(_vec3 vPosition, _float fScale)
{
	m_pTransformCom->Set_Scale(_vec3(fScale, fScale, fScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

HRESULT CEffect_Hit::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Hit", L"Com_Texture", (CComponent**)&m_pTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Hit::SetUp_ConstantTable()
{
	if (m_pTransformCom == nullptr || m_pShaderCom == nullptr)
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

	_int iTextureIndex = _int(m_TimeAcc * m_pTextureCom->Get_Size());
	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(iTextureIndex))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Hit::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CEffect_Hit::Set_Billboard()
{
	_matrix matView = m_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3 vRight = *(_vec3*)&matView.m[0];
	_vec3 vUp = *(_vec3*)&matView.m[1];
	_vec3 vLook = *(_vec3*)&matView.m[2];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	return NOERROR;
}

CEffect_Hit * CEffect_Hit::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Hit* pInstance = new CEffect_Hit(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Hit::Clone_GameObject(void * pArg)
{
	CEffect_Hit* pInstance = new CEffect_Hit(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Hit::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CEffect::Free();
}
