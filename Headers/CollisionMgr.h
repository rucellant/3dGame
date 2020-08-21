#pragma once

#include "Management.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayer;
class CIcicle;
class CCrystal;
class CMonster;
class CTwister;
class CMidBoss_Trigger;
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
		const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay, _bool* pIsSK = nullptr);
	HRESULT Collision_Monster_Detect_Player(_uint iSceneID, CMonster* pMonster,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, CCollider::COLLISIONWAY eWay, _bool* pIsTrue);
	HRESULT Collision_Monster_Attack_Player(_uint iSceneID, CMonster* pMonster,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pIsTrue);
	HRESULT Collision_Crystal_Player(_uint iSceneID, CCrystal* pCrystal,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pIsTrue);
	HRESULT Collision_Icicle_Player(_uint iSceneID, CIcicle* pIcicle,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pMode);
	HRESULT Collision_Twister_Player(_uint iSceneID, CTwister* pTwister,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag);
	HRESULT Collision_MidBoss_Trigger_Player(_uint iSceneID, CMidBoss_Trigger* pTrigger,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag);
	HRESULT Collision_Boss_Intersect_Player(_uint iSceneID, CMonster* pMonster, const _tchar* pMonsterComponentTag,
		const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, _bool* pbIsResult);
	HRESULT Collision_Boss_Knockback_Player(_uint iSceneID, CMonster* pMonster, const _tchar* pMonsterComponentTag,
		const _tchar* pPlayerComponentTag, _bool* pbIsResult);
private:
	CManagement*		m_pManagement = nullptr;
private:
	CObserver_Player*	m_pObserver = nullptr;
private:
	_bool Is_Player_Collided(CPlayer* pPlayer, const _tchar* pPlayerColldierTag, CGameObject* pGameObject, const _tchar* pColliderTag, CCollider::COLLISIONWAY eWay);
public:
	virtual void Free();
};

END