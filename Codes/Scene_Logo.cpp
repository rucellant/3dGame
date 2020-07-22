#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"
#include "Management.h"

USING(Client)

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Logo::Ready_Scene()
{
	return NOERROR;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	return _int();
}

_int CScene_Logo::LateUpdate_Scene(_double TimeDelta)
{
	return _int();
}

HRESULT CScene_Logo::Render_Scene()
{
	CManagement* pMangement = CManagement::GetInstance();
	if (pMangement == nullptr)
		return E_FAIL;
	Safe_AddRef(pMangement);

	pMangement->Render_FPS(L"Timer_60");

	Safe_Release(pMangement);

	return NOERROR;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(g_hWnd, L"CScene_Logo::Create() is Failed!", nullptr, MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	//Safe_Release(m_pLoading);

	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return;
	Safe_AddRef(pManagement);

	pManagement->Clear_Scene(SCENE_LOGO);

	Safe_Release(pManagement);

	CScene::Free();
}
