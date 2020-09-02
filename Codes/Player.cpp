#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Management.h"
#include "Effect_Hit.h"
#include "Effect_Buff.h"
#include "CollisionMgr.h"
#include "Particle_Buff.h"
#include "Subject_Player.h"
#include "Effect_Tornado.h"
#include "Effect_Shoulder.h"
#include "Effect_Earthquake.h"
#include "Particle_Cohesion.h"

USING(Client)

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPlayer::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(m_iAnimation)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&m_tObjDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&m_tObjDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&m_tObjDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(168.f, 55.f, -132.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(162.f, 28.f, -44.f));

	CTransform::STATEDESC tStateDesc;
	tStateDesc.fRotationPerSec = D3DXToRadian(90.f);
	tStateDesc.fSpeedPerSec = 20.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_pSubject = CSubject_Player::GetInstance();

	m_pSubject->AddData(CSubject_Player::TYPE_MATRIX, m_pTransformCom->Get_WorldMatrixPointer());
	m_pSubject->AddData(CSubject_Player::TYPE_INFO, &m_tPlayerInfo);
	m_pSubject->AddData(CSubject_Player::TYPE_RIGHTHAND, &m_matHandWorld[0]);
	m_pSubject->AddData(CSubject_Player::TYPE_LEFTHAND, &m_matHandWorld[1]);
	m_pSubject->AddData(CSubject_Player::TYPE_STATE, &m_eCurState);

	m_tPlayerInfo = m_tObjDesc.tPlayerInfo;

	m_pSubject->Notify(CSubject_Player::TYPE_INFO);
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Player::TYPE_RIGHTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_LEFTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_STATE);

	m_vSpringArm = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vec3(1.1f, 3.f, -6.f);

	m_pSpringArmCom->Set_State(CSpringArm::POSITION, m_vSpringArm);
	m_pSpringArmCom->Set_State(CSpringArm::TARGET, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	SetUp_OnNavigation();

	m_bIsAlive = true;

	return NOERROR;
}

_int CPlayer::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	m_TimeExp += TimeDelta;
	if (m_TimeExp >= 1.0)
		m_TimeExp = 0.0;

	/*if (m_pManagement->KeyUp(KEY_UP))
		m_TmpDuration += 0.01;
	if (m_pManagement->KeyUp(KEY_DOWN))
		m_TmpDuration -= 0.01;
	if (m_pManagement->KeyUp(KEY_RIGHT))
		m_TmpPeriod += 0.001;
	if (m_pManagement->KeyUp(KEY_LEFT))
		m_TmpPeriod -= 0.001;*/

	/*if (m_pManagement->KeyUp(KEY_UP))
	{
		m_iAnimation++;

		if (m_iAnimation > 52)
			m_iAnimation = 0;
	}*/

	if (m_bIsControl)
	{
		if (FAILED(State_Machine(TimeDelta)))
			return -1;
	}

	if (FAILED(Post_Update(TimeDelta)))
		return -1;

	return _int();
}

_int CPlayer::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CPlayer::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period)))
		return E_FAIL;

	if (FAILED(m_pMeshCom->Play_AnimationSet(m_TimeDelta)))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	//m_pHitColliderCom->Render_Collider();
	//m_pDmgColliderCom->Render_Collider();
	//m_pBottomColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CPlayer::Knockdown(_vec3 vPosition, _int iDmg)
{
	m_iAnimation = PLAYER_DOWN_START;

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

	m_tPlayerInfo.iCurHp -= iDmg;

	return NOERROR;
}

HRESULT CPlayer::SetUp_PlayerSK(CSK_Slot::SK_ID eID, _double Duration, _double Period)
{
	m_eCurState = SK;

	m_bIsSK = true;

	switch (eID)
	{
	case CSK_Slot::SK_TORNADO:
		m_Period = Period;
		m_Duration = Duration;
		m_iAnimation = PLAYER_TORNADO;
		break;
	case CSK_Slot::SK_SHOULDER:
		m_Period = Period;
		m_Duration = Duration;
		m_iAnimation = PLAYER_SHOULDER;
		break;
	case CSK_Slot::SK_EARTHQUAKE:
		m_Period = Period;
		m_Duration = Duration;
		m_iAnimation = PLAYER_EARTHQUAKE;
		break;
	case CSK_Slot::SK_SWORDWAVE:
		m_Period = Period;
		m_Duration = Duration;
		m_iAnimation = PLAYER_SWORDWAVE;
		break;
	case CSK_Slot::SK_BUFF:
		m_Period = Period;
		m_Duration = Duration;
		m_iAnimation = PLAYER_BUFFMOTION;
		break;
	}

	return NOERROR;
}

HRESULT CPlayer::GetHit(_int iMonsterDmg)
{
	if (m_tPlayerInfo.iCurHp - iMonsterDmg <= 0)
		m_tPlayerInfo.iCurHp = 1;
	else
		m_tPlayerInfo.iCurHp -= iMonsterDmg;

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

HRESULT CPlayer::Set_Position(_vec3 vPosition)
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return NOERROR;
}

HRESULT CPlayer::Set_Look(_vec3 vPosition)
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	vPosition.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScale = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	_vec3 vRight, vUp, vLook;

	vLook = vPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * fScale);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * fScale);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * fScale);

	return NOERROR;
}

HRESULT CPlayer::Set_Navigation_Mode(_uint eMode)
{
	if (m_pNavigationCom == nullptr)
		return E_FAIL;

	m_pNavigationCom->Set_Mode(CCell::MODE(eMode));

	return NOERROR;
}

HRESULT CPlayer::Update_PlayerInfo()
{
	if (m_pSubject == nullptr)
		return E_FAIL;

	m_pSubject->Notify(CSubject_Player::TYPE_INFO);
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Player::TYPE_RIGHTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_LEFTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_STATE);

	return NOERROR;
}

HRESULT CPlayer::Add_Component(void * pArg)
{
	m_tObjDesc = *(OBJDESC*)pArg;

	// For Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Player", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For. Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For. Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For. Com_SpringArm
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_SpringArm", L"Com_SpringArm", (CComponent**)&m_pSpringArmCom)))
		return E_FAIL;

	// For. Com_Navigation
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Navigation_Stage", L"Com_Navigation", (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	// For. Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Lups", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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
	tDmgDesc.fRadius = 5.f;
	tDmgDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDmgDesc.vLocalScale = _vec3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_DmgBox", (CComponent**)&m_pDmgColliderCom, &tDmgDesc)))
		return E_FAIL;

	// For. Com_BottomBox
	CCollider::COLLIDER_DESC tBottomDesc;
	tBottomDesc.fRadius = 0.f;
	tBottomDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tBottomDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tBottomDesc.vLocalScale = _vec3(5.f, 0.5f, 5.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_BottomBox", (CComponent**)&m_pBottomColliderCom, &tBottomDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayer::SetUp_ConstantTable(_uint iPassIndex)
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	if (iPassIndex == 0)
	{
		_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		_float fTimeExp = _float(m_TimeExp);
		if (FAILED(m_pShaderCom->Set_Value("g_fTimeExp", &fTimeExp, sizeof(_float))))
			return E_FAIL;

		return NOERROR;
	}

	return NOERROR;
}

HRESULT CPlayer::Render(_uint iPassIndex)
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

	//m_pNavigationCom->Render_Navigation();

	return NOERROR;
}

HRESULT CPlayer::State_Machine(_double TimeDelta)
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
	case SK:
		hr = State_SK(TimeDelta);
		break;
	case DOWN:
		hr = State_Down(TimeDelta);
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPlayer::State_Idle(_double TimeDelta)
{
	if (m_pManagement->KeyUp(KEY_LBUTTON))
	{
		m_iAnimation = PLAYER_ATT01;
		m_eCurState = ATT;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		//여기서 충돌처리요구
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Att_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
			return E_FAIL;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_W))
	{
		if (m_pManagement->KeyPressing(KEY_A))
		{
			m_pTransformCom->Move_FL(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_FL;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		if (m_pManagement->KeyPressing(KEY_D))
		{
			m_pTransformCom->Move_FR(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_FR;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		m_pTransformCom->Move_Forward(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_F;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_S))
	{
		if (m_pManagement->KeyPressing(KEY_A))
		{
			m_pTransformCom->Move_BL(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_BL;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		if (m_pManagement->KeyPressing(KEY_D))
		{
			m_pTransformCom->Move_BR(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_BR;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		m_pTransformCom->Move_Backward(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_B;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_A))
	{
		m_pTransformCom->Move_Left(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_L;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_D))
	{
		m_pTransformCom->Move_Right(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_R;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	m_iAnimation = PLAYER_IDLE;
	m_eCurState = IDLE;

	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CPlayer::State_Run(_double TimeDelta)
{
	if (m_pManagement->KeyUp(KEY_LBUTTON))
	{
		m_iAnimation = PLAYER_ATT01;
		m_eCurState = ATT;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		//여기서 충돌처리요구
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Att_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
			return E_FAIL;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_W))
	{
		if (m_pManagement->KeyPressing(KEY_A))
		{
			m_pTransformCom->Move_FL(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_FL;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		if (m_pManagement->KeyPressing(KEY_D))
		{
			m_pTransformCom->Move_FR(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_FR;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		m_pTransformCom->Move_Forward(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_F;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_S))
	{
		if (m_pManagement->KeyPressing(KEY_A))
		{
			m_pTransformCom->Move_BL(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_BL;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		if (m_pManagement->KeyPressing(KEY_D))
		{
			m_pTransformCom->Move_BR(m_pNavigationCom, TimeDelta);
			m_iAnimation = PLAYER_RUN_BR;
			m_eCurState = RUN;

			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}

		m_pTransformCom->Move_Backward(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_B;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_A))
	{
		m_pTransformCom->Move_Left(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_L;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_pManagement->KeyPressing(KEY_D))
	{
		m_pTransformCom->Move_Right(m_pNavigationCom, TimeDelta);
		m_iAnimation = PLAYER_RUN_R;
		m_eCurState = RUN;

		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	m_iAnimation = PLAYER_IDLE;
	m_eCurState = IDLE;

	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CPlayer::State_Att(_double TimeDelta)
{
	if (m_pManagement->KeyUp(KEY_LBUTTON))
	{
		if (m_iAnimation == PLAYER_ATT01 && !m_pMeshCom->is_Finished())
			m_iNextAnimation = PLAYER_ATT02;

		if (m_iAnimation == PLAYER_ATT02 && !m_pMeshCom->is_Finished())
			m_iNextAnimation = PLAYER_ATT03;
	}

	if (m_pMeshCom->is_Finished() && m_iNextAnimation != -1)
	{
		if (m_iNextAnimation == PLAYER_ATT02)
		{
			m_iAnimation = PLAYER_ATT02;
			m_iNextAnimation = -1;

			//여기서 충돌처리요구
			if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Att_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
				return E_FAIL;
		}

		if (m_iNextAnimation == PLAYER_ATT03)
		{
			m_iAnimation = PLAYER_ATT03;
			m_iNextAnimation = -1;

			m_Duration = 0.101;
			m_Period = 0.147;

			//여기서 충돌처리요구
			if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Att_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
				return E_FAIL;
		}

		return NOERROR;
	}

	if (m_pMeshCom->is_Finished() && m_iNextAnimation == -1)
	{
		m_iAnimation = PLAYER_IDLE;
		m_eCurState = IDLE;
		m_iNextAnimation = -1;

		m_Duration = 0.07;
		m_Period = 0.097;

		return NOERROR;
	}

	return NOERROR;
}

HRESULT CPlayer::State_SK(_double TimeDelta)
{
	// 1번 스킬 휘두르는 시각은 애니메이션 시작하고 0.55초 지난 후 부터 휘두름
	if (m_iAnimation == PLAYER_TORNADO)
	{
		if (!m_bIsTornadoParticle)
		{
			m_bIsTornadoParticle = !m_bIsTornadoParticle;

			if (FAILED(Create_Tornado_Particle()))
				return E_FAIL;
		}

		if (m_TimeTornadoAcc < 0.55)
			m_TimeTornadoAcc += TimeDelta;
		else
		{
			//여기서 충돌처리요구
			if (m_bIsSK)
			{
				if (FAILED(Create_Tornado()))
					return E_FAIL;
				
				if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Tornado_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
					return E_FAIL;
				m_bIsSK = false;
			}
		}
	}

	if (m_iAnimation == PLAYER_SHOULDER)
	{
		//여기서 충돌처리요구
		if (m_bIsSK)
		{
			if (FAILED(Create_Shoulder()))
				return E_FAIL;

			if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Shoulder_Monster(g_eScene, L"Layer_Player", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE)))
				return E_FAIL;
			m_bIsSK = false;
		}
	}

	if (m_iAnimation == PLAYER_BUFFMOTION)
	{
		//여기서 충돌처리요구
		if (m_bIsSK)
		{
			if (FAILED(Create_Buff()))
				return E_FAIL;

			if (FAILED(Create_Buff_Particle()))
				return E_FAIL;

			if (m_bIsBuff)
				m_TimeBuffAcc = 0.0;
			
			m_bIsBuff = true;

			m_bIsSK = false;
		}
	}

	if (m_iAnimation == PLAYER_EARTHQUAKE)
	{
		//여기서 충돌처리요구
		if (m_bIsSK)
		{
			if (FAILED(Create_Earthquake()))
				return E_FAIL;

			if (FAILED(CCollisionMgr::GetInstance()->Collision_Player_Earthquake_Monster(g_eScene, L"Layer_Player", L"Com_Collider", L"Com_DmgBox", L"Layer_Monster", L"Com_HitBox", CCollider::WAY_BOXSPHERE, &m_bIsSK)))
				return E_FAIL;
		}
	}
	
	if (m_pMeshCom->is_Finished())
	{
		m_iAnimation = PLAYER_IDLE;
		m_eCurState = IDLE;

		m_TimeTornadoAcc = 0.0;

		m_bIsSK = false;
		m_bIsEarthquake = false;
		m_bIsTornadoParticle = false;
	}

	return NOERROR;
}

HRESULT CPlayer::State_Down(_double TimeDelta)
{
	if (m_iAnimation == PLAYER_DOWN_START && !m_pMeshCom->is_Finished())
	{
		m_pTransformCom->Move_Backward(m_pNavigationCom, m_TimeDelta);

		return NOERROR;
	}

	if (m_iAnimation == PLAYER_DOWN_START && m_pMeshCom->is_Finished())
	{
		m_iAnimation = PLAYER_DOWN_LOOP;

		m_fNewSpeed = 0.5f;
		m_Duration = 0.2;
		m_Period = 0.09;

		return NOERROR;
	}

	if (m_iAnimation == PLAYER_DOWN_LOOP)
	{
		if (m_TimeDownAcc <= 1.5)
			m_TimeDownAcc += TimeDelta;
		else
		{
			m_TimeDownAcc = 0.0;
			m_iAnimation = PLAYER_GETUP;

			m_fNewSpeed = 0.5f;
			m_Duration = 0.2;
			m_Period = 0.15;

			return NOERROR;
		}
	}

	if (m_iAnimation == PLAYER_GETUP && m_pMeshCom->is_Finished())
	{
		m_iAnimation = PLAYER_IDLE;
		m_eCurState = IDLE;

		m_fNewSpeed = 1.f;
		m_Duration = 0.2;
		m_Period = 0.09;

		m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);
	}

	return NOERROR;
}

HRESULT CPlayer::Post_Update(_double TimeDelta)
{
	//콜라이더 업데이트
	Update_Collider();

	// 버프 시간 기록
	if (m_bIsBuff)
	{
		m_TimeBuffAcc += TimeDelta;

		if (m_TimeBuffAcc >= 60.0)
		{
			m_TimeBuffAcc = 0.0;
			m_bIsBuff = false;
		}
	}	

	// 마우스 가로축 회전 판별
	if (m_bIsControl && (m_eCurState != ATT))
	{
		_long MouseMove = 0;
		if (MouseMove = m_pManagement->Get_DIMMove(CInput_Device::DIM_X))
			m_pTransformCom->Rotation_Axis(&_vec3(0.f, 1.f, 0.f), MouseMove * (TimeDelta * 0.2f));
	}

	// 오른손 월드 행렬
	D3DXFRAME_DERIVED* pFrame_RightHand = m_pMeshCom->Get_FrameDerived("BN_Weapon_R");
	m_matHandWorld[0] = pFrame_RightHand->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	// 왼손 월드 행렬
	D3DXFRAME_DERIVED* pFrame_LeftHand = m_pMeshCom->Get_FrameDerived("BN_Weapon_L");
	m_matHandWorld[1] = pFrame_LeftHand->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	// 서브젝트에게 정보 갱신함
	m_pSubject->Notify(CSubject_Player::TYPE_INFO);
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Player::TYPE_RIGHTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_LEFTHAND);
	m_pSubject->Notify(CSubject_Player::TYPE_STATE);

	// 스프링암 관련
	Update_CameraPosition(TimeDelta);

	return NOERROR;
}

HRESULT CPlayer::Update_CameraPosition(_double TimeDelta)
{
	if (m_pSpringArmCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	_vec3 vSpringArm = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vec3 vAxisLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	D3DXVec3Normalize(&vAxisLook, &vAxisLook);
	vSpringArm += vAxisLook * -10.f;

	vSpringArm.y = m_vSpringArm.y;
	//vSpringArm.y = m_vSpringArm.y + m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	if (m_eCurState != ATT)
	{
		_long MouseMove = 0;
		if (MouseMove = m_pManagement->Get_DIMMove(CInput_Device::DIM_Y))
		{
			if (MouseMove < 0)
			{
				vSpringArm.y += _float(MouseMove * (TimeDelta * 0.4f));

				if (vSpringArm.y <= 1.f)
					vSpringArm.y = 1.f;
			}
			else
			{
				vSpringArm.y += _float(MouseMove * (TimeDelta * 0.4f));

				if (vSpringArm.y >= 6.f)
					vSpringArm.y = 6.f;
			}
		}
	}

	m_vSpringArm = vSpringArm;

	vSpringArm -= vAxisLook * -6.f;

	_vec3 vTarget = vSpringArm;
	vTarget.y = 3.f;

	_vec3 ResultPosition = m_vSpringArm + _vec3(0.f, m_pTransformCom->Get_State(CTransform::STATE_POSITION).y, 0.f);
	_vec3 ResultTarget = vTarget + _vec3(0.f, m_pTransformCom->Get_State(CTransform::STATE_POSITION).y, 0.f);

	m_pSpringArmCom->Set_State(CSpringArm::POSITION, ResultPosition);
	m_pSpringArmCom->Set_State(CSpringArm::TARGET, ResultTarget);

	/*m_pSpringArmCom->Set_State(CSpringArm::POSITION, m_vSpringArm);
	m_pSpringArmCom->Set_State(CSpringArm::TARGET, vTarget);*/

	return NOERROR;
}

HRESULT CPlayer::SetUp_OnNavigation()
{
	_float fHeight = m_pNavigationCom->SetUp_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = fHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

HRESULT CPlayer::Update_Collider()
{
	m_pHitColliderCom->Update_Collider();
	m_pDmgColliderCom->Update_Collider();
	m_pBottomColliderCom->Update_Collider();

	return NOERROR;
}

HRESULT CPlayer::Create_Tornado()
{
	CEffect_Tornado* pEffect_Tornado = (CEffect_Tornado*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Tornado");

	if (pEffect_Tornado == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Tornado", L"GameObject_Effect_Tornado")))
			return E_FAIL;
		
		pEffect_Tornado = (CEffect_Tornado*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Tornado");
	}

	if (pEffect_Tornado == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	pEffect_Tornado->Activate(m_TimeDelta);

	return NOERROR;
}

HRESULT CPlayer::Create_Shoulder()
{
	CEffect_Shoulder* pEffect_Shoulder = (CEffect_Shoulder*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Shoulder");

	if (pEffect_Shoulder == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Shoulder", L"GameObject_Effect_Shoulder")))
			return E_FAIL;

		pEffect_Shoulder = (CEffect_Shoulder*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Shoulder");
	}

	if (pEffect_Shoulder == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	pEffect_Shoulder->Activate();

	return NOERROR;
}

HRESULT CPlayer::Create_Earthquake()
{
	if (m_bIsEarthquake)
		return NOERROR;

	CEffect_Earthquake* pEffect_Earthquake = (CEffect_Earthquake*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Earthquake");

	if (pEffect_Earthquake == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Earthquake", L"GameObject_Effect_Earthquake")))
			return E_FAIL;

		pEffect_Earthquake = (CEffect_Earthquake*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Earthquake");
	}

	if (pEffect_Earthquake == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	pEffect_Earthquake->Activate();

	m_bIsEarthquake = true;

	return NOERROR;
}

HRESULT CPlayer::Create_Tornado_Particle()
{
	CParticle_Cohesion* pParticle = (CParticle_Cohesion*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Cohesion");

	if (pParticle == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Particle_Cohesion", L"GameObject_Particle_Cohesion")))
			return E_FAIL;

		pParticle = (CParticle_Cohesion*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Cohesion");
	}

	if (pParticle == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_RIGHTHAND);

	pParticle->Activate();

	return NOERROR;
}

HRESULT CPlayer::Create_Buff()
{
	CEffect_Buff* pEffect_Buff = (CEffect_Buff*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Buff");

	if (pEffect_Buff == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Buff", L"GameObject_Effect_Buff")))
			return E_FAIL;

		pEffect_Buff = (CEffect_Buff*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Buff");
	}

	if (pEffect_Buff == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	pEffect_Buff->Activate();

	return NOERROR;
}

HRESULT CPlayer::Create_Buff_Particle()
{
	CParticle_Buff* pParticle = (CParticle_Buff*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Buff");

	if (pParticle == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Particle_Buff", L"GameObject_Particle_Buff")))
			return E_FAIL;

		pParticle = (CParticle_Buff*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Buff");
	}

	if (pParticle == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	pParticle->Activate();

	return NOERROR;
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayer* pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pDmgColliderCom);
	Safe_Release(m_pBottomColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pSpringArmCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CSubject_Player::GetInstance()->DestroyInstance();

	CGameObject::Free();
}
