#include "stdafx.h"
#include "..\Headers\Terrain.h"
#include "Management.h"
#include "Camera_Light.h"

USING(Client)

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

_matrix CTerrain::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldMatrix();
}

HRESULT CTerrain::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CTerrain::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

	m_bIsAlive = true;

	return NOERROR;
}

_int CTerrain::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CTerrain::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_SHADOW, this);
	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CTerrain::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (m_iRenderIndex == 0)
	{
		if (FAILED(SetUp_ConstantTable(0)))
			return E_FAIL;

		if (FAILED(Render(1)))
			return E_FAIL;

		m_iRenderIndex++;
	}
	else if (m_iRenderIndex == 1)
	{
		if (FAILED(SetUp_ConstantTable(1)))
			return E_FAIL;

		if (FAILED(Render(0)))
			return E_FAIL;

		m_iRenderIndex = 0;
	}

	return NOERROR;
}

HRESULT CTerrain::Add_Component(void * pArg)
{
	_tchar szMeshTag[MAX_STR];
	memcpy(szMeshTag, pArg, sizeof(_tchar) * MAX_STR);

	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Terrain", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(g_eScene, szMeshTag, L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTerrain::SetUp_ConstantTable(_uint iRenderIndex)
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	if (iRenderIndex == 0)
	{
		CCamera_Light* pCamera_Light = (CCamera_Light*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Camera", 3);
		_matrix matLightView = pCamera_Light->GetViewMatrix_Inverse();
		_matrix matLightProj = pCamera_Light->GetProjMatrix();

		_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * matLightView * matLightProj;

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &matLightView, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &matLightProj, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;
	}
	else if (iRenderIndex == 1)
	{
		_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pManagement->SetRenderTarget_OnShader(m_pShaderCom, "g_ShadowTexture", L"Target_Shadow")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pManagement->SetRenderTarget_OnShader(m_pShaderCom, "g_ShadowTexture", L"Target_Shadow")))
			return E_FAIL;

		CCamera_Light* pCamera_Light = (CCamera_Light*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Camera", 3);
		_matrix matLightView = pCamera_Light->GetViewMatrix_Inverse();
		_matrix matLightProj = pCamera_Light->GetProjMatrix();

		if (FAILED(m_pShaderCom->Set_Value("g_matLightView", &matLightView, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matLightProj", &matLightProj, sizeof(_matrix))))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTerrain::Render(_uint iPassIndex)
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

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain* pInstance = new CTerrain(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone_GameObject(void * pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	
	CGameObject::Free();
}
