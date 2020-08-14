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

class CGate final : public CGameObject
{
public:
	typedef struct tagStaticDesc
	{
		_tchar szFilePath[MAX_STR];
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_vec3 vRotation;
	}STATICDESC;
private:
	explicit CGate(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CGate(const CGate& rhs);
	virtual ~CGate() = default;
public:
	STATICDESC* Get_Desc() {
		return &m_tStaticDesc;
	}
	_matrix Get_WorldMatrix();
	CTransform* Get_Transform() {
		return m_pTransformCom;
	}
public:
	void Set_Position(_vec3 vData);
	void Set_Frustum(_float fRadius) {
		m_tStaticDesc.fFrustumRadius = fRadius; }
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
	STATICDESC			m_tStaticDesc;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CGate* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END