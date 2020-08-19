#pragma once

#include "Subject.h"

BEGIN(Client)

class CSubject_Balrog final : public CSubject
{
	DECLARE_SINGLETON(CSubject_Balrog)
public:
	enum DATA_TYPE { TYPE_MATRIX, TYPE_INFO, TYPE_STATE, TYPE_END };
private:
	explicit CSubject_Balrog();
	virtual ~CSubject_Balrog() = default;
public:
	void* GetData(_int iMsg);
	HRESULT AddData(_int iMsg, void* pData);
private:
	map<_int, void*>		m_mapData;
public:
	virtual void Free();
};

END