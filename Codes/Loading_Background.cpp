#include "stdafx.h"
#include "..\Headers\Loading_Background.h"
#include "Management.h"


USING(Client)


CLoading_Background::CLoading_Background(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CLoading_Background::CLoading_Background(const CLoading_Background & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoading_Background::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CLoading_Background::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_iNumTexture = m_pTextureCom->Get_Size();

	m_bIsAlive = true;

	return NOERROR;
}

_int CLoading_Background::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 2.0)
	{
		if (++m_iCurTexture >= m_iNumTexture)
			m_iCurTexture = 1;

		m_TimeAcc = 0.0;
	}

	return _int();
}

_int CLoading_Background::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this)))
		return -1;

	return _int();
}

HRESULT CLoading_Background::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_iCurTexture)))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading_Background::Add_Component(void * pArg)
{
	// For. Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For. Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For. Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Default", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For. Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Loading_Background", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading_Background::SetUp_ConstantTable(_uint iTextureIndex)
{
	_matrix matWVP;
	D3DXMatrixIdentity(&matWVP);

	m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix));
	m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture(m_iCurTexture));

	return NOERROR;
}

HRESULT CLoading_Background::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CLoading_Background * CLoading_Background::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLoading_Background* pInstance = new CLoading_Background(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CLoading_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoading_Background::Clone_GameObject(void * pArg)
{
	CLoading_Background* pInstance = new CLoading_Background(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CLoading_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Background::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}
