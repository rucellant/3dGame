#pragma once

#include "Particle.h"
#include "Client_Defines.h"

BEGIN(Client)

class CObserver_Player;

class CParticle_Cohesion final : public CParticle
{
public:
	typedef struct tagAttribute
	{
		_vec3		vDir;
		_vec3		vPosition;
		_vec3		vVelocity;
		_vec3		vAcceleration;
		_bool       bIsAlive;
	}ATTRIBUTE;
public:
	explicit CParticle_Cohesion(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CParticle_Cohesion(const CParticle_Cohesion& rhs);
	virtual ~CParticle_Cohesion() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Activate();
private:
	list<ATTRIBUTE>			m_ParticleLst;
	list<ATTRIBUTE>			m_RenderingParticleLst;
private:
	CObserver_Player*		m_pObserver = nullptr;
private:
	_double					m_TimeAcc = 0.0;
private:
	virtual HRESULT Add_Component(void* pArg);
	virtual HRESULT SetUp_ConstantTable();
	virtual HRESULT Render(_uint iPassIndex);
	virtual HRESULT Render_Particle();
public:
	static CParticle_Cohesion* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END