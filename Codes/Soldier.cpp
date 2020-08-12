#include "stdafx.h"
#include "..\Headers\Soldier.h"
#include "Management.h"
#include "ColliderManager.h"
#include "Observer_Player.h"

USING(Client)

CSoldier::CSoldier(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CSoldier::CSoldier(const CSoldier & rhs)
	: CMonster(rhs)
{
}

HRESULT CSoldier::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSoldier::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(0)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&m_tObjDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&m_tObjDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&m_tObjDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	SetUp_OnNavigation();

	m_iAnimation = SOLDIER_IDLE;

	m_bActive = false;

	m_eType = TYPE_SOLDIER;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_bIsAlive = true;

	return NOERROR;
}

_int CSoldier::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	if (m_bActive)
	{
		if (FAILED(State_Machine(TimeDelta)))
			return E_FAIL;
	}

	Update_Collider();

	return _int();
}

_int CSoldier::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CSoldier::Render_GameObject()
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

	m_pDmgColliderCom->Render_Collider();
	m_pDetectColliderCom->Render_Collider();
	m_pAttRangeColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CSoldier::Knockdown(_vec3 vPosition)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::GetHit(_vec3 vPosition)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::Follow_Player(_vec3 vPosition)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::Add_Component(void * pArg)
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
	if (CGameObject::Add_Component(SCENE_STAGE, L"Component_Mesh_Soldier", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_DmgBox
	CCollider::COLLIDER_DESC tDmgDesc;
	tDmgDesc.fRadius = 4.f;
	tDmgDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDmgDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_DmgBox", (CComponent**)&m_pDmgColliderCom, &tDmgDesc)))
		return E_FAIL;

	// For. Com_DetectBox
	CCollider::COLLIDER_DESC tDetectDesc;
	tDetectDesc.fRadius = 13.f;
	tDetectDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDetectDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDetectDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_DetectBox", (CComponent**)&m_pDetectColliderCom, &tDetectDesc)))
		return E_FAIL;

	// For. Com_AttRangeBox
	CCollider::COLLIDER_DESC tAttRangeDesc;
	tAttRangeDesc.fRadius = 3.f;
	tAttRangeDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tAttRangeDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tAttRangeDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_AttRangeBox", (CComponent**)&m_pAttRangeColliderCom, &tAttRangeDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSoldier::SetUp_ConstantTable()
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

HRESULT CSoldier::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (size_t i = 0; i < iNumMeshContainer; ++i)
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

HRESULT CSoldier::State_Machine(_double TimeDelta)
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

HRESULT CSoldier::State_Idle(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Run(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Att(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Hit(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Down(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Groggy(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::State_Dead(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSoldier::Update_Collider()
{
	m_pDmgColliderCom->Update_Collider();
	m_pDetectColliderCom->Update_Collider();
	m_pAttRangeColliderCom->Update_Collider();

	return NOERROR;
}

HRESULT CSoldier::SetUp_OnNavigation()
{
	_float fHeight = m_pNavigationCom->SetUp_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = fHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

CSoldier * CSoldier::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSoldier* pInstance = new CSoldier(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CSoldier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSoldier::Clone_GameObject(void * pArg)
{
	CSoldier* pInstance = new CSoldier(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CSoldier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoldier::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDmgColliderCom);
	Safe_Release(m_pDetectColliderCom);
	Safe_Release(m_pAttRangeColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CMonster::Free();
}
