#pragma once

#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBabegazi_Axe final : public CMonster
{
private:
	explicit CBabegazi_Axe(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBabegazi_Axe(const CBabegazi_Axe& rhs);
	virtual ~CBabegazi_Axe() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
protected:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CBabegazi_Axe* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END