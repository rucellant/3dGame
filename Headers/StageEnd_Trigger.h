#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_VRect;
END

BEGIN(Client)

class CObserver_Boss;

class CStageEnd_Trigger final : public CGameObject
{
private:
	explicit CStageEnd_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CStageEnd_Trigger(const CStageEnd_Trigger& rhs);
	virtual ~CStageEnd_Trigger() = default;
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
	CCollider*			m_pColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CVIBuffer_VRect*	m_pVIBufferCom = nullptr;
private:
	CObserver_Boss*		m_pObserver = nullptr;
private:
	_bool*				m_pNextStage = nullptr;
private:
	_double				m_TimeAcc = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
	HRESULT Set_Billboard();
public:
	static CStageEnd_Trigger* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END