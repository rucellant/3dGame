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

	for (_int i = 0; i < 12; i++)
		D3DXMatrixIdentity(&(m_matCylinder[i]));

	m_bIsAlive = false;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Buff", this);

	return NOERROR;
}

_int CEffect_Buff::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
	m_TimeDelta = TimeDelta;

	for (_int i = 0; i < 12; i++)
	{
		if (m_TimeAcc >= 0.05f *i)
			m_bTimeAcc[i] = true;
	}

	for (_int i = 0; i < 12; i++)
	{
		if (m_bTimeAcc[i])
			m_TimeCylinderAcc[i] += TimeDelta;
	}

	return _int();
}

_int CEffect_Buff::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CEffect_Buff::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pCylinderMeshCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0, CIRCLE)))
		return E_FAIL;

	if (FAILED(Render(7, CIRCLE)))
		return E_FAIL;

	for (_int i = 0; i < 12; i++)
	{
		if (FAILED(SetUp_ConstantTable(i, CYLINDER)))
			return E_FAIL;

		if (FAILED(Render(8, CYLINDER)))
			return E_FAIL;
	}

	if (m_TimeAcc >= 2.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Buff", this);
	}

	return NOERROR;
}

HRESULT CEffect_Buff::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);

	_vec3 vRight = *(_vec3*)&matTarget.m[0];
	_vec3 vUp = *(_vec3*)&matTarget.m[1];
	_vec3 vLook = *(_vec3*)&matTarget.m[2];

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	// Circle
	{
		_float fScale = 5.f;

		_vec3 vTmpRight = vRight * fScale;
		_vec3 vTmpUp = vUp * fScale;
		_vec3 vTmpLook = vLook * fScale;

		memcpy((_vec3*)&m_matCircle.m[0], &vTmpRight, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle.m[1], &vTmpUp, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle.m[2], &vTmpLook, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle.m[3], (_vec3*)&matTarget.m[3], sizeof(_vec3));

		m_matCircle.m[3][1] += 0.1f;
	}

	// Cylinder
	for (_int i = 0; i < 12; i++)
	{
		_float fScale = 1.f;

		_vec3 vTmpRight = vRight * fScale * 0.1f;
		_vec3 vTmpUp = vUp * fScale * 2.f;
		_vec3 vTmpLook = vLook * fScale * 0.1f;

		memcpy((_vec3*)&m_matCylinder[i].m[0], &vTmpRight, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder[i].m[1], &vTmpUp, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder[i].m[2], &vTmpLook, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder[i].m[3], (_vec3*)&matTarget.m[3], sizeof(_vec3));

		_matrix matRotation;

		D3DXMatrixRotationY(&matRotation, D3DXToRadian(30.f * _float(i)));;
		D3DXVec3TransformNormal((_vec3*)&m_matCylinder[i].m[0], (_vec3*)&m_matCylinder[i].m[0], &matRotation);
		D3DXVec3TransformNormal((_vec3*)&m_matCylinder[i].m[1], (_vec3*)&m_matCylinder[i].m[1], &matRotation);
		D3DXVec3TransformNormal((_vec3*)&m_matCylinder[i].m[2], (_vec3*)&m_matCylinder[i].m[2], &matRotation);

		_vec3 vLook = *(_vec3*)&m_matCylinder[i].m[2];
		D3DXVec3Normalize(&vLook, &vLook);

		_vec3 vPosition = *(_vec3*)&m_matCylinder[i].m[3] + vLook * 2.f;
		memcpy((_vec3*)&m_matCylinder[i].m[3], &vPosition, sizeof(_vec3));

		m_TimeCylinderAcc[i] = 0.0;
		m_bTimeAcc[i] = false;
	}

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

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_HRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_CylinderMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Buff_Cylinder", L"Com_CylinderMesh", (CComponent**)&m_pCylinderMeshCom))
		return E_FAIL;

	

	// For. Com_HelixTMesh
	/*if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_HelixT_Ring", L"Com_HelixTMesh", (CComponent**)&m_pHelixTMeshCom))
		return E_FAIL;*/

	return NOERROR;
}

HRESULT CEffect_Buff::SetUp_ConstantTable(_int iIndex, MESH_TYPE eType)
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

		_float fTheta = D3DXToRadian(_float(m_TimeAcc) * 360.f) * 0.1f;
		if (FAILED(m_pShaderCom->Set_Value("g_fAngle", &fTheta, sizeof(_float))))
			return E_FAIL;
	}
	else if (eType == CYLINDER)
	{
		_matrix matWVP = m_matCylinder[iIndex] * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matCylinder, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(1))))
			return E_FAIL;

		_float fTimeAcc = _float(m_TimeCylinderAcc[iIndex]);
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
		m_pVIBufferCom->Render_VIBuffer();
	else if (eType == CYLINDER)
	{
		dwNumSubset = m_pCylinderMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pCylinderMeshCom->Render_Mesh(i);
	}
	else if (eType == HELIXT)
	{
		/*dwNumSubset = m_pHelixTMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pHelixTMeshCom->Render_Mesh(i);*/
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
	Safe_Release(m_pCylinderMeshCom);
	Safe_Release(m_pVIBufferCom);

	CEffect::Free();
}
