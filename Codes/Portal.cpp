#include "stdafx.h"
#include "..\Headers\Portal.h"
#include "Management.h"


USING(Client)


CPortal::CPortal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPortal::CPortal(const CPortal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPortal::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPortal::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(_vec3(10.f, 20.f, 10.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(14.f, 14.f, 78.f));
	m_pTransformCom->Set_Rotation(&_vec3(0.f, 1.f, 0.f), D3DXToRadian(-15.f));

	m_bIsAlive = true;

	return NOERROR;
}

_int CPortal::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 10.0)
		m_TimeAcc = 0.0;

	return _int();
}

_int CPortal::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	/*if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);*/

	/*if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f))
	{
		Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);
	}*/
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CPortal::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPortal::Add_Component(void * pArg)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Portal", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPortal::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(1))))
		return E_FAIL;

	_float fTimeAcc = _float(m_TimeAcc);
	if (FAILED(m_pShaderCom->Set_Value("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pManagement->SetRenderTarget_OnShader(m_pShaderCom, "g_DepthTexture", L"Target_Depth")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPortal::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CPortal * CPortal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPortal* pInstance = new CPortal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CPortal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPortal::Clone_GameObject(void * pArg)
{
	CPortal* pInstance = new CPortal(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CPortal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPortal::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}
