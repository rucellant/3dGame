#include "stdafx.h"
#include "..\Headers\Balrog.h"
#include "Player.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Observer_Player.h"
#include "Observer_Balrog.h"


USING(Client)


CBalrog::CBalrog(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CBalrog::CBalrog(const CBalrog & rhs)
	: CMonster(rhs)
{
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
	tStateDesc.fSpeedPerSec = 10.f;

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

	m_pSubject = CSubject_Balrog::GetInstance();

	m_pSubject->AddData(CSubject_Balrog::TYPE_MATRIX, m_pTransformCom->Get_WorldMatrixPointer());
	m_pSubject->AddData(CSubject_Balrog::TYPE_INFO, &m_tMonsterInfo);
	m_pSubject->AddData(CSubject_Balrog::TYPE_STATE, &m_eCurState);

	m_pSubject->Notify(CSubject_Balrog::TYPE_INFO);
	m_pSubject->Notify(CSubject_Balrog::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Balrog::TYPE_STATE);

	m_bIsAlive = true;

	return NOERROR;
}

_int CBalrog::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	if (m_pManagement->KeyUp(KEY_LEFT))
		m_tMonsterInfo.iCurHp -= 100;

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

	// 서브젝트에게 정보 갱신함
	m_pSubject->Notify(CSubject_Balrog::TYPE_INFO);
	m_pSubject->Notify(CSubject_Balrog::TYPE_MATRIX);
	m_pSubject->Notify(CSubject_Balrog::TYPE_STATE);

	return _int();
}

_int CBalrog::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius) && m_bIsRender)
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CBalrog::Render_GameObject()
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

HRESULT CBalrog::GetReady()
{
	m_iAnimation = BALROG_GETREADY;

	m_pMeshCom->SetUp_AnimationSet(m_iAnimation);

	m_bIsRender = true;

	return NOERROR;
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
	tHitDesc.vLocalPosition = _vec3(0.f, 2.f, 0.f);
	tHitDesc.vLocalScale = _vec3(5.f, 5.f, 5.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_HitBox", (CComponent**)&m_pHitColliderCom, &tHitDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CBalrog::SetUp_ConstantTable()
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

	m_pHitColliderCom->Render_Collider();

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
	m_iAnimation = BALROG_IDLE;
	m_eCurState = IDLE;

	m_fNewSpeed = DEFAULT_ANIM_SPEED;
	m_Duration = DEFAULT_ANIM_DURATION;
	m_Period = DEFAULT_ANIM_PERIOD;

	return NOERROR;
}

HRESULT CBalrog::State_Run(_double TimeDelta)
{
	return NOERROR;
}

HRESULT CBalrog::State_Att(_double TimeDelta)
{
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
	return NOERROR;
}

HRESULT CBalrog::Update_Collider()
{
	m_pHitColliderCom->Update_Collider();

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CMonster::Free();
}
