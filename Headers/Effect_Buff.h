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
private:
	CShader*		m_pShaderCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CMesh_Static*	m_pCircleMeshCom = nullptr;		// ������ �Ʒ��� �������� ������
	CMesh_Static*	m_pCylinderMeshCom = nullptr;	// ���̰� ���� �о����� ������
	CMesh_Static*	m_pHelixTMeshCom = nullptr;		// ���� ���¿����� ���� ���ҰŸ��鼭 ������ �Ʒ��� ������
private:
	_matrix			m_matCircle;
	_matrix			m_matHelixT;
	_matrix			m_matCylinder;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(MESH_TYPE eType);
	HRESULT Render(_uint iPassIndex, MESH_TYPE eType);
public:
	static CEffect_Buff* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END