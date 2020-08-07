#pragma once

#include "Base.h"
#include "Player.h"
#include "Terrain.h"
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CIOManager final : public CBase
{
	DECLARE_SINGLETON(CIOManager)
public:
	enum OBJECT_TYPE { TYPE_PLAYER, TYPE_MONSTER, TYPE_END };
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
public:
	virtual void Free();
};

END