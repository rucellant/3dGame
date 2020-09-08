#include "stdafx.h"
#include "..\Headers\Camera_Light.h"
#include "Light.h"
#include "Player.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CCamera_Light::CCamera_Light(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{

}

CCamera_Light::CCamera_Light(const CCamera_Light & rhs)
	: CCamera(rhs)
{

}

_matrix CCamera_Light::GetViewMatrix_Inverse()
{
	return m_pTransformCom->Get_WorldMatrix_Inverse();
}

HRESULT CCamera_Light::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCamera_Light::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(pArg)))
		return E_FAIL;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	CAMERADESC tCameraDesc = *(CAMERADESC*)pArg;

	m_bIsAlive = true;

	return NOERROR;
}

_int CCamera_Light::Update_GameObject(_double TimeDelta)
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);

	_vec3 vPlayerPosition = *(_vec3*)&matTarget.m[3];

	_vec3 vRight, vUp, vLook, vPosition;

	vLook = vPlayerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	vUp = _vec3(0.f, 1.f, 0.f);

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	m_ViewMatrix = m_pTransformCom->Get_WorldMatrix_Inverse();

	return _int();
}

_int CCamera_Light::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Light::Render_GameObject()
{
	return NOERROR;
}

CCamera_Light * CCamera_Light::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Light* pInstance = new CCamera_Light(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CCamera_Light");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Light::Clone_GameObject(void * pArg)
{
	CCamera_Light* pInstance = new CCamera_Light(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CCamera_Light");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Light::Free()
{
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pObserver);

	CCamera::Free();
}
