#pragma once

#include "Particle.h"
#include "Client_Defines.h"

BEGIN(Client)

class CParticle_Dead final : public CParticle
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

		_vec3		vDir;
		_vec3		vPosition;
		_vec3		vVelocity;
		_vec3		vAcceleration;
		_float		fLifeTime;     // how long the particle lives for before dying  
		_float      fAge;          // current age of the particle  
		D3DXCOLOR   vCurColor;     // current color of the particle   
		D3DXCOLOR   vColorFade;    // how the color fades with respect to time
		_bool       bIsAlive;
	}ATTRIBUTE;
public:
	explicit CParticle_Dead(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CParticle_Dead(const CParticle_Dead& rhs);
	virtual ~CParticle_Dead() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Activate(_vec3 vOrigin);
private:
	list<ATTRIBUTE>			m_ParticleLst;
	list<ATTRIBUTE>			m_RenderingParticleLst;
private:
	_double					m_TimeAcc = 0.0;
private:
	virtual HRESULT Add_Component(void* pArg);
	virtual HRESULT SetUp_ConstantTable();
	virtual HRESULT Render(_uint iPassIndex);
	virtual HRESULT Render_Particle();
public:
	static CParticle_Dead* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END