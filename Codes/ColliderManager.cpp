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

	if (FAILED(Collision_Player_Monster(iSceneID, eCaller, pSrcCollider, eCallee, pLayerTag, pColliderTag, eType, pArgOne, pArgTwo, pArgThree)))
		return E_FAIL;

	if (FAILED(Collision_Shield_Player(iSceneID, eCaller, pSrcCollider, eCallee, pLayerTag, pColliderTag, eType, pArgOne, pArgTwo, pArgThree)))
		return E_FAIL;
	
	if (FAILED(Collision_Monster_Player(iSceneID, eCaller, pSrcCollider, eCallee, pLayerTag, pColliderTag, eType, pArgOne, pArgTwo, pArgThree)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CColliderManager::Collision_Player_Monster(_uint iSceneID, TYPE eCaller, CCollider* pSrcCollider, TYPE eCallee, const _tchar* pLayerTag, const _tchar* pColliderTag, CollisionType eType, void* pArgOne, void* pArgTwo, void* pArgThree)
{
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
			if (((CMonster*)pGameObject)->GetType() == CMonster::TYPE_SKELETON || ((CMonster*)pGameObject)->GetType() == CMonster::TYPE_SOLDIER)
			{
				if (((CMonster*)pGameObject)->GetState() == CMonster::DOWN || ((CMonster*)pGameObject)->GetState() == CMonster::DEAD)
					continue;

				if (*(_uint*)pArgOne == PLAYER_TORNADO)
					((CMonster*)pGameObject)->Knockdown(*(_vec3*)pArgTwo);

				if (*(_uint*)pArgOne == PLAYER_ATT01 || *(_uint*)pArgOne == PLAYER_ATT02 || *(_uint*)pArgOne == PLAYER_ATT03)
				{
					// 부채꼴 범위 체크
					CTransform* pTargetTransform = (CTransform*)((CMonster*)pGameObject)->Get_Component(L"Com_Transform");

					_vec3 vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);

					vTargetPosition.y = (*(_vec3*)pArgTwo).y;

					_vec3 vDir = vTargetPosition - (*(_vec3*)pArgTwo);
					D3DXVec3Normalize(&vDir, &vDir);

					_vec3 vLook = *(_vec3*)pArgThree;
					D3DXVec3Normalize(&vLook, &vLook);

					if (0.4f <= D3DXVec3Dot(&vDir, &vLook) && 1.1f >= D3DXVec3Dot(&vDir, &vLook))
						((CMonster*)pGameObject)->GetHit(*(_vec3*)pArgTwo);
				}

				if (*(_uint*)pArgOne == PLAYER_SHOULDER)
				{
					// 부채꼴 범위 체크
					CTransform* pTargetTransform = (CTransform*)((CMonster*)pGameObject)->Get_Component(L"Com_Transform");

					_vec3 vTargetPosition = pTargetTransform->Get_State(CTransform::STATE_POSITION);

					vTargetPosition.y = (*(_vec3*)pArgTwo).y;

					_vec3 vDir = vTargetPosition - (*(_vec3*)pArgTwo);
					D3DXVec3Normalize(&vDir, &vDir);

					_vec3 vLook = *(_vec3*)pArgThree;
					D3DXVec3Normalize(&vLook, &vLook);

					if (0.4f <= D3DXVec3Dot(&vDir, &vLook) && 1.1f >= D3DXVec3Dot(&vDir, &vLook))
						((CMonster*)pGameObject)->Knockdown(*(_vec3*)pArgTwo);
				}
			}
		}
	}

	return NOERROR;
}

HRESULT CColliderManager::Collision_Shield_Player(_uint iSceneID, TYPE eCaller, CCollider * pSrcCollider, TYPE eCallee, const _tchar * pLayerTag, const _tchar * pColliderTag, CollisionType eType, void * pArgOne, void * pArgTwo, void * pArgThree)
{
	if (eCaller == TYPE_SHILED && eCallee == TYPE_PLAYER)
	{
		CCollider* pDstCollider = (CCollider*)m_pManagement->Get_Component(g_eScene, L"Layer_Player", L"Com_BottomBox");
		if (pDstCollider == nullptr)
			return E_FAIL; // 타겟 콜라이더가 없으면 fail

		_bool bResult = pSrcCollider->Collision_OBB(pDstCollider);

		if (!bResult)
			return NOERROR; // 충돌 안했으면 그냥 가시고

		list<CGameObject*>* pTargetLayer = m_pManagement->Get_Layer(g_eScene, L"Layer_Monster");
		if (pTargetLayer == nullptr)
			return E_FAIL;

		CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

		CCollider* pSrcCollider = (CCollider*)pPlayer->Get_Component(L"Com_DmgBox");

		for (auto& pGameObject : *pTargetLayer) // 각 몬스터들이 플레이어의 원과 겹치는 지 조사 해야함
		{
			if (!((CMonster*)pGameObject)->GetIsActive() || !((CMonster*)pGameObject)->GetIsAlive())
				continue; // 액티브가 아니거나 죽은 몬스터는 거름

						  // 플레이어와 몬스터의 콜라이더 가져옴
			CCollider* pDstCollider = (CCollider*)pGameObject->Get_Component(L"Com_HitBox");

			if (!pSrcCollider->Collision_BoxSphere(pDstCollider))
				continue;

			// 충돌했으면 그에 따른 사후 처리
			if (((CMonster*)pGameObject)->GetType() == CMonster::TYPE_SKELETON || ((CMonster*)pGameObject)->GetType() == CMonster::TYPE_SOLDIER)
			{
				if (((CMonster*)pGameObject)->GetState() == CMonster::DOWN || ((CMonster*)pGameObject)->GetState() == CMonster::DEAD)
					continue;

				((CMonster*)pGameObject)->Knockdown(((CTransform*)(pPlayer->Get_Component(L"Com_Transform")))->Get_State(CTransform::STATE_POSITION));
			}
		}
	}

	return NOERROR;
}

HRESULT CColliderManager::Collision_Monster_Player(_uint iSceneID, TYPE eCaller, CCollider * pSrcCollider, TYPE eCallee, const _tchar * pLayerTag, const _tchar * pColliderTag, CollisionType eType, void * pArgOne, void * pArgTwo, void * pArgThree)
{
	if (eCaller == TYPE_MONSTER && eCallee == TYPE_PLAYER)
	{
		CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(iSceneID, pLayerTag);
		if (pPlayer == nullptr)
			return E_FAIL;

		CCollider* pDstCollider = (CCollider*)pPlayer->Get_Component(pColliderTag);

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
		}

		if (!bResult)
		{
			// 스켈레톤이나 솔져의 경우
			if (CMonster::TYPE_SKELETON == *(CMonster::MONSTER_TYPE*)pArgOne || CMonster::TYPE_SOLDIER == *(CMonster::MONSTER_TYPE*)pArgOne)
			{
				if (*(CMonster::STATE*)pArgThree == CMonster::RUN)
				{
					_vec3 vPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);

					((CMonster*)pArgTwo)->Set_Idle();
				}
			}
		}
		else
		{
			// 스켈레톤이나 솔져의 경우
			if (CMonster::TYPE_SKELETON == *(CMonster::MONSTER_TYPE*)pArgOne || CMonster::TYPE_SOLDIER == *(CMonster::MONSTER_TYPE*)pArgOne)
			{
				if (*(CMonster::STATE*)pArgThree == CMonster::IDLE)
				{
					_vec3 vPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);

					((CMonster*)pArgTwo)->Follow_Player(vPosition);
				}
			}
		}
	}

	return NOERROR;
}

void CColliderManager::Free()
{
}
