#pragma once

#include "Monster.h"
#include "Client_Defines.h"

#define BALROG_IDLE				16			// ������
#define BALROG_RUN_F			14			// ����
#define BALROG_DEAD				26			// ���
#define BALROG_DEAD_LOOP		25			// �����������
#define BALROG_ATT01			28			// ������ ä������ ������ ������� ����
#define BALROG_ATT02			27			// ������
#define BALROG_BREATH			12			// �극�� 
#define BALROG_JUMPATT			4			// ������ �ڿ� �����ϸ鼭 ���� �̰� �ٵ� �ϴ� ����
#define BALROG_GETREADY			6			// �ƾ����� ����� �ִϸ��̼�

#define DEFAULT_ANIM_SPEED		1.f
#define DEFAULT_ANIM_DURATION	0.2
#define DEFAULT_ANIM_PERIOD		0.09		

BEGIN(Client)

class CSubject_Boss;
class CObserver_Player;

class CBalrog final : public CMonster
{
private:
	explicit CBalrog(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBalrog(const CBalrog& rhs);
	virtual ~CBalrog() = default;
public:
	virtual _matrix GetWorldMatrix();
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT GetReady();
	virtual HRESULT GetHit(_vec3 vPosition, _int iPlayerDmg);
	virtual HRESULT Attack_Target(_vec3 vPosition);
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pHitColliderCom = nullptr;				// �ǰ�
	CCollider*			m_pDmgOneColliderCom = nullptr;				// �Ϲ� ����
	CCollider*			m_pDmgTwoColliderCom = nullptr;				// ������
	CCollider*			m_pOuterIntersectColliderCom = nullptr;		// �ܺ� �����ڽ�
	CCollider*			m_pInnerIntersectColliderCom = nullptr;		// ���� �����ڽ�
	CTransform*			m_pTransformCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
private:
	_bool				m_bIsActive = false;
	_bool				m_bIsRender = false;
private: // ����������
	CSubject_Boss*		m_pSubject = nullptr;
private:
	CObserver_Player*	m_pObserver = nullptr;
private:
	_uint				m_iAttCount = 0;
private:
	_double				m_TimeWaitAcc = 0.0;
	_double				m_TimeDeadAcc = 0.0;
	_double				m_TimeAnimationAcc = 0.0;
private:
	_bool				m_bIsBreath = false;
	_bool				m_bIsAtt = false;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
private:
	HRESULT LookAtPlayer();
private:
	virtual HRESULT State_Machine(_double TimeDelta);
	virtual HRESULT State_Idle(_double TimeDelta);
	virtual HRESULT State_Run(_double TimeDelta);
	virtual HRESULT State_Att(_double TimeDelta);
	virtual HRESULT State_Hit(_double TimeDelta);
	virtual HRESULT State_Down(_double TimeDelta);
	virtual HRESULT State_Groggy(_double TimeDelta);
	virtual HRESULT State_Dead(_double TimeDelta);
private:
	HRESULT Update_Collider();
	HRESULT SetUp_OnNavigation();
public:
	static CBalrog* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END