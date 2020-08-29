#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Tornado final : public CEffect
{
private:
	explicit CEffect_Tornado(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Tornado(const CEffect_Tornado& rhs);
	virtual ~CEffect_Tornado() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	HRESULT Activate(_double TimeDelta);
private:
	CShader*		m_pShaderCom = nullptr;
	CFrustum*		m_pFrustumCom = nullptr;
	CTexture*		m_pTextureCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CMesh_Static*	m_pMeshCom = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
private:
	_bool			m_bIsWait = false;
	_bool			m_bIsFade = false;
public:
	static CEffect_Tornado* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END