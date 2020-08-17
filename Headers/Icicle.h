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

class CIcicle final : public CGameObject
{
public:
	typedef struct tagObjDesc
	{
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
private:
	explicit CIcicle(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CIcicle(const CIcicle& rhs);
	virtual ~CIcicle() = default;
public: // Get
public: // Set
	void SetIsActive(_bool bIsActive) {
		m_bIsActive = bIsActive; }
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
	CCollider*			m_pDmgColliderCom = nullptr;
	CCollider*			m_pIntersectColliderCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CMesh_Static*		m_pMeshCom = nullptr;
private:
	_double				m_TimeAcc = 0.0;
private:
	OBJDESC				m_tObjDesc;
private:
	_bool				m_bIsActive = false;
	_bool				m_bIsCollision = false;
private: // ¼öÁ÷ ³«ÇÏ
	_float				m_fVelocity = 0.f;
	_float				m_fAccelaration = 0.7f;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CIcicle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END