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

class CMonster abstract : public CGameObject
{
public:
	typedef struct tagObjDesc
	{
		_tchar szFileName[MAX_STR];
		_float fFrustumRadius;
		_matrix matWorld;
	}OBJDESC;
public:
	typedef struct tagMonsterInfo
	{
		_int iCurHp; _int iMaxHp;
		_int iMinDmg; _int iMaxDmg;

		_int iGold;
		_int iExp;
	}MONSTERINFO;
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
protected:
	CShader*			m_pShaderCom = nullptr;
	CFrustum*			m_pFrustumCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
protected:
	OBJDESC				m_tObjDesc;
	MONSTERINFO			m_tMonsterInfo;
protected:
	_double				m_TimeDelta = 0.0;
protected:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END