#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CFrustum;
class CRenderer;
class CTransform;
class CMesh_Dynamic;
END

BEGIN(Client)

class CWitchBlade final : public CGameObject
{
public:
	typedef struct tagObjDesc
	{
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
public:
	typedef struct tagPlayerInfo
	{
		_int iCurHp; _int iMaxHp;
		_int iMinDmg; _int iMaxDMg;
		_int iGold;
		_int iCurExp; _int iMaxExp;
		_int iCurLv; _int iMaxLv;
	}PLAYERINFO;
private:
	explicit CWitchBlade(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CWitchBlade(const CWitchBlade& rhs);
	virtual ~CWitchBlade() = default;
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
	CMesh_Dynamic*		m_pMeshCom = nullptr;
private:
	OBJDESC				m_tObjDesc;
	PLAYERINFO			m_tPlayerInfo;
private:
	_double				m_TimeDelta = 0.0;
private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CWitchBlade* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END