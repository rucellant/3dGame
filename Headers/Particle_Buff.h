#pragma once

#include "Particle.h"
#include "Client_Defines.h"

BEGIN(Client)

class CObserver_Player;

class CParticle_Buff final : public CParticle
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
		_float		fLifeTime;     // how long the particle lives for before dying  
		_float      fAge;          // current age of the particle  
		_float		fEmissive;
		_bool       bIsAlive;
	}ATTRIBUTE;
public:
	explicit CParticle_Buff(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CParticle_Buff(const CParticle_Buff& rhs);
	virtual ~CParticle_Buff() = default;
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
	static CParticle_Buff* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END