#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Buff final : public CEffect
{
private:
	enum MESH_TYPE { CIRCLE, CYLINDER, HELIXT };
private:
	explicit CEffect_Buff(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Buff(const CEffect_Buff& rhs);
	virtual ~CEffect_Buff() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Activate();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CMesh_Static*		m_pCylinderMeshCom = nullptr;	// 플레이어 외부에 겹쳐지는 두 겹의 원기둥
	CVIBuffer_HRect*	m_pVIBufferCom = nullptr;		// 바닥에 깔리는 마법진
	//CMesh_Static*		m_pHelixTMeshCom = nullptr;		// 버프 상태에서는 빛이 꼬불거리면서 위에서 아래로 내려옴
private:
	_matrix			m_matCircle; // 0은 바깥쪽 , 1은 안쪽
	_matrix			m_matHelixT;
	_matrix			m_matCylinder[12]; // 0은 바깥쪽 , 1은 안쪽
private:
	_double			m_TimeCylinderAcc[12];
	_bool			m_bTimeAcc[12];
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_int iIndex, MESH_TYPE eType);
	HRESULT Render(_uint iPassIndex, MESH_TYPE eType);
public:
	static CEffect_Buff* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END