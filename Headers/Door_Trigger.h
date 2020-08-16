#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CDoor_Trigger final : public CGameObject
{
private:
	explicit CDoor_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDoor_Trigger(const CDoor_Trigger& rhs);
	virtual ~CDoor_Trigger() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Register_GameObject(CGameObject* pGameObject);
private:
	list<CGameObject*>		m_ObjectLst;
private:
	_bool					m_bIsActive = false;
private:
	_double					m_TimeWait = 0.0;
public:
	static CDoor_Trigger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END