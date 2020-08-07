#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Management.h"
#include "Subject_Player.h"

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

	if (FAILED(m_pMeshCom->SetUp_AnimationSet(47)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&m_tObjDesc.matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&m_tObjDesc.matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&m_tObjDesc.matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	CTransform::STATEDESC tStateDesc;
	tStateDesc.fRotationPerSec = D3DXToRadian(90.f);
	tStateDesc.fSpeedPerSec = 20.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_pSubject = CSubject_Player::GetInstance();

	m_pSubject->AddData(CSubject_Player::TYPE_MATRIX, m_pTransformCom->Get_WorldMatrixPointer());
	m_pSubject->AddData(CSubject_Player::TYPE_INFO, &m_tPlayerInfo);

	m_pSubject->Notify(CSubject_Player::TYPE_INFO);
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	m_vSpringArm = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + _vec3(1.1f, 3.f, -6.f);

	m_pSpringArmCom->Set_State(CSpringArm::POSITION, m_vSpringArm);
	m_pSpringArmCom->Set_State(CSpringArm::TARGET, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_bIsAlive = true;

	return NOERROR;
}

_int CPlayer::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	_long MouseMove = 0;
	if (MouseMove = m_pManagement->Get_DIMMove(CInput_Device::DIM_X))
		m_pTransformCom->Rotation_Axis(&_vec3(0.f, 1.f, 0.f), MouseMove * (TimeDelta * 0.2f));

	m_pSubject->Notify(CSubject_Player::TYPE_INFO);
	m_pSubject->Notify(CSubject_Player::TYPE_MATRIX);

	Update_CameraPosition(TimeDelta);

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

	if (FAILED(m_pMeshCom->Play_AnimationSet(m_TimeDelta)))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

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

	return NOERROR;
}

HRESULT CPlayer::SetUp_ConstantTable()
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

	m_pNavigationCom->Render_Navigation();

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

	m_vSpringArm = vSpringArm;

	vSpringArm -= vAxisLook * -6.f;

	_vec3 vTarget = vSpringArm;
	vTarget.y = 3.f;

	m_pSpringArmCom->Set_State(CSpringArm::POSITION, m_vSpringArm);
	m_pSpringArmCom->Set_State(CSpringArm::TARGET, vTarget);

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pSpringArmCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMeshCom);

	CSubject_Player::GetInstance()->DestroyInstance();

	CGameObject::Free();
}
