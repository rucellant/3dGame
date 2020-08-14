#pragma once

#include "UI.h"

BEGIN(Client)

class CMonster;

class CHpBar_Monster final : public CUI
{
private:
	explicit CHpBar_Monster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHpBar_Monster(const CHpBar_Monster& rhs);
	virtual ~CHpBar_Monster() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	CMonster*			m_pTargetMonster = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT Render(_uint iPassIndex);
	HRESULT SetUp_OnTarget();
	HRESULT Set_Billboard();
public:
	static CHpBar_Monster* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END