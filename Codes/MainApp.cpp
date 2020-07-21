#include "stdafx.h"
#include "..\Headers\MainApp.h"


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

	//SetCursorPos(g_iWinCX / 2, g_iWinCY / 2);

	return 	0;//m_pManagement->Update_Current_Scene(TimeDelta);
}

HRESULT CMainApp::Render_MainApp()
{
	return E_NOTIMPL;
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
	return E_NOTIMPL;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	return E_NOTIMPL;
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
