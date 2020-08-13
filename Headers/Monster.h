#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#define DEFAULT_ANIM_SPEED		1.f
#define DEFAULT_ANIM_DURATION	0.2
#define DEFAULT_ANIM_PERIOD		0.09

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CMesh_Dynamic;
END

BEGIN(Client)

class CObserver_Player;

class CMonster abstract : public CGameObject
{
public:
	enum MONSTER_TYPE { TYPE_SKELETON, TYPE_SOLDIER, TYPE_END };
public:
	enum STATE { IDLE, RUN, ATT, HIT, DOWN, GROGGY, DEAD, STATE_END };
public:
	typedef struct tagMonsterInfo
	{
		_int iCurHp; _int iMaxHp;
		_int iMinDmg; _int iMaxDmg;

		_int iGold;
	}MONSTERINFO;
public:
	typedef struct tagObjDesc
	{
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
public: // Set
	void SetIsActive(_bool bIsActive) {
		m_bIsAlive = bIsActive; }
public: // Get
	_bool GetIsActive() { 
		return m_bIsAlive; }
	MONSTER_TYPE GetType() {
		return m_eType; }
	STATE GetState() { 
		return m_eCurState; }
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	virtual HRESULT Knockdown(_vec3 vPosition = _vec3(0.f,0.f,0.f)) PURE;
	virtual HRESULT GetHit(_vec3 vPosition) PURE;
	virtual HRESULT Follow_Player(_vec3 vPosition);
	virtual HRESULT Set_Idle();
	virtual HRESULT Attack_Target(_vec3 vPosition);
	_int GetDmg();
protected:
	CObserver_Player*	m_pObserver = nullptr;
protected:
	_bool				m_bActive = false;
protected:
	OBJDESC				m_tObjDesc;
	MONSTERINFO			m_tMonsterInfo;
	STATE				m_eCurState = STATE_END;
	MONSTER_TYPE		m_eType = TYPE_END;
protected:
	_uint				m_iAnimation = 0;

	_float				m_fNewSpeed = DEFAULT_ANIM_SPEED;
	_double				m_Duration = DEFAULT_ANIM_DURATION;
	_double				m_Period = DEFAULT_ANIM_PERIOD;

	_float				m_TmpNewSpeed = DEFAULT_ANIM_SPEED;
	_double				m_TmpDuration = DEFAULT_ANIM_DURATION;
	_double				m_TmpPeriod = DEFAULT_ANIM_PERIOD;
protected:
	_double				m_TimeDelta = 0.0;
protected:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
protected:
	virtual HRESULT State_Machine(_double TimeDelta) PURE;
	virtual HRESULT State_Idle(_double TimeDelta) PURE;
	virtual HRESULT State_Run(_double TimeDelta) PURE;
	virtual HRESULT State_Att(_double TimeDelta) PURE;
	virtual HRESULT State_Hit(_double TimeDelta) PURE;
	virtual HRESULT State_Down(_double TimeDelta) PURE;
	virtual HRESULT State_Groggy(_double TimeDelta) PURE;
	virtual HRESULT State_Dead(_double TimeDelta) PURE;
public:
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END