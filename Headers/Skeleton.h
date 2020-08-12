#pragma once

#include "Monster.h"
#include "Client_Defines.h"

#define SKELETON_IDLE			14
#define SKELETON_RUN_F			13		
#define SKELETON_DMG			15	
#define SKELETON_ATT			19
#define SKELETON_GROGGY			10
#define SKELETON_DEAD			17
#define SKELETON_DEAD_LOOP		18
#define SKELETON_DOWN_START		9
#define SKELETON_DOWN_LOOP		7
#define SKELETON_DOWN_DEAD		8
#define SKELETON_GETUP			6

BEGIN(Client)

class CSkeleton final : public CMonster
{
private:
	explicit CSkeleton(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSkeleton(const CSkeleton& rhs);
	virtual ~CSkeleton() = default;
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
	virtual HRESULT Set_Idle();
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pHitColliderCom = nullptr;
	CCollider*			m_pDmgColliderCom = nullptr;
	CCollider*			m_pDetectColliderCom = nullptr;
	CCollider*			m_pAttRangeColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;

private: // ≈∏¿Ã∏”
	_double				m_TimeDownAcc = 0.0;
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
	static CSkeleton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END