#pragma once

#include "Camera.h"

BEGIN(Client)

class CCamera_Event final : public CCamera
{
private:
	explicit CCamera_Event(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Event(const CCamera_Event& rhs);
	virtual ~CCamera_Event() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Set_Target(CTransform* pTargetTransform);
	HRESULT Set_Position(_vec3 vPosition);
	HRESULT Move_Forward();
	HRESULT Move_Backward();
	HRESULT Move_Right();
	HRESULT Move_Left();
private:
	_double			m_TimeDelta = 0.0;
	CTransform*		m_pTargetTransform = nullptr;
public:
	static CCamera_Event* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END