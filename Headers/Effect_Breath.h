#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Breath final : public CEffect
{
private:
	explicit CEffect_Breath(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Breath(const CEffect_Breath& rhs);
	virtual ~CEffect_Breath() = default;
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
	CCollider*			m_pColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	CObserver_Boss*		m_pObserver_Boss = nullptr;
private:
	_matrix				m_matBillboard;
private:
	_bool				m_bIsDmg = false;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
	HRESULT Set_Billboard();
public:
	static CEffect_Breath* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END