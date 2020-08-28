#pragma once

#include "Management.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	explicit CMainApp();
	virtual ~CMainApp() = default;
public:
	HRESULT Ready_MainApp();
	_int Update_MainApp(_double TimeDelta);
	HRESULT Render_MainApp();
private:
	CManagement*		m_pManagement = nullptr;
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
private:
	CRenderer*			m_pRenderer = nullptr;
private:
	HRESULT Ready_Default_Setting(CGraphic_Device::MODE eMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_Current_Scene(SCENEID eSceneID);
	HRESULT Ready_Component_Prototype();
public:
	static CMainApp* Create();
	virtual void Free(); // 나의 멤버변수를 정리한다.
};

END