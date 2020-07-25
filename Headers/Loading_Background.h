#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_VRect;
END

BEGIN(Client)

class CLoading_Background final : public CGameObject
{
private:
	explicit CLoading_Background(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CLoading_Background(const CLoading_Background& rhs);
	virtual ~CLoading_Background() = default;
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
	_uint				m_iNumTexture = 0;
	_uint				m_iCurTexture = 0;
	_double				m_TimeDelta = 0.0;
	_double				m_TimeAcc = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_uint iTextureIndex);
	HRESULT Render(_uint iPassIndex);
public:
	static CLoading_Background* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END