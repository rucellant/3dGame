#include "stdafx.h"
#include "..\Headers\Subject_Boss.h"


USING(Client)


IMPLEMENT_SINGLETON(CSubject_Boss)


CSubject_Boss::CSubject_Boss()
{
}

void * CSubject_Boss::GetData(_int iMsg)
{
	auto iter = m_mapData.find(iMsg);

	if (m_mapData.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CSubject_Boss::AddData(_int iMsg, void * pData)
{
	if (pData == nullptr)
		return E_FAIL;

	auto find = m_mapData.find(iMsg);

	if (find != m_mapData.end())
		return E_FAIL;

	m_mapData.insert({ iMsg,pData });

	CSubject::Notify(iMsg);

	return NOERROR;
}

void CSubject_Boss::Free()
{
	m_mapData.clear();

	CSubject::Free();
}

