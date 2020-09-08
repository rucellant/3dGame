#include "stdafx.h"
#include "..\Headers\Observer_Boss.h"
#include "Transform.h"


USING(Client)


CObserver_Boss::CObserver_Boss()
{
}

void * CObserver_Boss::GetData(_int iMsg)
{
	if (iMsg >= CSubject_Boss::TYPE_END)
		return nullptr;

	void* pData = nullptr;

	switch (iMsg)
	{
	case CSubject_Boss::TYPE_MATRIX:
		pData = &m_matWorld;
		break;
	case CSubject_Boss::TYPE_INFO:
		pData = &m_tInfo;
		break;
	case CSubject_Boss::TYPE_STATE:
		pData = &m_eCurState;
		break;
	case CSubject_Boss::TYPE_MOUSE:
		pData = &m_matMouse;
		break;
	}

	return pData;
}

HRESULT CObserver_Boss::Update(_int iMsg)
{
	void* pData = CSubject_Boss::GetInstance()->GetData(iMsg);

	if (pData == nullptr)
		return E_FAIL;

	switch (iMsg)
	{
	case CSubject_Boss::TYPE_MATRIX:
		m_matWorld = *(_matrix*)pData;
		break;
	case CSubject_Boss::TYPE_INFO:
		m_tInfo = *(CMonster::MONSTERINFO*)pData;
		break;
	case CSubject_Boss::TYPE_STATE:
		m_eCurState = *(CMonster::STATE*)pData;
		break;
	case CSubject_Boss::TYPE_MOUSE:
		m_matMouse = *(_matrix*)pData;
		break;
	}

	return NOERROR;
}

CObserver_Boss * CObserver_Boss::Create()
{
	CObserver_Boss* pInstance = new CObserver_Boss;

	return pInstance;
}

void CObserver_Boss::Free()
{
}

