#pragma once

#include "Camera.h"

BEGIN(Engine)
class CSpringArm;
END

BEGIN(Client)

class CObserver_Player;

class CCamera_Player final : public CCamera
{
private:
	explicit CCamera_Player(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Camera_Shake_On();
	HRESULT Camera_Shake_Off();
private:
	HRESULT GetRandVec(_vec3* vOut, _vec3* vMin, _vec3* vMax);
	_float GetRandFloat(_float fLowBound, _float fHighBound);
private:
	_bool		m_bIsShake = false;
	_bool		m_bIsMovingShake = false;
	_int		m_iShakeDir = 0;
	_double		m_ShakeTimeAcc = 0.0;
public:
	static CCamera_Player* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END