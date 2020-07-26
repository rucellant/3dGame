#include "stdafx.h"
#include "..\Headers\Loading_Icon.h"
#include "Management.h"


USING(Client)


CLoading_Icon::CLoading_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CLoading_Icon::CLoading_Icon(const CLoading_Icon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoading_Icon::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CLoading_Icon::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(0.05f, 0.05f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(0.f, -0.8f, 0.f));

	m_bIsAlive = true;

	return NOERROR;
}

_int CLoading_Icon::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 1.0)
		m_TimeAcc = 0.0;

	m_pTransformCom->Set_Rotation(&_vec3(0.f, 0.f, 1.f), _float(D3DXToRadian(360.0 * m_TimeAcc)));

	return _int();
}

_int CLoading_Icon::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (FAILED(m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this)))
		return -1;

	return _int();
}

HRESULT CLoading_Icon::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading_Icon::Add_Component(void * pArg)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Loading_Icon", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading_Icon::SetUp_ConstantTable()
{
	_matrix matWVP = m_pTransformCom->Get_WorldMatrix();

	m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix));
	m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pTextureCom->Get_Texture(0));

	return NOERROR;
}

HRESULT CLoading_Icon::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CLoading_Icon * CLoading_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLoading_Icon* pInstance = new CLoading_Icon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CLoading_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoading_Icon::Clone_GameObject(void * pArg)
{
	CLoading_Icon* pInstance = new CLoading_Icon(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CLoading_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Icon::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CGameObject::Free();
}
