#pragma once

#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
private:
	explicit CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	static CCamera_Free* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END