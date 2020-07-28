#include "stdafx.h"
#include "..\Headers\Loading.h"
#include "IOManager.h"
#include "Management.h"

USING(Client)

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphic_Device, CManagement* pManagement)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pManagement(pManagement)
{
	
}

HRESULT CLoading::Ready_Loading(SCENEID eSceneID)
{	
	m_eSceneID = eSceneID;

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 1, Thread_Main, this, 0, nullptr);
	if (m_hThread == 0)
		return E_FAIL;

	return NOERROR;
}

_uint CLoading::Thread_Main(void * pArg)
{
	CLoading*		pLoading = (CLoading*)pArg;

	EnterCriticalSection(&pLoading->m_CS);

	HRESULT			hr;

	switch (pLoading->Get_SceneID())
	{
	case SCENE_STAGE:
		hr = pLoading->Ready_Stage();
		break;
	case SCENE_BOSS:
		hr = pLoading->Ready_Boss();
		break;	
	case SCENE_LOGO:
		hr = pLoading->Ready_Logo();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoading->m_CS);	

	return _uint(0);
}

HRESULT CLoading::Ready_Logo()
{
	m_iComplete = 1;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage()
{
	// For. Component_Shader_Terrain
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Terrain", CShader::Create(m_pGraphic_Device,L"../Bin/ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;

	_matrix matLocal;
	D3DXMatrixIdentity(&matLocal);

	// For. Component_Mesh_Terrain_Stage
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Terrain_Stage", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Terrain/SoulValley/", L"SoulValley.X", &matLocal))))
		return E_FAIL;

	// For. GameObject_Terrain
	if (FAILED(m_pManagement->Add_GameObject_Prototype(L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	m_iComplete = 1;

	return NOERROR;
}

HRESULT CLoading::Ready_Boss()
{
	_matrix matLocal;
	D3DXMatrixIdentity(&matLocal);

	// For. Component_Mesh_Terrain_Boss
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_BOSS, L"Component_Mesh_Terrain_Boss", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Terrain/Volsland_UT/", L"Volsland_UT.X", &matLocal))))
		return E_FAIL;

	m_iComplete = 1;

	return NOERROR;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, CManagement* pManagement, SCENEID eSceneID)
{
	CLoading* pInstance = new CLoading(pGraphic_Device, pManagement);

	if (FAILED(pInstance->Ready_Loading(eSceneID)))
	{
		MSG_BOX("CLoading Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITY);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CS);	
}
