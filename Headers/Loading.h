#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CManagement;
class CComponent_Manager;
END

BEGIN(Client)

class CLoading final : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphic_Device, CManagement* pManagement);
	virtual ~CLoading() = default;
public:
	SCENEID Get_SceneID() const {
		return m_eSceneID; }
	_uint Get_Complete() const {
		return m_iComplete; }
public:
	HRESULT Ready_Loading(SCENEID eSceneID);
public:
	static _uint APIENTRY Thread_Main(void* pArg);
private:
	CManagement*			m_pManagement = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
	SCENEID					m_eSceneID;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CS;	
	_uint					m_iComplete;

private: // Logo
	HRESULT Ready_Logo();

private: // Stage
	HRESULT Ready_Stage();
	HRESULT Ready_Terrain_Stage();
	HRESULT Ready_Dynamic_Stage();
	HRESULT Ready_Static_Stage();
	HRESULT Ready_Navigation_Stage();

private: // Boss
	HRESULT Ready_Boss();
	HRESULT Ready_Terrain_Boss();

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, CManagement* pManagement, SCENEID eSceneID);
	virtual void Free();
};

END