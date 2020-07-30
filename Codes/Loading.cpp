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
	if (FAILED(Ready_Terrain_Stage()))
		return E_FAIL;

	if (FAILED(Ready_Dynamic_Stage()))
		return E_FAIL;

	if (FAILED(Ready_Static_Stage()))
		return E_FAIL;

	m_iComplete = 1;

	return NOERROR;
}

HRESULT CLoading::Ready_Terrain_Stage()
{
	// For. Component_Shader_Terrain
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Terrain", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;

	_matrix matLocal;
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	// For. Component_Mesh_Terrain_Stage
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Terrain_Stage", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Terrain/SoulValley/", L"SoulValley.X", &matLocal))))
		return E_FAIL;

	// For. GameObject_Terrain
	if (FAILED(m_pManagement->Add_GameObject_Prototype(L"GameObject_Terrain", CTerrain::Create(m_pGraphic_Device))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Dynamic_Stage()
{
	_matrix matLocal;

	// For. Component_Mesh_Hanuman
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Hanuman", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/HanumanBoss/", L"Hanuman.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Knole_Warrior
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Knole_Warrior", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Monster/Knole_Warrior/", L"Knole_Warrior.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Knole_Commander
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Knole_Commander", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Monster/Knole_Commander/", L"Knole_Commander.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Babegazi_Axe
	D3DXMatrixScaling(&matLocal, 0.7f, 0.7f, 0.7f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Babegazi_Axe", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Monster/Babegazi_Axe/", L"Babegazi_Axe.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Babegazi_Bow
	D3DXMatrixScaling(&matLocal, 0.6f, 0.6f, 0.6f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Babegazi_Bow", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Monster/Babegazi_Bow/", L"Babegazi_Bow.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Babegazi_Warrior
	D3DXMatrixScaling(&matLocal, 0.7f, 0.7f, 0.7f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Babegazi_Warrior", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Monster/Babegazi_Warrior/", L"Babegazi_Warrior.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_WitchBlade
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_WitchBlade", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Player_WitchBlade/", L"WitchBlade.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Ruisa
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Ruisa", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/NPC/Ruisa/", L"Ruisa.X", &matLocal))))
		return E_FAIL;

	// Read File
	HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Dynamic_Monster.dat", GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	_ulong dwBytes = 0;

	while (1)
	{
		_tchar szFileName[MAX_STR];
		ReadFile(hFile, szFileName, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

		_tchar szFilePath[MAX_STR];
		ReadFile(hFile, szFilePath, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

		_tchar szComponentTag[MAX_STR];
		ReadFile(hFile, szComponentTag, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

		_float fFrustumRadius;
		ReadFile(hFile, &fFrustumRadius, sizeof(_float), &dwBytes, nullptr);

		_matrix matWorld;
		ReadFile(hFile, &matWorld, sizeof(_matrix), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		CIOManager::GetInstance();
	}

	return NOERROR;
}

HRESULT CLoading::Ready_Static_Stage()
{
	_matrix matLocal;

	// For. Component_Mesh_Arrow
	D3DXMatrixScaling(&matLocal, 4.f, 4.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Arrow", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Arrow/", L"Arrow.X", &matLocal))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Boss()
{
	if (FAILED(Ready_Terrain_Boss()))
		return E_FAIL;

	m_iComplete = 1;

	return NOERROR;
}

HRESULT CLoading::Ready_Terrain_Boss()
{
	_matrix matLocal;
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	// For. Component_Mesh_Terrain_Boss
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_BOSS, L"Component_Mesh_Terrain_Boss", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Terrain/Volsland_UT/", L"Volsland_UT.X", &matLocal))))
		return E_FAIL;

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
