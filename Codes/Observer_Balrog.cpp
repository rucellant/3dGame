#include "stdafx.h"
#include "..\Headers\Observer_Balrog.h"
#include "Transform.h"


USING(Client)


CObserver_Balrog::CObserver_Balrog()
{
}

void * CObserver_Balrog::GetData(_int iMsg)
{
	if (iMsg >= CSubject_Balrog::TYPE_END)
		return nullptr;

	void* pData = nullptr;

	switch (iMsg)
	{
	case CSubject_Balrog::TYPE_MATRIX:
		pData = &m_matWorld;
		break;
	case CSubject_Balrog::TYPE_INFO:
		pData = &m_tInfo;
		break;
	case CSubject_Balrog::TYPE_STATE:
		pData = &m_eCurState;
		break;
	}

	return pData;
}

HRESULT CObserver_Balrog::Update(_int iMsg)
{
	void* pData = CSubject_Balrog::GetInstance()->GetData(iMsg);

	if (pData == nullptr)
		return E_FAIL;

	switch (iMsg)
	{
	case CSubject_Balrog::TYPE_MATRIX:
		m_matWorld = *(_matrix*)pData;
		break;
	case CSubject_Balrog::TYPE_INFO:
		m_tInfo = *(CMonster::MONSTERINFO*)pData;
		break;
	case CSubject_Balrog::TYPE_STATE:
		m_eCurState = *(CMonster::STATE*)pData;
		break;
	}

	return NOERROR;
}

CObserver_Balrog * CObserver_Balrog::Create()
{
	CObserver_Balrog* pInstance = new CObserver_Balrog;

	return pInstance;
}

void CObserver_Balrog::Free()
{
}

