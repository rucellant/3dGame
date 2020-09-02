#include "stdafx.h"
#include "..\Headers\Effect_Earthquake.h"
#include "Management.h"
#include "Observer_Player.h"
#include "Particle_Spread.h"


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

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	D3DXMatrixIdentity(&m_matCylinder);
	D3DXMatrixIdentity(&m_matRing);

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Earthquake", this);

	return NOERROR;
}

_int CEffect_Earthquake::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
	m_TimeDelta = TimeDelta;
	m_TimeCircleAcc += (TimeDelta * 4.0);

	if (m_TimeCircleAcc >= 1.0)
	{
		m_iCircleIndex++;

		if (m_iCircleIndex < 3)
			m_TimeCircleAcc = 0.0;
	}

	if (m_TimeAcc >= 0.8 && !m_bIsCreateParticle)
	{
		//요기서파티클 생성
		CParticle_Spread* pParticle = (CParticle_Spread*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Spread");

		if (pParticle == nullptr)
		{
			if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Particle_Spread", L"GameObject_Particle_Spread")))
				return E_FAIL;

			pParticle = (CParticle_Spread*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Spread");
		}

		if (pParticle == nullptr)
			return E_FAIL;

		// 토네이도 실행하기 전에 정보부터 넘겨줌 
		CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

		pPlayer->Update_PlayerInfo();

		pParticle->Activate();

		m_bIsCreateParticle = true;
	}

	return _int();
}

_int CEffect_Earthquake::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CEffect_Earthquake::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pCircleMeshCom == nullptr || m_pCylinderMeshCom == nullptr || m_pRingMeshCom == nullptr)
		return E_FAIL;

	if (m_TimeCircleAcc < 1.0 && m_iCircleIndex < 3)
	{
		if (FAILED(SetUp_ConstantTable(m_iCircleIndex, CIRCLE)))
			return E_FAIL;

		if (FAILED(Render(4, CIRCLE)))
			return E_FAIL;
	}

	if (m_TimeAcc < 0.8)
	{
		if (FAILED(SetUp_ConstantTable(m_iCircleIndex, CYLINDER)))
			return E_FAIL;

		if (FAILED(Render(5, CYLINDER)))
			return E_FAIL;
	}

	if (m_TimeAcc >= 0.8)
	{
		if (FAILED(SetUp_ConstantTable(m_iCircleIndex, RING)))
			return E_FAIL;

		if (FAILED(Render(6, RING)))
			return E_FAIL;
	}

	if (m_TimeAcc >= 2.0)
	{
		m_TimeAcc =0.0;
		m_iCircleIndex = 0;
		m_TimeCircleAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Earthquake", this);
		m_bIsCreateParticle = false;
	}

	return NOERROR;
}

HRESULT CEffect_Earthquake::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);

	// Circle
	_vec3 vRight = *(_vec3*)&matTarget.m[0];
	_vec3 vUp = *(_vec3*)&matTarget.m[1];
	_vec3 vLook = *(_vec3*)&matTarget.m[2];

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	for (_int i = 0; i < 3; i++)
	{
		m_matCircle[i] = matTarget;

		_float fScale = 0.25f;

		_vec3 vTmpRight = vRight * fScale;
		_vec3 vTmpUp = vUp * 0.2f;
		_vec3 vTmpLook = vLook * fScale;

		memcpy((_vec3*)&m_matCircle[i].m[0], &vTmpRight, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle[i].m[1], &vTmpUp, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle[i].m[2], &vTmpLook, sizeof(_vec3));
		memcpy((_vec3*)&m_matCircle[i].m[3], (_vec3*)&matTarget.m[3], sizeof(_vec3));
	}

	m_matCircle[0].m[3][1] += 3.f; m_matCircle[1].m[3][1] += 2.f; m_matCircle[2].m[3][1] += 1.f;

	_matrix matRotation;

	D3DXMatrixRotationY(&matRotation, D3DXToRadian(0.f));
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[0].m[0], (_vec3*)&m_matCircle[0].m[0], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[0].m[1], (_vec3*)&m_matCircle[0].m[1], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[0].m[2], (_vec3*)&m_matCircle[0].m[2], &matRotation);

	D3DXMatrixRotationY(&matRotation, D3DXToRadian(120.f));
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[1].m[0], (_vec3*)&m_matCircle[1].m[0], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[1].m[1], (_vec3*)&m_matCircle[1].m[1], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[1].m[2], (_vec3*)&m_matCircle[1].m[2], &matRotation);

	D3DXMatrixRotationY(&matRotation, D3DXToRadian(240.f));
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[2].m[0], (_vec3*)&m_matCircle[2].m[0], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[2].m[1], (_vec3*)&m_matCircle[2].m[1], &matRotation);
	D3DXVec3TransformNormal((_vec3*)&m_matCircle[2].m[2], (_vec3*)&m_matCircle[2].m[2], &matRotation);

	// Cylinder
	{
		_float fScale = 0.03f;

		_vec3 vTmpRight = vRight * fScale;
		_vec3 vTmpUp = vUp * fScale;
		_vec3 vTmpLook = vLook * fScale;

		memcpy((_vec3*)&m_matCylinder.m[0], &vTmpRight, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder.m[1], &vTmpUp, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder.m[2], &vTmpLook, sizeof(_vec3));
		memcpy((_vec3*)&m_matCylinder.m[3], (_vec3*)&matTarget.m[3], sizeof(_vec3));

		m_matCylinder.m[3][1] += 2.f;
	}

	// Ring
	{
		_float fScale = 0.1f;

		_vec3 vTmpRight = vRight * fScale;
		_vec3 vTmpUp = vUp * fScale;
		_vec3 vTmpLook = vLook * fScale;

		memcpy((_vec3*)&m_matRing.m[0], &vTmpRight, sizeof(_vec3));
		memcpy((_vec3*)&m_matRing.m[1], &vTmpUp, sizeof(_vec3));
		memcpy((_vec3*)&m_matRing.m[2], &vTmpLook, sizeof(_vec3));
		memcpy((_vec3*)&m_matRing.m[3], (_vec3*)&matTarget.m[3], sizeof(_vec3));

		//m_matRing.m[3][1] += 1.f;
	}

	return NOERROR;
}

HRESULT CEffect_Earthquake::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Earthquake", L"Com_Texture", (CComponent**)&m_pTextureCom))
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
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Earthquake_Cylinder", L"Com_CylinderMesh", (CComponent**)&m_pCylinderMeshCom))
		return E_FAIL;

	// For. Com_RingMesh
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Earthquake_Ring", L"Com_RingMesh", (CComponent**)&m_pRingMeshCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Earthquake::SetUp_ConstantTable(_int iIndex, MESH_TYPE eType)
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	if (eType == CIRCLE)
	{
		if (m_iCircleIndex >= 3)
			return NOERROR;

		_matrix matWVP = m_matCircle[iIndex] * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matCircle[iIndex], sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
			return E_FAIL;

		if (FAILED(m_pManagement->SetRenderTarget_OnShader(m_pShaderCom, "g_DepthTexture", L"Target_Depth")))
			return E_FAIL;

		_float fTimeCircleAcc = _float(m_TimeCircleAcc);
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

		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(1))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(4))))
			return E_FAIL;

		_float fTimeAcc = _float(m_TimeAcc);
		if (FAILED(m_pShaderCom->Set_Value("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;
	}
	else if (eType == RING)
	{
		_matrix matWVP = m_matRing * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matRing, sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		
		_float fTimeAcc = _float(D3DXToRadian(m_TimeAcc)); //_float(m_TimeAcc);
		if (FAILED(m_pShaderCom->Set_Value("g_fTimeAcc", &fTimeAcc, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(3))))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CEffect_Earthquake::Render(_uint iPassIndex, MESH_TYPE eType)
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
	else if (eType == RING)
	{
		dwNumSubset = m_pRingMeshCom->Get_NumSubset();

		for (_ulong i = 0; i < dwNumSubset; ++i)
			m_pRingMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

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
