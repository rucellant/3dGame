#include "stdafx.h"
#include "..\Headers\Skeleton.h"
#include "Player.h"
#include "Management.h"
#include "Effect_Hit.h"
#include "CollisionMgr.h"
#include "Particle_Dead.h"
#include "Observer_Player.h"

USING(Client)

CSkeleton::CSkeleton(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CSkeleton::CSkeleton(const CSkeleton & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkeleton::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSkeleton::Ready_GameObject_Clone(void * pArg)
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

	m_iAnimation = SKELETON_IDLE;

	m_eCurState = IDLE;

	m_eType = TYPE_SKELETON;

	m_bActive = true;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_tMonsterInfo.iCurHp = 1000;
	m_tMonsterInfo.iMaxHp = 1000;
	m_tMonsterInfo.iMaxDmg = 50;
	m_tMonsterInfo.iMinDmg = 100;
	m_tMonsterInfo.iGold = 100;

	m_bIsAlive = true;

	return NOERROR;
}

_int CSkeleton::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	/*if (m_pManagement->KeyUp(KEY_UP))
		m_TmpDuration += 0.01;
	if (m_pManagement->KeyUp(KEY_DOWN))
		m_TmpDuration -= 0.01;
	if (m_pManagement->KeyUp(KEY_RIGHT))
		m_TmpPeriod += 0.001;
	if (m_pManagement->KeyUp(KEY_LEFT))
		m_TmpPeriod -= 0.001;*/

	if (m_bActive)
	{
		if (FAILED(State_Machine(TimeDelta)))
			return E_FAIL;
	}

	Update_Collider();

	return _int();
}

_int CSkeleton::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CSkeleton::Render_GameObject()
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

	//m_pHitColliderCom->Render_Collider();
	//m_pDmgColliderCom->Render_Collider();
	//m_pDetectColliderCom->Render_Collider();
	//m_pAttRangeColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CSkeleton::Knockdown(_vec3 vPosition, _int iPlayerDmg)
{
	m_iAnimation = SKELETON_DOWN_START;

	m_eCurState = DOWN;
	
	m_fNewSpeed = 1.f;
	m_Duration = 0.2;
	m_Period = 0.2;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);
	
	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScaleRight = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScaleUp = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScaleLook = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScaleRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScaleUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScaleLook);
	
	m_tMonsterInfo.iCurHp -= iPlayerDmg;

	// 여기서 맞는 이펙트
	D3DXFRAME_DERIVED* pFrame_Derived = m_pMeshCom->Get_FrameDerived("Bip01-Pelvis");
	_matrix matWorld = pFrame_Derived->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	CEffect_Hit* pEffect_Hit = (CEffect_Hit*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Hit");

	if (pEffect_Hit == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Hit", L"GameObject_Effect_Hit")))
			return E_FAIL;

		pEffect_Hit = (CEffect_Hit*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Hit");
	}

	pEffect_Hit->Set_Position(*(_vec3*)matWorld.m[3], 3.f);

	return NOERROR;
}

HRESULT CSkeleton::GetHit(_vec3 vPosition, _int iPlayerDmg)
{
	if (m_iAnimation == SKELETON_DMG)
		m_pMeshCom->SetUp_AnimationSet(SKELETON_IDLE, m_fNewSpeed, m_Duration, m_Period);

	m_iAnimation = SKELETON_DMG;

	m_eCurState = HIT;

	m_fNewSpeed = 1.f;
	m_Duration = 0.2;
	m_Period = 0.09;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScaleRight = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScaleUp = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScaleLook = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScaleRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScaleUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScaleLook);

	m_tMonsterInfo.iCurHp -= iPlayerDmg;

	if (m_tMonsterInfo.iCurHp <= 0)
	{
		m_eCurState = DEAD;
		m_iAnimation = SKELETON_DEAD;
		m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);
	}

	// 여기서 맞는 이펙트
	D3DXFRAME_DERIVED* pFrame_Derived = m_pMeshCom->Get_FrameDerived("Bip01-Pelvis");
	_matrix matWorld = pFrame_Derived->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	CEffect_Hit* pEffect_Hit = (CEffect_Hit*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Hit");

	if (pEffect_Hit == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Hit", L"GameObject_Effect_Hit")))
			return E_FAIL;

		pEffect_Hit = (CEffect_Hit*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Hit");
	}

	pEffect_Hit->Set_Position(*(_vec3*)matWorld.m[3], 3.f);

	return NOERROR;
}

HRESULT CSkeleton::Follow_Player(_vec3 vPosition)
{
	m_iAnimation = SKELETON_RUN_F;
	m_eCurState = RUN;

	m_fNewSpeed = 1.f;
	m_Duration = 0.2;
	m_Period = 0.09;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScaleRight = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScaleUp = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScaleLook = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScaleRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScaleUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScaleLook);

	return NOERROR;
}

HRESULT CSkeleton::Set_Idle()
{
	m_iAnimation = SKELETON_IDLE;
	m_eCurState = IDLE;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

	return NOERROR;
}

HRESULT CSkeleton::Attack_Target(_vec3 vPosition)
{
	m_iAnimation = SKELETON_ATT;
	m_eCurState = ATT;

	m_fNewSpeed = 1.f;
	m_Duration = 0.2;
	m_Period = 0.09;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScaleRight = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScaleUp = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScaleLook = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScaleRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScaleUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScaleLook);

	return NOERROR;
}

HRESULT CSkeleton::Add_Component(void * pArg)
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
	if (CGameObject::Add_Component(SCENE_STAGE, L"Component_Mesh_Skeleton", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_HitBox
	CCollider::COLLIDER_DESC tHitDesc;
	tHitDesc.fRadius = 0.f;
	tHitDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tHitDesc.vLocalPosition = _vec3(0.f, 2.f, 0.f);
	tHitDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_HitBox", (CComponent**)&m_pHitColliderCom, &tHitDesc)))
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

HRESULT CSkeleton::SetUp_ConstantTable()
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

	if (m_eCurState == HIT || m_eCurState == DOWN)
	{
		if (FAILED(m_pShaderCom->Set_Bool("g_bRimMode", true)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_Bool("g_bRimMode", false)))
			return E_FAIL;
	}

	_matrix matCamera = m_pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixInverse(&matCamera, nullptr, &CManagement::GetInstance()->Get_Transform(D3DTS_VIEW));

	if (FAILED(m_pShaderCom->Set_Value("g_vCamPosition", &matCamera.m[3][0], sizeof(_vec4))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSkeleton::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; i++)
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

HRESULT CSkeleton::State_Machine(_double TimeDelta)
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

HRESULT CSkeleton::State_Idle(_double TimeDelta)
{
	//여기서 충돌처리요구
	_bool bIsTrue = false;
	if (FAILED(CCollisionMgr::GetInstance()->Collision_Monster_Detect_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", CCollider::WAY_BOXSPHERE, &bIsTrue)))
		return E_FAIL;
	if (bIsTrue)
		return NOERROR;

	m_iAnimation = SKELETON_IDLE;
	m_eCurState = IDLE;

	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CSkeleton::State_Run(_double TimeDelta)
{
	//STATE eState = RUN; // AttRange
	//if (FAILED(CColliderManager::GetInstance()->Collision_Check(g_eScene, CColliderManager::TYPE_MONSTER, m_pAttRangeColliderCom, CColliderManager::TYPE_PLAYER, L"Layer_Player", L"Com_HitBox", CColliderManager::TYPE_BOXSPHERE, &m_eType, this, &eState)))
	//	return E_FAIL;
	//if (m_eCurState != RUN)
	//	return NOERROR;

	//eState = IDLE; // Detect
	//if (FAILED(CColliderManager::GetInstance()->Collision_Check(g_eScene, CColliderManager::TYPE_MONSTER, m_pDetectColliderCom, CColliderManager::TYPE_PLAYER, L"Layer_Player", L"Com_HitBox", CColliderManager::TYPE_BOXSPHERE, &m_eType, this, &eState)))
	//	return E_FAIL;
	//if (m_eCurState != RUN)
	//	return NOERROR;

	_bool bIsTrue = false;

	if (FAILED(CCollisionMgr::GetInstance()->Collision_Monster_Attack_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &bIsTrue)))
		return E_FAIL;
	if (bIsTrue)
		return NOERROR;

	if (FAILED(CCollisionMgr::GetInstance()->Collision_Monster_Detect_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", CCollider::WAY_BOXSPHERE, &bIsTrue)))
		return E_FAIL;
	if (!bIsTrue)
		return NOERROR;

	_vec3 vPosition = *(_vec3*)(*(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX)).m[3];

	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	_float fScaleRight = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScaleUp = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	_float fScaleLook = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScaleRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScaleUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScaleLook);

	m_pTransformCom->Move_Forward(m_pNavigationCom, TimeDelta);

	return NOERROR;
}

HRESULT CSkeleton::State_Att(_double TimeDelta)
{
	if (m_pMeshCom->is_Finished())
	{
		m_eCurState = IDLE;
		m_iAnimation = SKELETON_IDLE;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;
	}

	return NOERROR;
}

HRESULT CSkeleton::State_Hit(_double TimeDelta)
{
	if (m_iAnimation == SKELETON_DMG && m_pMeshCom->is_Finished())
	{
		m_iAnimation = SKELETON_IDLE;
		m_eCurState = IDLE;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;
	}

	return NOERROR;
}

HRESULT CSkeleton::State_Down(_double TimeDelta)
{
	if (m_iAnimation == SKELETON_DOWN_START && !m_pMeshCom->is_Finished())
	{
		m_pTransformCom->Move_Backward(m_pNavigationCom, m_TimeDelta);

		return NOERROR;
	}

	if (m_iAnimation == SKELETON_DOWN_START && m_pMeshCom->is_Finished())
	{
		if (m_tMonsterInfo.iCurHp <= 0)
		{
			m_eCurState = DEAD;
			m_iAnimation = SKELETON_DOWN_LOOP;
			m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

			return NOERROR;
		}

		m_iAnimation = SKELETON_DOWN_LOOP;

		m_fNewSpeed = DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_iAnimation == SKELETON_DOWN_LOOP)
	{
		if(m_TimeDownAcc <= 1.5)
			m_TimeDownAcc += TimeDelta;
		else
		{
			m_TimeDownAcc = 0.0;
			m_iAnimation = SKELETON_GETUP;

			return NOERROR;
		}
	}

	if (m_iAnimation == SKELETON_GETUP && m_pMeshCom->is_Finished())
	{
		m_iAnimation = SKELETON_IDLE;
		m_eCurState = IDLE;

		m_Duration = 0.08;
		m_Period = 0.575;

		m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);
	}

	return NOERROR;
}

HRESULT CSkeleton::State_Groggy(_double TimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSkeleton::State_Dead(_double TimeDelta)
{
	if (m_iAnimation == SKELETON_DEAD_LOOP || m_iAnimation == SKELETON_DOWN_LOOP)
	{
		m_TimeDeadAcc += TimeDelta;
		if (m_TimeDeadAcc >= 2.0)
		{
			CParticle_Dead* pParticle = (CParticle_Dead*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Dead");

			if (pParticle == nullptr)
			{
				if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Particle_Dead", L"GameObject_Particle_Dead")))
					return E_FAIL;

				pParticle = (CParticle_Dead*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Dead");
			}

			_vec3 vOrigin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			vOrigin.y += 1.f;

			pParticle->Activate(vOrigin);

			m_pManagement->Push_GameObject(g_eScene, L"Layer_Monster", this);
		}
	}

	if (m_iAnimation == SKELETON_DEAD && m_pMeshCom->is_Finished())
		m_iAnimation = SKELETON_DEAD_LOOP;

	return NOERROR;
}

HRESULT CSkeleton::Update_Collider()
{
	m_pHitColliderCom->Update_Collider();
	m_pDmgColliderCom->Update_Collider();
	m_pDetectColliderCom->Update_Collider();
	m_pAttRangeColliderCom->Update_Collider();

	return NOERROR;
}

HRESULT CSkeleton::SetUp_OnNavigation()
{
	_float fHeight = m_pNavigationCom->SetUp_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = fHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

CSkeleton * CSkeleton::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkeleton* pInstance = new CSkeleton(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CSkeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkeleton::Clone_GameObject(void * pArg)
{
	CSkeleton* pInstance = new CSkeleton(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CSkeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkeleton::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pDmgColliderCom);
	Safe_Release(m_pDetectColliderCom);
	Safe_Release(m_pAttRangeColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CMonster::Free();
}
