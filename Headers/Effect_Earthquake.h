#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Earthquake final : public CEffect
{
private:
	enum MESH_TYPE { CIRCLE, CYLINDER, RING };
private:
	explicit CEffect_Earthquake(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Earthquake(const CEffect_Earthquake& rhs);
	virtual ~CEffect_Earthquake() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Activate();
private:
	CShader*		m_pShaderCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CMesh_Static*	m_pCircleMeshCom = nullptr;		// �ٴ� ġ�� ���� ��ó���� �ݹ��� ���� ����Ʈ
	CMesh_Static*	m_pCylinderMeshCom = nullptr;	// �ٴ� ġ�� ���� ������ ���� ���θ鼭 �������� ����Ʈ
	CMesh_Static*	m_pRingMeshCom = nullptr;		// �ٴ� ���� �� �÷��̾� ��ó�� ��Ÿ���� �ٴ� ����Ʈ
private:
	_matrix			m_matCircle[3];
	_matrix			m_matCylinder;
	_matrix			m_matRing;
private:
	_int			m_iCircleIndex = 0;
private:
	_double			m_TimeCircleAcc = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_int iIndex, MESH_TYPE eType);
	HRESULT Render(_uint iPassIndex, MESH_TYPE eType);
public:
	static CEffect_Earthquake* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END