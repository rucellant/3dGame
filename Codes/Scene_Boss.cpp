#include "stdafx.h"
#include "..\Headers\Scene_Boss.h"
#include "Loading.h"
#include "Management.h"

USING(Client)

CScene_Boss::CScene_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{
}

HRESULT CScene_Boss::Ready_Scene()
{
	return NOERROR;
}

_int CScene_Boss::Update_Scene(_double TimeDelta)
{
	return _int();
}

_int CScene_Boss::LateUpdate_Scene(_double TimeDelta)
{
	return _int();
}

HRESULT CScene_Boss::Render_Scene()
{
	return NOERROR;
}

CScene_Boss * CScene_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Boss* pInstance = new CScene_Boss(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(g_hWnd, L"CScene_Boss::Create() is Failed!", nullptr, MB_OK);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Boss::Free()
{
	Safe_Release(m_pLoading);

	CManagement* pManagement = CManagement::GetInstance();
	if (pManagement == nullptr)
		return;
	Safe_AddRef(pManagement);

	pManagement->Clear_Scene(SCENE_BOSS);

	Safe_Release(pManagement);

	g_eScene = SCENE_END;

	CScene::Free();
}
