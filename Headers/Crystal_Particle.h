#pragma once

#include "GameObject.h"

BEGIN(Client)

class CCrystal_Particle final : public CGameObject
{
public:
	CCrystal_Particle(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCrystal_Particle(const CCrystal_Particle& rhs);
	virtual ~CCrystal_Particle() = default;
};

END