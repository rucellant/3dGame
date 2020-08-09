#include "stdafx.h"
#include "..\Headers\UI.h"
#include "Management.h"


USING(Client)


CUI::CUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CUI::Ready_GameObject_Clone(void * pArg)
{
	return NOERROR;
}

_int CUI::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CUI::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CUI::Render_GameObject()
{
	return NOERROR;
}

CGameObject * CUI::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CUI::Free()
{
	CGameObject::Free();
}
