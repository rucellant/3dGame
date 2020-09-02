#pragma once

#include "Particle.h"
#include "Client_Defines.h"

BEGIN(Client)

class CObserver_Player;

class CParticle_Spread final : public CParticle
{
public:
	typedef struct tagAttribute
	{
		tagAttribute()
		{
			fLifeTime = 0.f;
			fAge = 0.0f;
			bIsAlive = true;
		}

		_vec3		vPosition;
		_vec3		vVelocity;
		_vec3		vAcceleration;
		_float		fLifeTime;     // how long the particle lives for before dying  
		_float      fAge;          // current age of the particle  
		_float		fRadius;
		_bool       bIsAlive;
	}ATTRIBUTE;
public:
	explicit CParticle_Spread(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CParticle_Spread(const CParticle_Spread& rhs);
	virtual ~CParticle_Spread() = default;
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
	static CParticle_Spread* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END