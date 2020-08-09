#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Observer_Player.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CTransform;
class CMesh_Static;
END

BEGIN(Client)

class CShield final : public CGameObject
{
private:
	explicit CShield(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CShield(const CShield& rhs);
	virtual ~CShield() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CMesh_Static*		m_pMeshCom = nullptr;
private:
	CObserver_Player*	m_pObserver = nullptr;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CShield* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END