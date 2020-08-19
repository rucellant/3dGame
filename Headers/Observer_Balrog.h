#pragma once

#include "Balrog.h"
#include "Observer.h"
#include "Client_Defines.h"
#include "Subject_Balrog.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CObserver_Balrog final : public CObserver
{
private:
	explicit CObserver_Balrog();
	virtual ~CObserver_Balrog() = default;
public:
	void* GetData(_int iMsg);
public:
	virtual HRESULT Update(_int iMsg);
private:
	_matrix m_matWorld;
	CMonster::MONSTERINFO m_tInfo;
	CMonster::STATE m_eCurState;
public:
	static CObserver_Balrog* Create();
	virtual void Free();
};

END