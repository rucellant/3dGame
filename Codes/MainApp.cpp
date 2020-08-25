#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "IOManager.h"
#include "Scene_Boss.h"
#include "Scene_Logo.h"
#include "Scene_Stage.h"
#include "CollisionMgr.h"
#include "ColliderManager.h"


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

	_float fRotY = D3DX_PI / 2;

	_matrix matWorld;

	return 	m_pManagement->Update_Current_Scene(TimeDelta);
}

HRESULT CMainApp::Render_MainApp()
{
	if (m_pGraphic_Device == nullptr ||
		m_pManagement == nullptr)
		return E_FAIL;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 0.f, 1.f), 1.f, 0);
	m_pGraphic_Device->BeginScene();

	m_pRenderer->Render_Game();
	m_pManagement->Render_Current_Scene();

	m_pManagement->Render_FPS(L"Timer_60");

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

	// For. Engine
	if (FAILED(m_pManagement->Ready_Engine(SCENE_END)))
		return E_FAIL;

	// For. Graphic_Device
	if (FAILED(m_pManagement->Ready_Graphic_Device(g_hWnd, eMode, iWinCX, iWinCY, &m_pGraphic_Device)))
		return E_FAIL;

	// For. Input_Device
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
		pScene = CScene_Stage::Create(m_pGraphic_Device);
		break;
	case SCENE_BOSS:
		pScene = CScene_Boss::Create(m_pGraphic_Device);
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
	if (m_pManagement == nullptr)
		return E_FAIL;

	// For. Component_Renderer
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphic_Device))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	// For. Component_Transform
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Transform", CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	// For. Component_Frustum
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Frustum", CFrustum::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For. Component_Collider_AABB
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_AABB", CCollider::Create(m_pGraphic_Device, CCollider::TYPE_AABB))))
		return E_FAIL;

	// For. Component_Collider_OBB
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_OBB", CCollider::Create(m_pGraphic_Device, CCollider::TYPE_OBB))))
		return E_FAIL;

	// For. Component_Collider_Sphere
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Collider_Sphere", CCollider::Create(m_pGraphic_Device, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	// For. Component_SpringArm
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_SpringArm", CSpringArm::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For. Component_Shader_Default
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_Shader_Default", CShader::Create(m_pGraphic_Device, L"../Bin/ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;

	// For. Component_VIBuffer_VRect
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_VRect", CVIBuffer_VRect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For. Component_VIBuffer_HRect
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC, L"Component_VIBuffer_HRect", CVIBuffer_HRect::Create(m_pGraphic_Device))))
		return E_FAIL;

	// For. Component_Texture_Loading_Background
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC,
		L"Component_Texture_Loading_Background", 
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Loading_Background/Loading_Background_%d.png",
			4))))
		return E_FAIL;

	// For. Component_Texture_Loading_Icon
	if (FAILED(m_pManagement->Add_Component_Prototype(SCENE_STATIC,
		L"Component_Texture_Loading_Icon",
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Textures/Loading_Icon/Loading_Icon_%d.png"))))
		return E_FAIL;

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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pManagement);
	Safe_Release(m_pGraphic_Device);

	CIOManager::GetInstance()->DestroyInstance();
	CColliderManager::GetInstance()->DestroyInstance();
	CCollisionMgr::GetInstance()->DestroyInstance();

	CManagement::Release_Engine();
}
