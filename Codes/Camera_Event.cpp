#include "stdafx.h"
#include "..\Headers\Camera_Event.h"
#include "Management.h"


USING(Client)


CCamera_Event::CCamera_Event(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{
}

CCamera_Event::CCamera_Event(const CCamera_Event & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Event::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCamera_Event::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(pArg)))
		return E_FAIL;

	CAMERADESC tCameraDesc = *(CAMERADESC*)pArg;

	if (FAILED(m_pManagement->Push_Camera(tCameraDesc.iSceneID, L"Camera_Event", this)))
		return E_FAIL;

	return NOERROR;
}

_int CCamera_Event::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta = TimeDelta;

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Event::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Event::Render_GameObject()
{
	return NOERROR;
}

HRESULT CCamera_Event::Set_Target(CTransform * pTargetTransform)
{
	if (pTargetTransform == nullptr)
		return E_FAIL;

	m_pTargetTransform = pTargetTransform;

	_vec3 vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);

	_vec3 vRight, vUp, vLook;

	vLook = vTargetPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, &_vec3(0.f, 1.f, 0.f), &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	return NOERROR;
}

HRESULT CCamera_Event::Set_Position(_vec3 vPosition)
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return NOERROR;
}

HRESULT CCamera_Event::Move_Forward()
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	return m_pTransformCom->Move_Forward(m_TimeDelta * 0.15);
}

HRESULT CCamera_Event::Move_Backward()
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	return m_pTransformCom->Move_Backward(m_TimeDelta * 0.15);
}

HRESULT CCamera_Event::Move_Right()
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	return m_pTransformCom->Move_Right(m_TimeDelta * 0.15);
}

HRESULT CCamera_Event::Move_Left()
{
	if (m_pTransformCom == nullptr)
		return E_FAIL;

	return m_pTransformCom->Move_Left(m_TimeDelta * 0.15);
}

CCamera_Event * CCamera_Event::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Event* pInstance = new CCamera_Event(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CCamera_Event");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Event::Clone_GameObject(void * pArg)
{
	CCamera_Event* pInstance = new CCamera_Event(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CCamera_Event");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Event::Free()
{
	Safe_Release(m_pTransformCom);

	CCamera::Free();
}
