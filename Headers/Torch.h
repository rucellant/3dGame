#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CTransform;
class CMesh_Static;
END

BEGIN(Client)

class CTorch final : public CGameObject
{
public:
	typedef struct tagObjDesc
	{
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_matrix matWorld;
		D3DXCOLOR vColor;
	}OBJDESC;
private:
	explicit CTorch(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTorch(const CTorch& rhs);
	virtual ~CTorch() = default;
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
	OBJDESC				m_tObjDesc;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CTorch* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END