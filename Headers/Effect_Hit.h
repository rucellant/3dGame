#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Hit final : public CEffect
{
private:
	explicit CEffect_Hit(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Hit(const CEffect_Hit& rhs);
	virtual ~CEffect_Hit() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
public:
	void Set_Position(_vec3 vPosition, _float fScale);
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
	HRESULT Set_Billboard();
public:
	static CEffect_Hit* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END