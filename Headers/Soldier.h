#pragma once

#include "Monster.h"
#include "Client_Defines.h"

#define SOLDIER_IDLE		22
#define SOLDIER_RUN_F		7
#define SOLDIER_RUN_L		19
#define SOLDIER_RUN_R		18
#define SOLDIER_RUN_B		20
#define SOLDIER_ATT_01		1
#define SOLDIER_ATT_02		2
#define SOLDIER_GROGGY		21
#define SOLDIER_DOWN_START	9
#define SOLDIER_DOWN_LOOP	12
#define SOLDIER_DOWN_DEAD	11
#define SOLDIER_DEAD		25
#define SOLDIER_DEAD_LOOP	26
#define SOLDIER_DMG			23
#define SOLDIER_GETUP		10
#define SOLDIER_SK			6

BEGIN(Client)

class CSoldier final : public CMonster
{
private:
	explicit CSoldier(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSoldier(const CSoldier& rhs);
	virtual ~CSoldier() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	virtual HRESULT Knockdown(_vec3 vPosition = _vec3(0.f, 0.f, 0.f));
	virtual HRESULT GetHit(_vec3 vPosition);
	virtual HRESULT Follow_Player(_vec3 vPosition);
	virtual HRESULT Attack_Target(_vec3 vPosition);
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pDmgColliderCom = nullptr;
	CCollider*			m_pDetectColliderCom = nullptr;
	CCollider*			m_pAttRangeColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
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
	static CSoldier* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END