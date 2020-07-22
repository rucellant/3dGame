#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Scene_Logo.h"


USING(Client)


CMainApp::CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	g_eScene = SCENE_LOGO;

	if (FAILED(Ready_Default_Setting(CGraphic_Device::MODE_WIN, g_iWinCX, g_iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_Component_Prototype()))
		return E_FAIL;

	if (FAILED(Ready_Current_Scene(g_eScene)))
		return E_FAIL;

	return NOERROR;
}

_int CMainApp::Update_MainApp(double TimeDelta)
{
	if (m_pManagement == nullptr)
		return -1;

	if (FAILED(m_pManagement->Update_Input_Device()))
		return -1;

	return 	m_pManagement->Update_Current_Scene(TimeDelta);
}

HRESULT CMainApp::Render_MainApp()
{
	if (m_pGraphic_Device == nullptr ||
		m_pManagement == nullptr)
		return E_FAIL;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
	m_pGraphic_Device->BeginScene();

	//m_pRenderer->Render();
	m_pManagement->Render_Current_Scene();

	m_pGraphic_Device->EndScene();
	m_pGraphic_Device->Present(nullptr, nullptr, 0, nullptr);

	return NOERROR;
}

HRESULT CMainApp::Ready_Default_Setting(CGraphic_Device::MODE eMode, _uint iWinCX, _uint iWinCY)
{
	// For.Management
	if (m_pManagement == nullptr)
	{
		m_pManagement = CManagement::GetInstance();
		if (m_pManagement == nullptr)
			return E_FAIL;

		Safe_AddRef(m_pManagement);
	}

	// 엔진에서 사용할 수 있는 객체들의 생성작업을 한다.
	if (FAILED(m_pManagement->Ready_Engine(SCENE_END)))
		return E_FAIL;

	// For.Graphic_Device
	if (FAILED(m_pManagement->Ready_Graphic_Device(g_hWnd, eMode, iWinCX, iWinCY, &m_pGraphic_Device)))
		return E_FAIL;

	// For.Input_Device
	if (FAILED(m_pManagement->Ready_Input_Device(g_hInst, g_hWnd)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMainApp::Ready_Current_Scene(SCENEID eSceneID)
{
	if (m_pManagement == nullptr)
		return E_FAIL;

	CScene* pScene = nullptr;

	switch (eSceneID)
	{
	case SCENE_LOGO:
		pScene = CScene_Logo::Create(m_pGraphic_Device);
		break;
	case SCENE_STAGE:
		//pScene = CScene_Stage::Create(m_pGraphic_Device);
		break;
	case SCENE_BOSS:
		//pScene = CScene_Boss::Create(m_pGraphic_Device);
		break;
	}

	if (pScene == nullptr)
		return E_FAIL;

	if (FAILED(m_pManagement->Ready_Current_Scene(pScene)))
		return E_FAIL;

	Safe_Release(pScene);

	return NOERROR;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	return NOERROR;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("Failed while Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pManagement);
	Safe_Release(m_pGraphic_Device);

	CManagement::Release_Engine();
}
