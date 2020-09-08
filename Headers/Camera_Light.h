#pragma once

#include "Camera.h"

BEGIN(Client)

class CObserver_Player;

class CCamera_Light final : public CCamera
{
private:
	explicit CCamera_Light(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Light(const CCamera_Light& rhs);
	virtual ~CCamera_Light() = default;
public:
	_matrix GetViewMatrix() {
		return m_ViewMatrix; }
	_matrix GetViewMatrix_Inverse();
	_matrix GetProjMatrix() {
		return m_ProjMatrix; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CObserver_Player*		m_pObserver = nullptr;
public:
	static CCamera_Light* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END