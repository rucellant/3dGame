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
	CLoading* pLoading = (CLoading*)pArg;

	EnterCriticalSection(&pLoading->m_CS);

	HRESULT hr;

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

	if (FAILED(Ready_Navigation_Stage()))
		return E_FAIL;

	if (FAILED(Ready_UI_Stage()))
		return E_FAIL;

	if (FAILED(Ready_Interact_Stage()))
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
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

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

	// For. Component_Mesh_Skeleton
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Skeleton", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Skeleton_00/", L"Skeleton_00.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Soldier
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Soldier", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Soldier_00/", L"Soldier_00.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Ruisa
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Ruisa", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/NPC/Ruisa/", L"Ruisa.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Lups
	D3DXMatrixScaling(&matLocal, 0.8f, 0.8f, 0.8f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Lups", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Lups_00/", L"Lups_00.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Balrog
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Balrog", CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Balrog_00/", L"Balrog_00.X", &matLocal))))
		return E_FAIL;

	// 플레이어 몬스터 NPC 순으로 파일 입출력 진행

	// Read File Player
	{
		// For. Component_Shader_Player
		if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Player", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Player.fx"))))
			return E_FAIL;

		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Dynamic_Player.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

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

		CPlayer::OBJDESC tPlayerDesc;
		tPlayerDesc.fFrustumRadius = fFrustumRadius;
		tPlayerDesc.matWorld = matWorld;

		CIOManager::GetInstance()->Store(CIOManager::TYPE_PLAYER, &tPlayerDesc);

		CloseHandle(hFile);
	}

	// Read File Monster
	{
		// For. Component_Shader_Monster
		if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Monster", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Monster.fx"))))
			return E_FAIL;

		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Dynamic_Monster_Idle.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

	/*	while (1)
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

			CMonster::OBJDESC tMonsterDesc;
			tMonsterDesc.fFrustumRadius = fFrustumRadius;
			tMonsterDesc.matWorld = matWorld;
			lstrcpy(tMonsterDesc.szFileName, szFileName);

			CIOManager::GetInstance()->Store(CIOManager::TYPE_MONSTER, &tMonsterDesc);
		}*/

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

		CMonster::OBJDESC tMonsterDesc;
		tMonsterDesc.fFrustumRadius = fFrustumRadius;
		tMonsterDesc.matWorld = matWorld;
		lstrcpy(tMonsterDesc.szFileName, szFileName);

		CIOManager::GetInstance()->Store(CIOManager::TYPE_MONSTER, &tMonsterDesc);

		CloseHandle(hFile);
	}

	// Read File MidBoss
	{
		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Dynamic_Balrog.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

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

		CBalrog::OBJDESC tBalrogDesc;
		tBalrogDesc.fFrustumRadius = fFrustumRadius;
		tBalrogDesc.matWorld = matWorld;

		CIOManager::GetInstance()->Store(CIOManager::TYPE_BALROG, &tBalrogDesc);

		CloseHandle(hFile);
	}

	return NOERROR;
}

HRESULT CLoading::Ready_Static_Stage()
{
	_matrix matLocal;

	// For. Component_Shader_Mesh_Static
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Mesh_Static", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Mesh_Static.fx"))))
		return E_FAIL;

	// For. Component_Mesh_Arrow
	D3DXMatrixScaling(&matLocal, 4.f, 4.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Arrow", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Arrow/", L"Arrow.X", &matLocal))))
		return E_FAIL;

	// For. Component_Shader_Weapon
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Weapon", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Weapon.fx"))))
		return E_FAIL;

	// For. Component_Mesh_Weapon0
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Weapon0", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Weapon/", L"Weapon_00.X", &matLocal))))
		return E_FAIL;

	// For. Component_Texture_Weapon0
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Weapon0", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Weapon/Weapon_00.tga"))))
		return E_FAIL;

	// For. Component_Mesh_Weapon1
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Weapon1", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Weapon/", L"Weapon_01.X", &matLocal))))
		return E_FAIL;

	// For. Component_Texture_Weapon1
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Weapon1", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Weapon/Weapon_01.tga"))))
		return E_FAIL;

	// For. Component_Mesh_Weapon2
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Weapon2", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Weapon/", L"Weapon_02.X", &matLocal))))
		return E_FAIL;

	// For. Component_Texture_Weapon2
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Weapon2", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Weapon/Weapon_02.tga"))))
		return E_FAIL;

	// For. Component_Mesh_Weapon3
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Weapon3", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Weapon/", L"Weapon_03.X", &matLocal))))
		return E_FAIL;

	// For. Component_Texture_Weapon3
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Weapon3", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Weapon/Weapon_03.tga"))))
		return E_FAIL;

	// For. Component_Mesh_Shield
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Shield", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Shield/", L"Shield.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Gate
	_matrix matScale, matRotZ, matRotX;
	D3DXMatrixScaling(&matScale, 0.5f, 0.5f, 0.5f);
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(90.f));
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(-35.f));

	matLocal = matScale * matRotX * matRotZ;

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Mesh_Gate", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Dynamic/Gate/", L"Gate_A.X", &matLocal))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Navigation_Stage()
{
	// For. Component_Navigation_Stage
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Navigation_Stage", CNavigation::Create(m_pGraphic_Device, L"../Bin/Resources/Data/Stage_Navigation.dat"))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_UI_Stage()
{
	// For. Component_Shader_UI
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_UI", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_UI.fx"))))
		return E_FAIL;

	// for. Component_Texture_SK
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_SK", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/UI/SK_Slot/SK_%d.png", 5))))
		return E_FAIL;

	// For. Component_Texture_SK_Base
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_SK_Base", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/UI/SK_Slot/SK_Base.png"))))
		return E_FAIL;

	// For. Component_Texture_HpBar
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_HpBar", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/UI/HpBar/HpBar_%d.png", 3))))
		return E_FAIL;

	// For. Component_Texture_MpBar
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_MpBar", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/UI/MpBar/MpBar_%d.png", 3))))
		return E_FAIL;

	// For. Component_Texture_HpBar_Boss
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_HpBar_Boss", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/UI/HpBar_Boss/HpBar_Boss_%d.png", 4))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Interact_Stage()
{
	// For. Component_Shader_Effect
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Effect", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Effect.fx"))))
		return E_FAIL;

	_matrix matLocal, matScale, matRotY;

	// For. Component_Mesh_Crystal
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Crystal", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Cristal_1/", L"Cristal_1.X", &matLocal))))
		return E_FAIL;

	// For. Component_Mesh_Icicle
	D3DXMatrixScaling(&matLocal, 1.f, 1.f, 1.f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Icicle", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Static/Ice_Missile_2/", L"Ice_Missile_2.X", &matLocal))))
		return E_FAIL;

	// For. Component_Texture_Portal
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Texture_Portal", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Portal/Portal_%d.tga", 2))))
		return E_FAIL;

	// For. Component_Mesh_Twister
	D3DXMatrixScaling(&matLocal, 0.5f, 0.5f, 0.5f);

	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STAGE, L"Component_Mesh_Twister", CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resources/Mesh/Interact/IceTwister/", L"IceTwister.X", &matLocal))))
		return E_FAIL;

	// Read File Crystal
	{
		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Interact_Crystal.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

		while (1)
		{
			_tchar szFileName[MAX_STR];
			ReadFile(hFile, szFileName, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

			_tchar szFilePath[MAX_STR];
			ReadFile(hFile, szFilePath, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

			_float fFrustumRadius;
			ReadFile(hFile, &fFrustumRadius, sizeof(_float), &dwBytes, nullptr);

			_matrix matWorld;
			ReadFile(hFile, &matWorld, sizeof(_matrix), &dwBytes, nullptr);

			if (dwBytes == 0)
				break;

			CCrystal::OBJDESC tCrystalDesc;
			tCrystalDesc.fFrustumRadius = fFrustumRadius;
			tCrystalDesc.matWorld = matWorld;
			lstrcpy(tCrystalDesc.szFileName, szFileName);

			CIOManager::GetInstance()->Store(CIOManager::TYPE_INTERACT, &tCrystalDesc);
		}

		CloseHandle(hFile);
	}

	// Read File Icicle
	{
		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Interact_Icicle.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

		while (1)
		{
			_tchar szFileName[MAX_STR];
			ReadFile(hFile, szFileName, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

			_tchar szFilePath[MAX_STR];
			ReadFile(hFile, szFilePath, sizeof(_tchar) * MAX_STR, &dwBytes, nullptr);

			_float fFrustumRadius;
			ReadFile(hFile, &fFrustumRadius, sizeof(_float), &dwBytes, nullptr);

			_matrix matWorld;
			ReadFile(hFile, &matWorld, sizeof(_matrix), &dwBytes, nullptr);

			if (dwBytes == 0)
				break;

			CIcicle::OBJDESC tIcicleDesc;
			tIcicleDesc.fFrustumRadius = fFrustumRadius;
			tIcicleDesc.matWorld = matWorld;
			lstrcpy(tIcicleDesc.szFileName, szFileName);

			CIOManager::GetInstance()->Store(CIOManager::TYPE_ICICLE, &tIcicleDesc);
		}

		CloseHandle(hFile);
	}

	// Read File Twister
	{
		HANDLE hFile = CreateFile(L"../Bin/Resources/Data/Stage_Interact_Twister.dat", GENERIC_READ, 0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		_ulong dwBytes = 0;

		_int iCount = 0;

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

			if (iCount == 0)
				matWorld.m[3][0] = matWorld.m[3][0];

			if (iCount == 1)
				matWorld.m[3][0] = matWorld.m[3][0] + 10.f;

			if (iCount == 2)
				matWorld.m[3][0] = matWorld.m[3][0] - 10.f;

			if (dwBytes == 0)
				break;

			CTwister::OBJDESC tTwisterDesc;
			tTwisterDesc.fFrustumRadius = fFrustumRadius;
			tTwisterDesc.matWorld = matWorld;

			CIOManager::GetInstance()->Store(CIOManager::TYPE_TWISTER, &tTwisterDesc);

			iCount++;
		}

		CloseHandle(hFile);
	}

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
