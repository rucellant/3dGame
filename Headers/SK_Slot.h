#pragma once

#include "UI.h"

BEGIN(Client)

class CSK_Slot final : public CUI
{
public:
	enum SK_ID { SK_TORNADO, SK_SHOULDER, SK_EARTHQUAKE, SK_BUFFATT, SK_BUFFDEF, SK_END };
public: 
	typedef struct tagSlotDesc
	{
		_float fX, fY;
		_float fSizeX, fSizeY;
		_double TimeAcc;
		_double TimeWait;
	}SLOTDESC;
private:
	explicit CSK_Slot(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSK_Slot(const CSK_Slot& rhs);
	virtual ~CSK_Slot() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CShader*			m_pShaderCom = nullptr;
	CTexture*			m_pSKTextureCom = nullptr;
	CTexture*			m_pBaseTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	SLOTDESC			m_SK[SK_END];
private:
	_float				m_TmpNewSpeed = 1.f;
	_double				m_TmpDuration = 0.2;
	_double				m_TmpPeriod = 0.09;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(SK_ID eID);
	HRESULT Render(_uint iPassIndex);
private:
	HRESULT SetUp_PlayerSK(SK_ID eID);
public:
	static CSK_Slot* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END