#pragma once

#include "SK_Slot.h"
#include "GameObject.h"
#include "Client_Defines.h"

#define PLAYER_IDLE			39			// ������
#define PLAYER_ATT01		5			// 1�� ����
#define PLAYER_ATT02		6			// 2�� ����
#define PLAYER_ATT03		7			// 3�� ����
#define PLAYER_TORNADO		14			// ������ ���� ��ų
#define PLAYER_SHOULDER		22			// ����� -> ���߿� ���� ���
#define PLAYER_EARTHQUAKE	17			// ���� -> ȭ�� ����ŷ + ���� �ں��߸�
#define PLAYER_BUFFMOTION	23			// ������� -> ���� ��ų �� �� ����� ��� ������ �������� ����� �ΰ� ����
#define PLAYER_SWORDWAVE	8			// Į ���������� �ٴڿ� ���� ����. ���������� ������ ����� �߻���Ű��
#define PLAYER_RUN_F		31			// �̵� ���
#define PLAYER_RUN_L		29			// �̵� ���
#define PLAYER_RUN_R		28			// �̵� ���
#define PLAYER_RUN_B		34			// �̵� ���
#define PLAYER_RUN_FL		11			// �̵� ���
#define PLAYER_RUN_FR		30			// �̵� ���
#define PLAYER_RUN_BL		33			// �̵� ���
#define PLAYER_RUN_BR		32			// �̵� ���
#define PLAYER_GROGGY		35			// �÷��̾� �׷α� ���� ���
#define PLAYER_DOWN_START	48			// ó�� �ٿ� ���
#define PLAYER_DOWN_LOOP	46			// �ݺ� �ٿ� ���
#define PLAYER_GETUP		45			// �ٿ�->���̵� ���� ���

#define DEFAULT_ANIM_SPEED		1.f
#define DEFAULT_ANIM_DURATION	0.2
#define DEFAULT_ANIM_PERIOD		0.09

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CCollider;
class CTransform;
class CSpringArm;
class CNavigation;
class CMesh_Dynamic;
END

BEGIN(Client)

class CSubject_Player;

class CPlayer final : public CGameObject
{
public:
	typedef struct tagPlayerInfo
	{
		_int iCurHp; _int iMaxHp;
		_int iCurMp; _int iMaxMp;
		_int iMinDmg; _int iMaxDmg;
		_int iDefaultDef; _int iCurDef;
		_int iGold;
	}PLAYERINFO;
public:
	typedef struct tagObjDesc
	{
		_float fFrustumRadius;
		_matrix matWorld;
		PLAYERINFO tPlayerInfo;
	}OBJDESC;
public:
	enum STATE { IDLE, RUN, ATT, SK, DOWN, GROGGY, STATE_END };
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public: // Get
	_bool GetIsControl() { 
		return m_bIsControl; }
public: // Set
	void SetIsControl(_bool bIsControl) {
		m_bIsControl = bIsControl; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Knockdown(_vec3 vPosition = _vec3(0.f, 0.f, 0.f), _int iDmg = 0);
	HRESULT SetUp_PlayerSK(CSK_Slot::SK_ID eID, _double Duration = DEFAULT_ANIM_DURATION, _double Period = DEFAULT_ANIM_PERIOD);
	HRESULT GetHit(_int iMonsterDmg);
public:
	HRESULT Set_Position(_vec3 vPosition);
	HRESULT Set_Look(_vec3 vPosition);
	HRESULT Set_Navigation_Mode(_uint eMode);
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pHitColliderCom = nullptr;
	CCollider*			m_pDmgColliderCom = nullptr;
	CCollider*			m_pBottomColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CSpringArm*			m_pSpringArmCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
private: // ������
	OBJDESC				m_tObjDesc;
	PLAYERINFO			m_tPlayerInfo;
private: // �������
	_bool				m_bIsControl = true;
	_bool				m_bIsSK = false;
private: // ����
	STATE				m_eCurState = IDLE;
private: // Animation 
	_uint				m_iAnimation = PLAYER_IDLE;
	_uint				m_iNextAnimation = -1;

	_float				m_fNewSpeed = DEFAULT_ANIM_SPEED;
	_double				m_Duration = DEFAULT_ANIM_DURATION;
	_double				m_Period = DEFAULT_ANIM_PERIOD;

	_float				m_TmpNewSpeed = DEFAULT_ANIM_SPEED;
	_double				m_TmpDuration = DEFAULT_ANIM_DURATION;
	_double				m_TmpPeriod = DEFAULT_ANIM_PERIOD;
private: // ����������
	CSubject_Player*	m_pSubject = nullptr;
private: // Camera
	_vec3				m_vSpringArm;
private: // ���� �ऩ��
	_matrix				m_matHandWorld[2];
private: // Ÿ�̸�
	_double				m_TimeDelta = 0.0;
	_double				m_TimeAttAcc = 0.0;
	_double				m_TimeTornadoAcc = 0.0;
	_double				m_TimeExp = 0.0;
	_double				m_TimeDownAcc = 0.0;
	_double				m_TimeAfterImageAcc = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_uint iPassIndex);
	HRESULT Render(_uint iPassIndex);
private:
	HRESULT State_Machine(_double TimeDelta);
	HRESULT State_Idle(_double TimeDelta);
	HRESULT State_Run(_double TimeDelta);
	HRESULT State_Att(_double TimeDelta);
	HRESULT State_SK(_double TimeDelta);
	HRESULT State_Down(_double TimeDelta);
private:
	HRESULT Post_Update(_double TimeDelta);
private:
	HRESULT Update_CameraPosition(_double TimeDelta);
	HRESULT SetUp_OnNavigation();
	HRESULT Update_Collider();
	HRESULT Create_Tornado();
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END