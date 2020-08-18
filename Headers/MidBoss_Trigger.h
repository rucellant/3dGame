#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Client)

class CMidBoss_Trigger final : public CGameObject
{
private:
	explicit CMidBoss_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMidBoss_Trigger(const CMidBoss_Trigger& rhs);
	virtual ~CMidBoss_Trigger() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
private:
	_bool				m_bIsActive = false;
private:
	HRESULT Add_Component(void* pArg);
public:
	static CMidBoss_Trigger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END