#include "stdafx.h"
#include "..\Headers\Monster.h"
#include "Management.h"

USING(Client)

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CMonster::Ready_GameObject_Clone(void * pArg)
{
	return NOERROR;
}

_int CMonster::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CMonster::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CMonster::Render_GameObject()
{
	return NOERROR;
}

HRESULT CMonster::Add_Component(void * pArg)
{
	return NOERROR;
}

HRESULT CMonster::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CMonster::Render(_uint iPassIndex)
{
	return NOERROR;
}

CGameObject * CMonster::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CMonster::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
