#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

#define PLAYER_IDLE			39			// 대기상태
#define PLAYER_ATT01		5			// 1번 공격
#define PLAYER_ATT02		6			// 2번 공격
#define PLAYER_ATT03		7			// 3번 공격
#define PLAYER_TORNADO		14			// 휠윈드 같은 스킬
#define PLAYER_SHOULDER		22			// 어깨빵 -> 공중에 몬스터 띄움
#define PLAYER_EARTHQUAKE	17			// 지진 -> 화면 쉐이킹 + 몬스터 자빠뜨림
#define PLAYER_BUFFMOTION	23			// 버프모션 -> 버프 스킬 쓸 때 사용할 모션
#define PLAYER_RUN_F		31			// 이동 모션
#define PLAYER_RUN_L		29			// 이동 모션
#define PLAYER_RUN_R		28			// 이동 모션
#define PLAYER_RUN_B		34			// 이동 모션
#define PLAYER_RUN_FL		11			// 이동 모션
#define PLAYER_RUN_FR		30			// 이동 모션
#define PLAYER_RUN_BL		33			// 이동 모션
#define PLAYER_RUN_BR		32			// 이동 모션
#define PLAYER_GROGGY		35			// 플레이어 그로기 상태 모션
#define PLAYER_DOWN_START	48			// 처음 다운 모션
#define PLAYER_DOWN_LOOP	46			// 반복 다운 모션
#define PLAYER_GETUP		45			// 다운->아이들 변경 모션


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
private: // 정보들
	OBJDESC				m_tObjDesc;
	PLAYERINFO			m_tPlayerInfo;
private: // 상태
	STATE				m_eCurState = IDLE;
private: // Animation 
	_uint				m_iAnimation = PLAYER_IDLE;
private: // 옵저버패턴
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