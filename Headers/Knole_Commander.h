#pragma once

#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CKnole_Commander final : public CMonster
{
private:
	explicit CKnole_Commander(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CKnole_Commander(const CKnole_Commander& rhs);
	virtual ~CKnole_Commander() = default;
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
	static CKnole_Commander* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END