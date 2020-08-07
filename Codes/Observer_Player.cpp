#include "stdafx.h"
#include "..\Headers\Observer_Player.h"
#include "Transform.h"


USING(Client)


CObserver_Player::CObserver_Player()
{
}

void * CObserver_Player::GetData(_int iMsg)
{
	if (iMsg >= CSubject_Player::TYPE_END)
		return nullptr;

	void* pData = nullptr;

	switch (iMsg)
	{
	case CSubject_Player::TYPE_MATRIX:
		pData = &m_matWorld;
		break;
	case CSubject_Player::TYPE_INFO:
		pData = &m_tInfo;
		break;
	}

	return pData;
}

HRESULT CObserver_Player::Update(_int iMsg)
{
	void* pData = CSubject_Player::GetInstance()->GetData(iMsg);

	if (pData == nullptr)
		return E_FAIL;

	switch (iMsg)
	{
	case CSubject_Player::TYPE_MATRIX:
		m_matWorld = *(_matrix*)pData;
		break;
	case CSubject_Player::TYPE_INFO:
		m_tInfo = *(CPlayer::PLAYERINFO*)pData;
		break;
	}

	return NOERROR;
}

CObserver_Player * CObserver_Player::Create()
{
	CObserver_Player* pInstance = new CObserver_Player;

	return pInstance;
}

void CObserver_Player::Free()
{
}

