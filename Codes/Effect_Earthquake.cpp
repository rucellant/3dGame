#include "stdafx.h"
#include "..\Headers\Effect_Earthquake.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CEffect_Earthquake::CEffect_Earthquake(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Earthquake::CEffect_Earthquake(const CEffect_Earthquake & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Earthquake::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Earthquake::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	return NOERROR;
}

_int CEffect_Earthquake::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
	m_TimeDelta = TimeDelta;
	
	return _int();
}

_int CEffect_Earthquake::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect_Earthquake::Render_GameObject()
{
	return NOERROR;
}

HRESULT CEffect_Earthquake::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
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

	// For. Com_CircleMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Earthquake_Circle", L"Com_CircleMesh", (CComponent**)&m_pCircleMeshCom))
		return E_FAIL;

	// For. Com_CylinderMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Earthquake_Circle", L"Com_CylinderMesh", (CComponent**)&m_pCylinderMeshCom))
		return E_FAIL;

	// For. Com_RingMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Earthquake_Ring", L"Com_RingMesh", (CComponent**)&m_pRingMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Earthquake::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CEffect_Earthquake::Render(_uint iPassIndex)
{
	return NOERROR;
}

CEffect_Earthquake * CEffect_Earthquake::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Earthquake* pInstance = new CEffect_Earthquake(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Earthquake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Earthquake::Clone_GameObject(void * pArg)
{
	CEffect_Earthquake* pInstance = new CEffect_Earthquake(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Earthquake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Earthquake::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCircleMeshCom);
	Safe_Release(m_pCylinderMeshCom);
	Safe_Release(m_pRingMeshCom);

	CEffect::Free();
}
