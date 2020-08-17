#pragma once

#include "Management.h"
#include "Client_Defines.h"

BEGIN(Client)

class CIcicle;
class CCrystal;
class CMonster;
class CObserver_Player;

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
	HRESULT Collision_Crystal_Player(_uint iSceneID, CCrystal* pCrystal,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pIsTrue);
	HRESULT Collision_Icicle_Player(_uint iSceneID, CIcicle* pIcicle,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pMode);
private:
	CManagement*		m_pManagement = nullptr;
private:
	CObserver_Player*	m_pObserver = nullptr;
public:
	virtual void Free();
};

END