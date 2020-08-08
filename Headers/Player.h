#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#define PLAYER_IDLE			39			// ������
#define PLAYER_ATT01		5			// 1�� ����
#define PLAYER_ATT02		6			// 2�� ����
#define PLAYER_ATT03		7			// 3�� ����
#define PLAYER_TORNADO		14			// ������ ���� ��ų
#define PLAYER_SHOULDER		22			// ����� -> ���߿� ���� ���
#define PLAYER_EARTHQUAKE	17			// ���� -> ȭ�� ����ŷ + ���� �ں��߸�
#define PLAYER_BUFFMOTION	23			// ������� -> ���� ��ų �� �� ����� ���
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


BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
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
	typedef struct tagObjDesc
	{
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
public:
	typedef struct tagPlayerInfo
	{
		_int iCurHp; _int iMaxHp;
		_int iMinDmg; _int iMaxDMg;
		_int iGold;
		_int iCurExp; _int iMaxExp;
		_int iCurLv; _int iMaxLv;
	}PLAYERINFO;
public:
	enum STATE { IDLE, RUN, ATT, SK, DOWN, GROGGY, STATE_END };
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CSpringArm*			m_pSpringArmCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
private: // ������
	OBJDESC				m_tObjDesc;
	PLAYERINFO			m_tPlayerInfo;
private: // ����
	STATE				m_eCurState = IDLE;
private: // Animation 
	_uint				m_iAnimation = PLAYER_IDLE;
private: // ����������
	CSubject_Player*	m_pSubject = nullptr;
private: // Camera
	_vec3				m_vSpringArm;
private:
	_double				m_TimeDelta = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
private:
	HRESULT State_Machine(_double TimeDelta);
	HRESULT State_Idle(_double TimeDelta);
	HRESULT State_Run(_double TimeDelta);
private:
	HRESULT Post_Update(_double TimeDelta);
private:
	HRESULT Update_CameraPosition(_double TimeDelta);
	HRESULT SetUp_OnNavigation();
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END