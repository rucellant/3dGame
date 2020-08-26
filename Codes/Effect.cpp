#include "stdafx.h"
#include "..\Headers\Effect.h"
#include "Management.h"
#include "Observer_Player.h"

USING(Client)

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect::Ready_GameObject_Clone(void * pArg)
{
	return NOERROR;
}

_int CEffect::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CEffect::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CEffect::Render_GameObject()
{
	return NOERROR;
}

HRESULT CEffect::Add_Component(void * pArg)
{
	return NOERROR;
}

HRESULT CEffect::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CEffect::Render(_uint iPassIndex)
{
	return NOERROR;
}

CGameObject * CEffect::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CEffect::Free()
{
	Safe_Release(m_pObserver);

	CGameObject::Free();
}
