#include "stdafx.h"
#include "..\Headers\IOManager.h"


USING(Client)


IMPLEMENT_SINGLETON(CIOManager)


CIOManager::CIOManager()
{
}

HRESULT CIOManager::Store(OBJECT_TYPE eType, void * pArg)
{
	if (eType >= TYPE_END)
		return E_FAIL;

	if (eType == TYPE_PLAYER)
	{
		vector<CWitchBlade::OBJDESC> vecElement;
		vecElement.push_back(*(CWitchBlade::OBJDESC*)pArg);

		m_mapWitchBlade.insert({ eType,vecElement });
	}

	if (eType == TYPE_MONSTER)
	{
		if (m_mapMonsterDesc.empty())
		{
			vector<CMonster::OBJDESC> vecElement;
			vecElement.push_back(*(CMonster::OBJDESC*)pArg);

			m_mapMonsterDesc.insert({ eType,vecElement });
		}
		else
			m_mapMonsterDesc[eType].push_back(*(CMonster::OBJDESC*)pArg);
	}
	

	return NOERROR;
}

void * CIOManager::Load(OBJECT_TYPE eType)
{
	if (eType >= TYPE_END)
		return nullptr;

	if (eType == TYPE_PLAYER)
		return &m_mapWitchBlade[eType];

	if (eType == TYPE_MONSTER)
		return &m_mapMonsterDesc[eType];

	return nullptr;
}

HRESULT CIOManager::Clear()
{
	for (auto& Pair : m_mapWitchBlade)
		Pair.second.clear();

	m_mapWitchBlade.clear();

	for (auto& Pair : m_mapMonsterDesc)
		Pair.second.clear();

	m_mapMonsterDesc.clear();

	return NOERROR;
}


void CIOManager::Free()
{
}
