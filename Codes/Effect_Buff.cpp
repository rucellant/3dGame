#include "stdafx.h"
#include "..\Headers\Effect_Buff.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)

CEffect_Buff::CEffect_Buff(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Buff::CEffect_Buff(const CEffect_Buff & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Buff::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Buff::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	D3DXMatrixIdentity(&m_matCircle);
	D3DXMatrixIdentity(&m_matHelixT);
	D3DXMatrixIdentity(&m_matCylinder);

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Buff", this);

	return NOERROR;
}

_int CEffect_Buff::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CEffect_Buff::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Buff::Render_GameObject()
{
	return NOERROR;
}

HRESULT CEffect_Buff::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Buff", L"Com_Texture", (CComponent**)&m_pTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_CircleMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Buff_Circle", L"Com_CircleMesh", (CComponent**)&m_pCircleMeshCom))
		return E_FAIL;

	// For. Com_CylinderMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Buff_Cylinder", L"Com_CylinderMesh", (CComponent**)&m_pCylinderMeshCom))
		return E_FAIL;

	// For. Com_HelixTMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_HelixT_Ring", L"Com_HelixTMesh", (CComponent**)&m_pHelixTMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Buff::SetUp_ConstantTable(MESH_TYPE eType)
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	if (eType == CIRCLE)
	{
		_matrix matWVP = m_matCircle * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matCircle, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
			return E_FAIL;

		_float fTimeCircleAcc = _float(m_TimeAcc);
		if (FAILED(m_pShaderCom->Set_Value("g_fTimeAcc", &fTimeCircleAcc, sizeof(_float))))
			return E_FAIL;
	}
	else if (eType == CYLINDER)
	{
		_matrix matWVP = m_matCylinder * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matCylinder, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
			return E_FAIL;

		_float fTimeAcc = _float(m_TimeAcc);
		if (FAILED(m_pShaderCom->Set_Value("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;
	}
	else if (eType == HELIXT)
	{
		_matrix matWVP = m_matHelixT * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matHelixT, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CEffect_Buff::Render(_uint iPassIndex, MESH_TYPE eType)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_ulong dwNumSubset = 0;

	if (eType == CIRCLE)
	{
		dwNumSubset = m_pCircleMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pCircleMeshCom->Render_Mesh(i);
	}
	else if (eType == CYLINDER)
	{
		dwNumSubset = m_pCylinderMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pCylinderMeshCom->Render_Mesh(i);
	}
	else if (eType == HELIXT)
	{
		dwNumSubset = m_pHelixTMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pHelixTMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CEffect_Buff * CEffect_Buff::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Buff* pInstance = new CEffect_Buff(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Buff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Buff::Clone_GameObject(void * pArg)
{
	CEffect_Buff* pInstance = new CEffect_Buff(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Buff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Buff::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCircleMeshCom);
	Safe_Release(m_pCylinderMeshCom);
	Safe_Release(m_pHelixTMeshCom);

	CEffect::Free();
}
