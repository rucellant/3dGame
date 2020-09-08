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

	CCamera::Update_GameObject(0.0);

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

	_vec3 vInterpolationPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION) * (1.f - _float(TimeDelta * 10.f)) + vCameraPosition * _float(TimeDelta * 10.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInterpolationPosition);

	// 쉐이크 모드면
	if (m_bIsShake)
	{
		if (!m_bIsMovingShake) // 현재 어느 방향으로 흔들기가 진행되고 있지 않으면
		{
			m_iShakeDir = rand() % 4;

			m_bIsMovingShake = true;
		}
		else // 흔드는 중 
		{
			switch (m_iShakeDir)
			{
			case 0:
				m_pTransformCom->Move_Right(TimeDelta);
				break;
			case 1:
				m_pTransformCom->Move_Left(TimeDelta);
				break;
			case 2:
				m_pTransformCom->Move_Up(TimeDelta);
				break;
			case 3:
				m_pTransformCom->Move_Down(TimeDelta);
				break;
			}

			m_ShakeTimeAcc += TimeDelta;

			if (m_ShakeTimeAcc >= 0.001)
			{
				m_ShakeTimeAcc = 0.0;
				m_bIsMovingShake = false;
			}
		}
	}

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

HRESULT CCamera_Player::Camera_Shake_On()
{
	if (m_bIsShake)
		return NOERROR;

	m_bIsShake = true;

	m_iShakeDir = 0;
	m_ShakeTimeAcc = 0.0;

	return NOERROR;
}

HRESULT CCamera_Player::Camera_Shake_Off()
{
	if (!m_bIsShake)
		return NOERROR;

	m_bIsShake = false;

	m_iShakeDir = 0;
	m_ShakeTimeAcc = 0.0;

	return NOERROR;
}

HRESULT CCamera_Player::GetRandVec(_vec3 * vOut, _vec3 * vMin, _vec3 * vMax)
{
	if (vOut == nullptr || vMin == nullptr || vMax == nullptr)
		return E_FAIL;

	vOut->x = GetRandFloat(vMin->x, vMax->x);
	vOut->y = GetRandFloat(vMin->y, vMax->y);
	vOut->z = GetRandFloat(vMin->z, vMax->z);

	return NOERROR;
}

_float CCamera_Player::GetRandFloat(_float fLowBound, _float fHighBound)
{
	if (fLowBound >= fHighBound) // bad input
		return fLowBound;

	// get random float in [0, 1] interval
	_float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (fHighBound - fLowBound)) + fLowBound;
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
