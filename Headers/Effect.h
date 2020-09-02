#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CTexture;
class CRenderer;
class CTransform;
class CMesh_Static;
class CVIBuffer_VRect;
class CVIBuffer_HRect;
END

BEGIN(Client)

class CObserver_Player;

class CEffect abstract : public CGameObject
{
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
protected:
	CObserver_Player*	m_pObserver = nullptr;
protected:
	_double				m_TimeDelta = 0.0;
	_double				m_TimeAcc = 0.0;
protected:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END