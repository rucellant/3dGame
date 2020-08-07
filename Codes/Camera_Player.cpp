#include "stdafx.h"
#include "..\Headers\Camera_Player.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CCamera_Player::CCamera_Player(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCamera_Player::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(pArg)))
		return E_FAIL;

	CAMERADESC tCameraDesc = *(CAMERADESC*)pArg;

	if (FAILED(m_pManagement->Push_Camera(tCameraDesc.iSceneID, L"Camera_Player", this)))
		return E_FAIL;

	return NOERROR;
}

_int CCamera_Player::Update_GameObject(_double TimeDelta)
{
	CSpringArm* pPlayerSpringArm = (CSpringArm*)m_pManagement->Get_Component(SCENE_STAGE, L"Layer_Player", L"Com_SpringArm");

	_vec3 vCameraPosition = pPlayerSpringArm->Get_State().vCameraPosition;
	_vec3 vCameraTarget = pPlayerSpringArm->Get_State().vCameraTarget;

	_vec3 vRight, vUp, vLook, vPosition;

	vLook = vCameraTarget - vCameraPosition;
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	_vec3 vInterpolationPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) * (1.f - (TimeDelta * 10.f)) + vCameraPosition * (TimeDelta * 10.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInterpolationPosition);

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Player::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Player::Render_GameObject()
{
	return NOERROR;
}

CCamera_Player * CCamera_Player::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Player* pInstance = new CCamera_Player(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Player::Clone_GameObject(void * pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CCamera_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	Safe_Release(m_pTransformCom);

	CCamera::Free();
}
