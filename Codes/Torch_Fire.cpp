#include "stdafx.h"
#include "..\Headers\Torch_Fire.h"
#include "Management.h"

USING(Client)

CTorch_Fire::CTorch_Fire(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTorch_Fire::CTorch_Fire(const CTorch_Fire & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTorch_Fire::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CTorch_Fire::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)pArg);

	m_iNumTexture = m_pTextureCom->Get_Size();

	m_bIsAlive = true;

	return NOERROR;
}

_int CTorch_Fire::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
	
	if (m_TimeAcc >= 1.0)
		m_TimeAcc = 0.0;

	return _int();
}

_int CTorch_Fire::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f))
	{
		Set_Billboard();
		Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);
	}

	return _int();
}

HRESULT CTorch_Fire::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(9)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTorch_Fire::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Torch", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTorch_Fire::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(_uint(m_iNumTexture * m_TimeAcc)))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTorch_Fire::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CTorch_Fire::Set_Billboard()
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

CTorch_Fire * CTorch_Fire::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTorch_Fire* pInstance = new CTorch_Fire(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CTorch_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTorch_Fire::Clone_GameObject(void * pArg)
{
	CTorch_Fire* pInstance = new CTorch_Fire(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CTorch_Fire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTorch_Fire::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}
