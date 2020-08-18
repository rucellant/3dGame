#include "stdafx.h"
#include "..\Headers\Quatran.h"
#include "Player.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Observer_Player.h"


USING(Client)


CQuatran::CQuatran(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CQuatran::CQuatran(const CQuatran & rhs)
	: CMonster(rhs)
{
}

HRESULT CQuatran::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CQuatran::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(0)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&m_tObjDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&m_tObjDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&m_tObjDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	CTransform::STATEDESC tStateDesc;
	tStateDesc.fRotationPerSec = D3DXToRadian(90.f);
	tStateDesc.fSpeedPerSec = 10.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	SetUp_OnNavigation();

	m_bIsAlive = true;

	return NOERROR;
}

_int CQuatran::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	if (m_pManagement->KeyUp(KEY_UP))
	{
		m_iAnimation++;

		if (m_iAnimation > 15)
			m_iAnimation = 0;
	}

	/*if (m_pManagement->KeyUp(KEY_UP))
		m_TmpDuration += 0.01;
	if (m_pManagement->KeyUp(KEY_DOWN))
		m_TmpDuration -= 0.01;
	if (m_pManagement->KeyUp(KEY_RIGHT))
		m_TmpPeriod += 0.001;
	if (m_pManagement->KeyUp(KEY_LEFT))
		m_TmpPeriod -= 0.001;*/

	if (FAILED(State_Machine(TimeDelta)))
		return E_FAIL;

	Update_Collider();

	return _int();
}

_int CQuatran::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CQuatran::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->Play_AnimationSet(m_TimeDelta)))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CQuatran::Add_Component(void * pArg)
{
	m_tObjDesc = *(OBJDESC*)pArg;

	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Monster", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Frustum
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Navigation
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Navigation_Stage", L"Com_Navigation", (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(SCENE_STAGE, L"Component_Mesh_Quatran", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_HitBox
	CCollider::COLLIDER_DESC tDesc;
	tDesc.fRadius = 0.f;
	tDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDesc.vLocalPosition = _vec3(0.f, 2.f, 0.f);
	tDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_HitBox", (CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CQuatran::SetUp_ConstantTable()
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
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

	return NOERROR;
}

HRESULT CQuatran::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		_ulong dwNumSubset = m_pMeshCom->Get_NumSubset(_ulong(i));

		m_pMeshCom->Update_SkinnedMesh(_ulong(i));

		for (_ulong j = 0; j < dwNumSubset; ++j)
		{
			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(_ulong(i), _ulong(j), MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Set_Texture("g_NormalTexture", m_pMeshCom->Get_MaterialTexture(_ulong(i), _ulong(j), MESHTEXTURE::TYPE_NORMAL))))
				return E_FAIL;

			m_pShaderCom->Commit_Change();

			m_pMeshCom->Render_Mesh(_ulong(i), _ulong(j));
		}
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CQuatran::State_Machine(_double TimeDelta)
{
	HRESULT hr = 0;

	switch (m_eCurState)
	{
	case IDLE:
		hr = State_Idle(TimeDelta);
		break;
	case RUN:
		hr = State_Run(TimeDelta);
		break;
	case ATT:
		hr = State_Att(TimeDelta);
		break;
	case HIT:
		hr = State_Hit(TimeDelta);
		break;
	case DOWN:
		hr = State_Down(TimeDelta);
		break;
	case GROGGY:
		hr = State_Groggy(TimeDelta);
		break;
	case DEAD:
		hr = State_Dead(TimeDelta);
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return NOERROR;
}

HRESULT CQuatran::State_Idle(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Run(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Att(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Hit(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Down(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Groggy(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::State_Dead(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CQuatran::Update_Collider()
{
	m_pColliderCom->Update_Collider();

	return NOERROR;
}

HRESULT CQuatran::SetUp_OnNavigation()
{
	_float fHeight = m_pNavigationCom->SetUp_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = fHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

CQuatran * CQuatran::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQuatran* pInstance = new CQuatran(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CQuatran");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuatran::Clone_GameObject(void * pArg)
{
	CQuatran* pInstance = new CQuatran(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CQuatran");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuatran::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CMonster::Free();
}
