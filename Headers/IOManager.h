#pragma once

#include "Base.h"
#include "Icicle.h"
#include "Player.h"
#include "Balrog.h"
#include "Twister.h"
#include "Crystal.h"
#include "Terrain.h"
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CIOManager final : public CBase
{
	DECLARE_SINGLETON(CIOManager)
public:
	enum OBJECT_TYPE { TYPE_PLAYER, TYPE_MONSTER, TYPE_INTERACT, TYPE_ICICLE, TYPE_TWISTER, TYPE_BALROG, TYPE_END };
private:
	explicit CIOManager();
	virtual ~CIOManager() = default;
public:
	HRESULT Store(OBJECT_TYPE eType, void* pArg);
	void* Load(OBJECT_TYPE eType);
	HRESULT Clear();
private:
	unordered_map<OBJECT_TYPE, vector<CMonster::OBJDESC>> m_mapMonsterDesc;
	unordered_map<OBJECT_TYPE, vector<CPlayer::OBJDESC>> m_mapWitchBlade;
	unordered_map<OBJECT_TYPE, vector<CCrystal::OBJDESC>> m_mapInteract;
	unordered_map<OBJECT_TYPE, vector<CIcicle::OBJDESC>> m_mapIcicle;
	unordered_map<OBJECT_TYPE, vector<CTwister::OBJDESC>> m_mapTwister;
	unordered_map<OBJECT_TYPE, vector<CBalrog::OBJDESC>> m_mapBalrog;
public:
	virtual void Free();
};

END