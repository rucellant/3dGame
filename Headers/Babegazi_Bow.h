#pragma once

#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBabegazi_Bow final : public CMonster
{
private:
	explicit CBabegazi_Bow(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBabegazi_Bow(const CBabegazi_Bow& rhs);
	virtual ~CBabegazi_Bow() = default;
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
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Render(_uint iPassIndex);
public:
	static CBabegazi_Bow* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END