#include "stdafx.h"
#include "..\Headers\CollisionMgr.h"
#include "Player.h"
#include "Shield.h"
#include "Monster.h"

USING(Client)


IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
	: m_pManagement(CManagement::GetInstance())
{
}


HRESULT CCollisionMgr::Collision_Player_Att_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag,
	const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay)
{
	list<CGameObject*>* pPlayerLayer = m_pManagement->Get_Layer(iSceneID, pPlayerLayerTag);
	list<CGameObject*>* pMonsterLayer = m_pManagement->Get_Layer(iSceneID, pMonsterLayerTag);

	if (pPlayerLayer == nullptr || pMonsterLayer == nullptr)
		return NOERROR;

	for (auto& pPlayer : *pPlayerLayer)
	{
		if (!((CPlayer*)pPlayer)->GetIsAlive() || !((CPlayer*)pPlayer)->GetIsControl())
			return NOERROR;

		for (auto& pMonster : *pMonsterLayer)
		{
			if (!((CMonster*)pMonster)->GetIsActive() || !((CMonster*)pMonster)->GetIsAlive())
				continue;

			_bool bIsCollision = false;

			CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(pPlayerComponentTag);
			CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(pMonsterComponentTag);

			if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
				return E_FAIL;

			switch (eWay)
			{
			case CCollider::WAY_AABB:
				bIsCollision = pPlayerCollider->Collision_AABB(pMonsterCollider);
				break;
			case CCollider::WAY_OBB:
				bIsCollision = pPlayerCollider->Collision_OBB(pMonsterCollider);
				break;
			case CCollider::WAY_SPHERE:
				bIsCollision = pPlayerCollider->Collision_Sphere(pMonsterCollider);
				break;
			case CCollider::WAY_BOXSPHERE:
				bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);
				break;
			}

			if (!bIsCollision) // 충돌하지 않을 경우
				continue;
			else // 충돌한 경우
			{
				//1. 몬스터로 향하는 벡터를 구함
				_vec3 vPlayerPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
				_vec3 vMonsterPosition = ((CTransform*)pMonster->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
				vMonsterPosition.y = vPlayerPosition.y;

				_vec3 vDir = vMonsterPosition - vPlayerPosition;
				D3DXVec3Normalize(&vDir, &vDir);

				//2. 플레이어의 룩을 구함
				_vec3 vPlayerLook = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_LOOK);
				D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

				if (0.4f <= D3DXVec3Dot(&vDir, &vPlayerLook) && 1.1f >= D3DXVec3Dot(&vDir, &vPlayerLook))
					((CMonster*)pMonster)->GetHit(vPlayerPosition);
			}
		}
	}

	return NOERROR;
}

HRESULT CCollisionMgr::Collision_Player_Tornado_Monster(_uint iSceneID, const _tchar * pPlayerLayerTag, const _tchar * pPlayerComponentTag, 
	const _tchar * pMonsterLayerTag, const _tchar * pMonsterComponentTag, CCollider::COLLISIONWAY eWay)
{
	list<CGameObject*>* pPlayerLayer = m_pManagement->Get_Layer(iSceneID, pPlayerLayerTag);
	list<CGameObject*>* pMonsterLayer = m_pManagement->Get_Layer(iSceneID, pMonsterLayerTag);

	if (pPlayerLayer == nullptr || pMonsterLayer == nullptr)
		return NOERROR;

	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

	if (!pPlayer->GetIsAlive() || !pPlayer->GetIsControl())
		return NOERROR;

	for (auto& pMonster : *pMonsterLayer)
	{
		if (!((CMonster*)pMonster)->GetIsActive() || !((CMonster*)pMonster)->GetIsAlive())
			continue;

		_bool bIsCollision = false;

		CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(pPlayerComponentTag);
		CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(pMonsterComponentTag);

		if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
			return E_FAIL;

		switch (eWay)
		{
		case CCollider::WAY_AABB:
			bIsCollision = pPlayerCollider->Collision_AABB(pMonsterCollider);
			break;
		case CCollider::WAY_OBB:
			bIsCollision = pPlayerCollider->Collision_OBB(pMonsterCollider);
			break;
		case CCollider::WAY_SPHERE:
			bIsCollision = pPlayerCollider->Collision_Sphere(pMonsterCollider);
			break;
		case CCollider::WAY_BOXSPHERE:
			bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);
			break;
		}

		if (!bIsCollision) // 충돌하지 않을 경우
			continue;
		else // 충돌한 경우
		{
			_vec3 vPlayerPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);

			((CMonster*)pMonster)->Knockdown(vPlayerPosition);
		}
	}

	return NOERROR;
}

HRESULT CCollisionMgr::Collision_Player_Shoulder_Monster(_uint iSceneID, const _tchar * pPlayerLayerTag, const _tchar * pPlayerComponentTag, 
	const _tchar * pMonsterLayerTag, const _tchar * pMonsterComponentTag, CCollider::COLLISIONWAY eWay)
{
	list<CGameObject*>* pPlayerLayer = m_pManagement->Get_Layer(iSceneID, pPlayerLayerTag);
	list<CGameObject*>* pMonsterLayer = m_pManagement->Get_Layer(iSceneID, pMonsterLayerTag);

	if (pPlayerLayer == nullptr || pMonsterLayer == nullptr)
		return NOERROR;

	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

	if (!pPlayer->GetIsAlive() || !pPlayer->GetIsControl())
		return NOERROR;

	for (auto& pMonster : *pMonsterLayer)
	{
		if (!((CMonster*)pMonster)->GetIsActive() || !((CMonster*)pMonster)->GetIsAlive())
			continue;

		_bool bIsCollision = false;

		CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(pPlayerComponentTag);
		CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(pMonsterComponentTag);

		if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
			return E_FAIL;

		switch (eWay)
		{
		case CCollider::WAY_AABB:
			bIsCollision = pPlayerCollider->Collision_AABB(pMonsterCollider);
			break;
		case CCollider::WAY_OBB:
			bIsCollision = pPlayerCollider->Collision_OBB(pMonsterCollider);
			break;
		case CCollider::WAY_SPHERE:
			bIsCollision = pPlayerCollider->Collision_Sphere(pMonsterCollider);
			break;
		case CCollider::WAY_BOXSPHERE:
			bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);
			break;
		}

		if (!bIsCollision) // 충돌하지 않을 경우
			continue;
		else // 충돌한 경우
		{
			//1. 몬스터로 향하는 벡터를 구함
			_vec3 vPlayerPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
			_vec3 vMonsterPosition = ((CTransform*)pMonster->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
			vMonsterPosition.y = vPlayerPosition.y;

			_vec3 vDir = vMonsterPosition - vPlayerPosition;
			D3DXVec3Normalize(&vDir, &vDir);

			//2. 플레이어의 룩을 구함
			_vec3 vPlayerLook = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_LOOK);
			D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

			if (0.4f <= D3DXVec3Dot(&vDir, &vPlayerLook) && 1.1f >= D3DXVec3Dot(&vDir, &vPlayerLook))
				((CMonster*)pMonster)->Knockdown(vPlayerPosition);
		}
	}

	return NOERROR;
}

HRESULT CCollisionMgr::Collision_Player_Earthquake_Monster(_uint iSceneID, const _tchar* pPlayerLayerTag, const _tchar* pShieldComponentTag, const _tchar* pPlayerComponentTag,
	const _tchar* pMonsterLayerTag, const _tchar* pMonsterComponentTag, CCollider::COLLISIONWAY eWay)
{
	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, pPlayerLayerTag, 0);
	CShield* pShield = (CShield*)m_pManagement->Get_GameObject(g_eScene, pPlayerLayerTag, 2);
	list<CGameObject*>* pMonsterLayer = m_pManagement->Get_Layer(iSceneID, pMonsterLayerTag);

	if (pPlayer == nullptr || pShield == nullptr || pMonsterLayer == nullptr)
		return E_FAIL;

	CCollider* pBottomCollider = (CCollider*)pPlayer->Get_Component(L"Com_BottomBox");
	CCollider* pShieldCollider = (CCollider*)pShield->Get_Component(L"Com_Collider");

	if (pBottomCollider == nullptr || pShieldCollider == nullptr)
		return E_FAIL;

	_bool bIsCollision = pBottomCollider->Collision_OBB(pShieldCollider);
	if (!bIsCollision)
		return NOERROR;

	for (auto& pMonster : *pMonsterLayer)
	{
		if (!((CMonster*)pMonster)->GetIsActive() || !((CMonster*)pMonster)->GetIsAlive())
			continue;

		_bool bIsCollision = false;

		CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(pPlayerComponentTag);
		CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(pMonsterComponentTag);

		if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
			return E_FAIL;

		switch (eWay)
		{
		case CCollider::WAY_AABB:
			bIsCollision = pPlayerCollider->Collision_AABB(pMonsterCollider);
			break;
		case CCollider::WAY_OBB:
			bIsCollision = pPlayerCollider->Collision_OBB(pMonsterCollider);
			break;
		case CCollider::WAY_SPHERE:
			bIsCollision = pPlayerCollider->Collision_Sphere(pMonsterCollider);
			break;
		case CCollider::WAY_BOXSPHERE:
			bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);
			break;
		}

		if (!bIsCollision) // 충돌하지 않을 경우
			continue;
		else // 충돌한 경우
		{
			_vec3 vPlayerPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);

			((CMonster*)pMonster)->Knockdown(vPlayerPosition);
		}
	}

	return NOERROR;
}

HRESULT CCollisionMgr::Collision_Monster_Detect_Player(_uint iSceneID, CMonster* pMonster,
	const _tchar* pPlayerLayerTag, const _tchar* pPlayerComponentTag, CCollider::COLLISIONWAY eWay, _bool* pIsTrue)
{
	list<CGameObject*>* pPlayerLayer = m_pManagement->Get_Layer(iSceneID, pPlayerLayerTag);

	if (pPlayerLayer == nullptr || pMonster == nullptr)
		return E_FAIL;

	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

	if (!pPlayer->GetIsAlive() || !pPlayer->GetIsControl())
		return NOERROR;

	CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(L"Com_HitBox");
	CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(L"Com_DetectBox");
	if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
		return E_FAIL;

	_bool bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);

	if (!bIsCollision)
	{
		*pIsTrue = false;
		pMonster->Set_Idle();
	}
	else
	{
		*pIsTrue = true;
		_vec3 vPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
		pMonster->Follow_Player(vPosition);
	}

	return NOERROR;
}

HRESULT CCollisionMgr::Collision_Monster_Attack_Player(_uint iSceneID, CMonster * pMonster, const _tchar * pPlayerLayerTag, const _tchar * pPlayerComponentTag, _bool* pIsTrue)
{
	list<CGameObject*>* pPlayerLayer = m_pManagement->Get_Layer(iSceneID, pPlayerLayerTag);

	if (pPlayerLayer == nullptr || pMonster == nullptr)
		return E_FAIL;

	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");

	if (!pPlayer->GetIsAlive() || !pPlayer->GetIsControl())
		return NOERROR;

	CCollider* pPlayerCollider = (CCollider*)pPlayer->Get_Component(pPlayerComponentTag);
	CCollider* pMonsterCollider = (CCollider*)pMonster->Get_Component(L"Com_AttRangeBox");
	if (pPlayerCollider == nullptr || pMonsterCollider == nullptr)
		return E_FAIL;

	_bool bIsCollision = pPlayerCollider->Collision_BoxSphere(pMonsterCollider);

	if (!bIsCollision)
		return NOERROR;
	else
	{
		*pIsTrue = true;
		_vec3 vPosition = ((CTransform*)pPlayer->Get_Component(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION);
		pMonster->Attack_Target(vPosition);
		pPlayer->GetHit(pMonster->GetDmg());
	}

	return NOERROR;
}

void CCollisionMgr::Free()
{
}
