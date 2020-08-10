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
		vector<CPlayer::OBJDESC> vecElement;
		vecElement.push_back(*(CPlayer::OBJDESC*)pArg);

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

	if (eType == TYPE_INTERACT)
	{
		if (m_mapMonsterDesc.empty())
		{
			vector<CCrystal::OBJDESC> vecElement;
			vecElement.push_back(*(CCrystal::OBJDESC*)pArg);

			m_mapInteract.insert({ eType,vecElement });
		}
		else
			m_mapInteract[eType].push_back(*(CCrystal::OBJDESC*)pArg);
	}
	
	if (eType == TYPE_ICICLE)
	{
		if (m_mapMonsterDesc.empty())
		{
			vector<CIcicle::OBJDESC> vecElement;
			vecElement.push_back(*(CIcicle::OBJDESC*)pArg);

			m_mapIcicle.insert({ eType,vecElement });
		}
		else
			m_mapIcicle[eType].push_back(*(CIcicle::OBJDESC*)pArg);
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

	if (eType == TYPE_INTERACT)
		return &m_mapInteract[eType];

	if (eType == TYPE_ICICLE)
		return &m_mapIcicle[eType];

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

	for (auto& Pair : m_mapInteract)
		Pair.second.clear();

	m_mapInteract.clear();

	for (auto& Pair : m_mapIcicle)
		Pair.second.clear();

	m_mapIcicle.clear();

	return NOERROR;
}


void CIOManager::Free()
{
}
