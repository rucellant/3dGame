#pragma once

#include "Player.h"
#include "Observer.h"
#include "Client_Defines.h"
#include "Subject_Player.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CObserver_Player final : public CObserver
{
private:
	explicit CObserver_Player();
	virtual ~CObserver_Player() = default;
public:
	void* GetData(_int iMsg);
public:
	virtual HRESULT Update(_int iMsg);
private:
	_matrix m_matWorld;
	CPlayer::PLAYERINFO m_tInfo;
	_matrix m_matRightHandWorld;
	_matrix m_matLeftHandWorld;
	CPlayer::STATE m_eCurState;
	_bool m_bIsBuff;
public:
	static CObserver_Player* Create();
	virtual void Free();
};

END