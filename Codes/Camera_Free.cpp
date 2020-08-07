#include "stdafx.h"
#include "..\Headers\Camera_Free.h"
#include "Management.h"


USING(Client)


CCamera_Free::CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
{

}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
{

}

HRESULT CCamera_Free::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCamera_Free::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(CCamera::Ready_GameObject_Clone(pArg)))
		return E_FAIL;

	CAMERADESC tCameraDesc = *(CAMERADESC*)pArg;

	if (FAILED(m_pManagement->Push_Camera(tCameraDesc.iSceneID, L"Camera_Free", this)))
		return E_FAIL;

	return NOERROR;
}

_int CCamera_Free::Update_GameObject(_double TimeDelta)
{
	if (m_pManagement->KeyPressing(KEY_LSHIFT))
	{
		if (m_pManagement->KeyPressing(KEY_W))
			m_pTransformCom->Move_Forward(TimeDelta);

		if (m_pManagement->KeyPressing(KEY_S))
			m_pTransformCom->Move_Backward(TimeDelta);

		if (m_pManagement->KeyPressing(KEY_A))
			m_pTransformCom->Move_Left(TimeDelta);

		if (m_pManagement->KeyPressing(KEY_D))
			m_pTransformCom->Move_Right(TimeDelta);

		_long MouseMove = 0;

		if (MouseMove = m_pManagement->Get_DIMMove(CInput_Device::DIM_X))
			m_pTransformCom->Rotation_Axis(&_vec3(0.f, 1.f, 0.f), TimeDelta * (MouseMove * 0.2f));

		if (MouseMove = m_pManagement->Get_DIMMove(CInput_Device::DIM_Y))
			m_pTransformCom->Rotation_Axis(&m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * (MouseMove * 0.2f));
	}

	return CCamera::Update_GameObject(TimeDelta);
}

_int CCamera_Free::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Free::Render_GameObject()
{
	return NOERROR;
}

CCamera_Free * CCamera_Free::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCamera_Free* pInstance = new CCamera_Free(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Free::Clone_GameObject(void * pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCamera_Free::Free()
{
	Safe_Release(m_pTransformCom);

	CCamera::Free();
}
