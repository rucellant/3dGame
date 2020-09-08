#include "stdafx.h"
#include "..\Headers\Balrog.h"
#include "Player.h"
#include "Management.h"
#include "Effect_Hit.h"
#include "CollisionMgr.h"
#include "Camera_Light.h"
#include "Observer_Boss.h"
#include "Effect_Breath.h"
#include "Camera_Player.h"
#include "Observer_Player.h"


USING(Client)


CBalrog::CBalrog(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CBalrog::CBalrog(const CBalrog & rhs)
	: CMonster(rhs)
{
}

_matrix CBalrog::GetWorldMatrix()
{
	_matrix matTmp;
	D3DXMatrixIdentity(&matTmp);

	if (m_pTransformCom == nullptr)
		return matTmp;

	return m_pTransformCom->Get_WorldMatrix();
}

HRESULT CBalrog::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CBalrog::Ready_GameObject_Clone(void * pArg)
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
	tStateDesc.fSpeedPerSec = 15.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	SetUp_OnNavigation();

	m_iAnimation = BALROG_IDLE;

	m_eCurState = IDLE;

	m_eType = TYPE_BALROG;

	m_tMonsterInfo.iCurHp = 4000;
	m_tMonsterInfo.iMaxHp = 4000;
	m_tMonsterInfo.iMaxDmg = 50;
	m_tMonsterInfo.iMinDmg = 100;
	m_tMonsterInfo.iGold = 100;

	m_pSubject = CSubject_Boss::GetInstance();

	D3DXFRAME_DERIVED* pFrame_Derived = m_pMeshCom->Get_FrameDerived("Bip01-Head");
	m_matMouse = pFrame_Derived->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	m_pSubject->AddData(CSubject_Boss::TYPE_MATRIX, m_pTransformCom->Get_WorldMatrixPointer());
	m_pSubject->AddData(CSubject_Boss::TYPE_INFO, &m_tMonsterInfo);
	m_pSubject->AddData(CSubject_Boss::TYPE_STATE, &m_eCurState);
	m_pSubject->AddData(CSubject_Boss::TYPE_MOUSE, &m_matMouse);

	m_pSubject->Notify(CSubject_Boss::TYPE_INFO);
	m_pSubject->Notify(CSubject_Boss::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Boss::TYPE_STATE);
	m_pSubject->Notify(CSubject_Boss::TYPE_MOUSE);

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_bIsAlive = true;

	return NOERROR;
}

_int CBalrog::Update_GameObject(_double TimeDelta)
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

	if (m_bIsActive)
	{
		if (FAILED(State_Machine(TimeDelta)))
			return E_FAIL;
	}
	else
	{
		if (m_pMeshCom->is_Finished() && m_iAnimation == BALROG_GETREADY)
		{
			m_bIsActive = true;
			m_pManagement->Play_Camera(g_eScene, L"Camera_Player");

			((CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player"))->SetIsControl(true);

			list<CGameObject*>* pUILayer = m_pManagement->Get_Layer(g_eScene, L"Layer_UI");

			for (auto& pUI : *pUILayer)
				m_pManagement->Pop_GameObject(g_eScene, L"Layer_UI");

			/*for (auto& pUI : *pUILayer)
				m_pManagement->Push_GameObject(g_eScene, L"Layer_UI", pUI);*/
		}
	}

	Update_Collider();

	D3DXFRAME_DERIVED* pFrame_Derived = m_pMeshCom->Get_FrameDerived("Bip01-Head");
	m_matMouse = pFrame_Derived->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMatrix();

	// 서브젝트에게 정보 갱신함
	m_pSubject->Notify(CSubject_Boss::TYPE_INFO);
	m_pSubject->Notify(CSubject_Boss::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Boss::TYPE_STATE);
	m_pSubject->Notify(CSubject_Boss::TYPE_MOUSE);

	return _int();
}

_int CBalrog::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius) && m_bIsRender)
	{
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);
	}

	return _int();
}

HRESULT CBalrog::Render_GameObject()
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
		if (FAILED(m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period)))
			return E_FAIL;

		if (FAILED(m_pMeshCom->Play_AnimationSet(m_TimeDelta)))
			return E_FAIL;

		if (FAILED(SetUp_ConstantTable(1)))
			return E_FAIL;

		if (FAILED(Render(0)))
			return E_FAIL;

		m_iRenderIndex = 0;
	}

	return NOERROR;
}

HRESULT CBalrog::GetReady()
{
	m_iAnimation = BALROG_GETREADY;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation);

	m_bIsRender = true;

	return NOERROR;
}

HRESULT CBalrog::GetHit(_vec3 vPosition, _int iPlayerDmg)
{
	_bool bIsBuff = *(_bool*)m_pObserver->GetData(CSubject_Player::TYPE_BUFF);

	if (bIsBuff)
		iPlayerDmg = _int(iPlayerDmg * 1.3f);

	m_tMonsterInfo.iCurHp -= _int(iPlayerDmg);

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

	if (m_tMonsterInfo.iCurHp <= 0)
	{
		m_iAnimation = BALROG_DEAD;
		m_eCurState = DEAD;

		m_fNewSpeed = DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		m_pMeshCom->SetUp_AnimationSet(m_iAnimation, m_fNewSpeed, m_Duration, m_Period);

		return NOERROR;
	}

	return NOERROR;
}

HRESULT CBalrog::Attack_Target(_vec3 vPosition)
{
	return E_NOTIMPL;
}

HRESULT CBalrog::Add_Component(void * pArg)
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
	if (CGameObject::Add_Component(SCENE_STAGE, L"Component_Mesh_Balrog", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_HitBox
	CCollider::COLLIDER_DESC tHitDesc;
	tHitDesc.fRadius = 0.f;
	tHitDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tHitDesc.vLocalPosition = _vec3(0.f, 5.f, -10.f);
	tHitDesc.vLocalScale = _vec3(15.f, 10.f, 15.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_HitBox", (CComponent**)&m_pHitColliderCom, &tHitDesc)))
		return E_FAIL;

	// For. Com_DmgOneBox
	CCollider::COLLIDER_DESC tDmgOneDesc;
	tDmgOneDesc.fRadius = 0.f;
	tDmgOneDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgOneDesc.vLocalPosition = _vec3(-5.f, 5.f, 50.f);
	tDmgOneDesc.vLocalScale = _vec3(10.f, 10.f, 75.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_DmgOneBox", (CComponent**)&m_pDmgOneColliderCom, &tDmgOneDesc)))
		return E_FAIL;

	// For. Com_DmgTwoBox
	CCollider::COLLIDER_DESC tDmgTwoDesc;
	tDmgTwoDesc.fRadius = 5.f;
	tDmgTwoDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgTwoDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDmgTwoDesc.vLocalScale = _vec3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_DmgTwoBox", (CComponent**)&m_pDmgTwoColliderCom, &tDmgTwoDesc)))
		return E_FAIL;

	// For. Com_OuterIntersectBox
	CCollider::COLLIDER_DESC tOuterIntersectDesc;
	tOuterIntersectDesc.fRadius = 22.f;
	tOuterIntersectDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tOuterIntersectDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tOuterIntersectDesc.vLocalScale = _vec3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_OuterIntersectBox", (CComponent**)&m_pOuterIntersectColliderCom, &tOuterIntersectDesc)))
		return E_FAIL;

	// For. Com_InnerIntersectBox
	CCollider::COLLIDER_DESC tInnerIntersectDesc;
	tInnerIntersectDesc.fRadius = 4.f;
	tInnerIntersectDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tInnerIntersectDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tInnerIntersectDesc.vLocalScale = _vec3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_Sphere", L"Com_InnerIntersectBox", (CComponent**)&m_pInnerIntersectColliderCom, &tInnerIntersectDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBalrog::SetUp_ConstantTable(_uint iRenderIndex)
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

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		if (m_eCurState == ATT)
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
	}

	return NOERROR;
}

HRESULT CBalrog::Render(_uint iPassIndex)
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

	//m_pHitColliderCom->Render_Collider();
	//m_pDmgOneColliderCom->Render_Collider();
	//m_pDmgTwoColliderCom->Render_Collider();
	//m_pOuterIntersectColliderCom->Render_Collider();
	//m_pInnerIntersectColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CBalrog::LookAtPlayer()
{
	_matrix matPlayer = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);

	matPlayer.m[3][1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

	_float fScale = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	_vec3 vRight, vUp, vLook;

	vLook = *(_vec3*)&matPlayer.m[3] - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
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

HRESULT CBalrog::State_Machine(_double TimeDelta)
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

HRESULT CBalrog::State_Idle(_double TimeDelta)
{
	if (FAILED(LookAtPlayer()))
		return E_FAIL;

	if ((m_iAttCount == 3 && m_TimeWaitAcc <= 5.0) || m_bIsBreath && m_TimeWaitAcc <= 5.0)
	{
		m_TimeWaitAcc += TimeDelta;
		return NOERROR;
	}
	else
	{
		m_TimeWaitAcc = 0.0;
		m_bIsBreath = false;
	}

	// 아우터 인터섹트와 충돌 검사 -> 충돌 안하면 플레이어한테 걸어 감
	_bool bIsResult = false;
	if (FAILED(CCollisionMgr::GetInstance()->Collision_Boss_Intersect_Player(g_eScene, this, L"Com_OuterIntersectBox", L"Layer_Player", L"Com_HitBox", &bIsResult)))
		return E_FAIL;

	if (!bIsResult)
	{
		m_iAnimation = BALROG_RUN_F;
		m_eCurState = RUN;

		m_fNewSpeed = DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	// 이너 인터섹트와 충돌 검사 -> 충돌 하면 발차기 함
	if (FAILED(CCollisionMgr::GetInstance()->Collision_Boss_Intersect_Player(g_eScene, this, L"Com_InnerIntersectBox", L"Layer_Player", L"Com_HitBox", &bIsResult)))
		return E_FAIL;

	if (bIsResult)
	{
		m_iAnimation = BALROG_ATT02;
		m_eCurState = ATT;

		m_fNewSpeed = 2.f;//DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		return NOERROR;
	}

	if (m_iAttCount < 3)
	{
		m_iAnimation = BALROG_ATT01;
		m_eCurState = ATT;

		m_fNewSpeed = 2.f;//DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		m_iAttCount++;

		/*_bool bIsTrue = false;
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Monster_Attack_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &bIsTrue)))
			return E_FAIL;*/

		return NOERROR;
	}

	if (m_iAttCount >= 3)
	{
		m_iAnimation = BALROG_BREATH;
		m_eCurState = ATT;

		m_fNewSpeed = DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		m_iAttCount = 0;

		m_bIsBreath = true;

		return NOERROR;
	}

	////

	m_iAnimation = BALROG_IDLE;
	m_eCurState = IDLE;

	m_fNewSpeed = DEFAULT_ANIM_SPEED;
	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CBalrog::State_Run(_double TimeDelta)
{
	// 아우터 인터섹트와 충돌 검사 -> 충돌 안하면 플레이어한테 걸어 감
	_bool bIsResult = false;
	if (FAILED(CCollisionMgr::GetInstance()->Collision_Boss_Intersect_Player(g_eScene, this, L"Com_OuterIntersectBox", L"Layer_Player", L"Com_HitBox", &bIsResult)))
		return E_FAIL;

	if (bIsResult)
	{
		if (m_iAttCount < 3)
		{
			m_iAnimation = BALROG_ATT01;
			m_eCurState = ATT;

			m_fNewSpeed = DEFAULT_ANIM_SPEED;
			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}
		else
		{
			m_iAnimation = BALROG_BREATH;
			m_eCurState = ATT;

			m_fNewSpeed = DEFAULT_ANIM_SPEED;
			m_Duration = DEFAULT_ANIM_DURATION;
			m_Period = DEFAULT_ANIM_PERIOD;

			return NOERROR;
		}
	}

	if (FAILED(LookAtPlayer()))
		return E_FAIL;

	m_pTransformCom->Move_Forward(TimeDelta);

	m_iAnimation = BALROG_RUN_F;
	m_eCurState = RUN;

	m_fNewSpeed = DEFAULT_ANIM_SPEED;
	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CBalrog::State_Att(_double TimeDelta)
{
	if (!m_pMeshCom->is_Finished() && m_iAnimation == BALROG_ATT01)
	{
		m_TimeAnimationAcc += TimeDelta;

		if (m_TimeAnimationAcc >= 1.0 && !m_bIsAtt)
		{
			if (FAILED(CCollisionMgr::GetInstance()->Collision_Monster_Attack_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &m_bIsAtt)))
				return E_FAIL;

			// 카메라 쉐이크 온
			CCamera_Player* pCamera = (CCamera_Player*)m_pManagement->Get_Camera(g_eScene, L"Camera_Player");

			pCamera->Camera_Shake_On();
		}

		

		return NOERROR;
	}

	if (!m_pMeshCom->is_Finished() && m_iAnimation == BALROG_ATT02)
	{
		m_TimeAnimationAcc += TimeDelta;

		if (m_TimeAnimationAcc >= 1.0 && !m_bIsAtt)
		{
			if (FAILED(CCollisionMgr::GetInstance()->Collision_Boss_Knockback_Player(g_eScene, this, L"Com_InnerIntersectBox", L"Com_HitBox", &m_bIsAtt)))
				return E_FAIL;
		}

		return NOERROR;
	}

	if (!m_pMeshCom->is_Finished() && m_iAnimation == BALROG_BREATH)
	{
		m_TimeBreathStartAcc += TimeDelta;

		if (m_TimeBreathStartAcc >= 3.0)
		{
			m_TimeBreathAcc += TimeDelta;

			if (m_TimeBreathAcc >= 0.1)
			{
				if (FAILED(Create_Breath()))
					return E_FAIL;

				m_TimeBreathAcc = 0.0;
			}
		}
	}

	if (m_pMeshCom->is_Finished())
	{
		m_iAnimation = BALROG_IDLE;
		m_eCurState = IDLE;

		m_fNewSpeed = DEFAULT_ANIM_SPEED;
		m_Duration = DEFAULT_ANIM_DURATION;
		m_Period = DEFAULT_ANIM_PERIOD;

		m_bIsAtt = false;
		m_TimeAnimationAcc = 0.0;
		m_TimeBreathStartAcc = 0.0;

		// 카메라 쉐이크 오프
		CCamera_Player* pCamera = (CCamera_Player*)m_pManagement->Get_Camera(g_eScene, L"Camera_Player");

		pCamera->Camera_Shake_Off();

		return NOERROR;
	}

	return NOERROR;
}

HRESULT CBalrog::State_Hit(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CBalrog::State_Down(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CBalrog::State_Groggy(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CBalrog::State_Dead(_double TimeDelta)
{
	if (m_pMeshCom->is_Finished() && m_iAnimation == BALROG_DEAD)
		m_iAnimation = BALROG_DEAD_LOOP;

	if (m_iAnimation == BALROG_DEAD_LOOP)
	{
		m_TimeDeadAcc += TimeDelta;

		if (m_TimeDeadAcc >= 5.0)
			m_pManagement->Push_GameObject(g_eScene, L"Layer_Monster", this);
	}

	return NOERROR;
}

HRESULT CBalrog::Update_Collider()
{
	m_pHitColliderCom->Update_Collider();
	m_pDmgOneColliderCom->Update_Collider();
	m_pDmgTwoColliderCom->Update_Collider();
	m_pOuterIntersectColliderCom->Update_Collider();
	m_pInnerIntersectColliderCom->Update_Collider();

	return NOERROR;
}

HRESULT CBalrog::SetUp_OnNavigation()
{
	_float fHeight = m_pNavigationCom->SetUp_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition.y = fHeight;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

HRESULT CBalrog::Create_Breath()
{
	CEffect_Breath* pEffect_Breath = (CEffect_Breath*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Breath");

	if (pEffect_Breath == nullptr)
	{
		if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Effect_Breath", L"GameObject_Effect_Breath")))
			return E_FAIL;

		pEffect_Breath = (CEffect_Breath*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Effect_Breath");
	}

	if (pEffect_Breath == nullptr)
		return E_FAIL;

	// 토네이도 실행하기 전에 정보부터 넘겨줌 
	m_pSubject->Notify(CSubject_Boss::TYPE_MOUSE);

	pEffect_Breath->Activate();

	return NOERROR;
}

CBalrog * CBalrog::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBalrog* pInstance = new CBalrog(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CBalrog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBalrog::Clone_GameObject(void * pArg)
{
	CBalrog* pInstance = new CBalrog(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CBalrog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalrog::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pDmgOneColliderCom);
	Safe_Release(m_pDmgTwoColliderCom);
	Safe_Release(m_pOuterIntersectColliderCom);
	Safe_Release(m_pInnerIntersectColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);
		
	CMonster::Free();
}
