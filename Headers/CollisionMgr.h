#pragma once

#include "Management.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMonster;

class CCollisionMgr final : public CBase
{
	DECLARE_SINGLETON(CCollisionMgr)
private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr() = default;
public:
	HRESULT Collision_Player_Att_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag,
		const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay);
	HRESULT Collision_Player_Tornado_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag,
		const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay);
	HRESULT Collision_Player_Shoulder_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag,
		const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay);
	HRESULT Collision_Player_Earthquake_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pShieldComponentTag, const _tchar* pPlayerComponentTag,
		const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay);
	HRESULT Collision_Monster_Detect_Player(_uint iSceneID, CMonster* pMonster,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, CCollider::COLLISIONWAY eWay, _bool* pIsTrue);
	HRESULT Collision_Monster_Attack_Player(_uint iSceneID, CMonster* pMonster,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pIsTrue);
private:
	CManagement*	m_pManagement = nullptr;
public:
	virtual void Free();
};

END