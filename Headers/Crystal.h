#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CCollider;
class CTransform;
class CMesh_Static;
END

BEGIN(Client)

class CCrystal final : public CGameObject
{
public:
	typedef struct tagObjDesc
	{
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
private:
	explicit CCrystal(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCrystal(const CCrystal& rhs);
	virtual ~CCrystal() = default;
public: // Get
	_uint GetHitCount() {
		return m_iHitCount; }
public: // Set
	void AddHitCount() {
		m_iHitCount++; }
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
	CCollider*			m_pColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CMesh_Static*		m_pMeshCom = nullptr;
private:
	OBJDESC				m_tObjDesc;
private:
	_uint				m_iHitCount = 0;
	_bool				m_bIsHit = false;
private:
	_double				m_TimeShader = 0.0;
private:
	_uint				m_iRenderIndex = 0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(_uint iRenderIndex);
	HRESULT Render(_uint iPassIndex);
public:
	static CCrystal* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END