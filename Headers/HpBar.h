#pragma once

#include "UI.h"

BEGIN(Client)

class CObserver_Player;

class CHpBar final : public CUI
{
public:
	typedef struct tagDesc
	{
		_float fX, fY;
		_float fSizeX, fSizeY;
	}DESC;
private:
	explicit CHpBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHpBar(const CHpBar& rhs);
	virtual ~CHpBar() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	CObserver_Player*	m_pObserver = nullptr;
private:
	DESC				m_tDesc[2]; // 0은 배경의 검은색, 1은 실제 피 텍스쳐 
private:
	_double				m_TimeDelta = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT Render(_uint iPassIndex);
public:
	static CHpBar* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END