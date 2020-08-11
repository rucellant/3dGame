#include "stdafx.h"
#include "..\Headers\ColliderManager.h"
#include "Player.h"
#include "Monster.h"


USING(Client)


IMPLEMENT_SINGLETON(CColliderManager)


CColliderManager::CColliderManager()
	: m_pManagement(CManagement::GetInstance())
{
}


HRESULT CColliderManager::Collision_Check(_uint iSceneID, TYPE eCaller, CCollider* pSrcCollider, TYPE eCallee, const _tchar* pLayerTag, const _tchar* pColliderTag, CollisionType eType
	, void* pArgOne, void* pArgTwo, void* pArgThree)
{
	if (iSceneID >= SCENE_END || eCaller >= TYPE_END || eCallee >= TYPE_END || pSrcCollider == nullptr)
		return E_FAIL;

	if (eCaller == TYPE_PLAYER && eCallee == TYPE_MONSTER)
	{
		list<CGameObject*>* pTargetLayer = m_pManagement->Get_Layer(g_eScene, pLayerTag);
		if (pTargetLayer == nullptr)
			return E_FAIL;

		for (auto& pGameObject : *pTargetLayer)
		{
			if (!((CMonster*)pGameObject)->GetIsActive() || !((CMonster*)pGameObject)->GetIsAlive())
				continue; // 액티브가 아니거나 죽은 몬스터는 거름

			CCollider* pDstCollider = (CCollider*)pGameObject->Get_Component(pColliderTag);
			if (pDstCollider == nullptr)
				return E_FAIL; // 타겟 콜라이더가 없으면 fail

			_bool bResult = false;

			switch (eType)
			{
			case TYPE_AABB:
				bResult = pSrcCollider->Collision_AABB(pDstCollider);
				break;
			case TYPE_OBB:
				bResult = pSrcCollider->Collision_OBB(pDstCollider);
				break;
			case TYPE_SPHERE:
				bResult = pSrcCollider->Collision_Sphere(pDstCollider);
				break;
			case TYPE_BOXSPHERE:
				bResult = pSrcCollider->Collision_BoxSphere(pDstCollider);
				break;
			} // 충돌 검사 

			if (!bResult)
				continue;

			// 충돌했으면 그에 따른 사후 처리
			if (*(_uint*)pArgOne == PLAYER_TORNADO)
				((CMonster*)pGameObject)->Knockdown(*(_vec3*)pArgTwo);
		}
	}
	
	if (eCaller == TYPE_MONSTER)
	{

	}

	return NOERROR;
}

void CColliderManager::Free()
{
}
