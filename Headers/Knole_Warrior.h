#pragma once

#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CKnole_Warrior final : public CMonster
{
private:
	explicit CKnole_Warrior(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CKnole_Warrior(const CKnole_Warrior& rhs);
	virtual ~CKnole_Warrior() = default;
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
	static CKnole_Warrior* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END