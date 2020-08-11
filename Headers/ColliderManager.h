#pragma once

#include "Base.h"
#include "Management.h"
#include "Client_Defines.h"

BEGIN(Client)

class CColliderManager final : public CBase
{
	DECLARE_SINGLETON(CColliderManager)
public:
	enum TYPE { TYPE_PLAYER, TYPE_MONSTER, TYPE_END };
	enum CollisionType { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_BOXSPHERE, TYPE_COLLISION };
private:
	explicit CColliderManager();
	virtual ~CColliderManager() = default;
public:
	// 호출자를 명확히 할 것. 호출대상을 명확히 할 것
	HRESULT Collision_Check(_uint iSceneID, TYPE eCaller, CCollider* pSrcCollider, TYPE eCallee, const _tchar* pLayerTag, const _tchar* pColliderTag, CollisionType eType, void* pArgOne = nullptr, void* pArgTwo = nullptr, void* pArgThree = nullptr);
private:
	CManagement* m_pManagement = nullptr;
public:
	virtual void Free();
};

END