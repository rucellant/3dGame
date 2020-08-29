#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Shoulder final : public CEffect
{
private:
	explicit CEffect_Shoulder(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_Shoulder(const CEffect_Shoulder& rhs);
	virtual ~CEffect_Shoulder() = default;
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
	CTexture*			m_pDstTextureCom = nullptr;
	CTexture*			m_pSrcTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
	HRESULT Set_Billboard();
public:
	static CEffect_Shoulder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END