#pragma once

#include "Subject.h"

BEGIN(Client)

class CSubject_Player final : public CSubject
{
	DECLARE_SINGLETON(CSubject_Player)
public:
	enum DATA_TYPE { TYPE_MATRIX, TYPE_INFO, TYPE_RIGHTHAND, TYPE_LEFTHAND, TYPE_STATE, TYPE_BUFF, TYPE_END };
private:
	explicit CSubject_Player();
	virtual ~CSubject_Player() = default;
public:
	void* GetData(_int iMsg);
	HRESULT AddData(_int iMsg, void* pData);
private:
	map<_int, void*>		m_mapData;
public:
	virtual void Free();
};

END