#pragma once

#include "Scene.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLoading;

class CScene_Stage final : public CScene
{
private:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage() = default;
public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual _int LateUpdate_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();
private:
	CLoading* m_pLoading = nullptr;
private:
	HRESULT Ready_Layer_Terrain();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_Monster();
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Interact();
	HRESULT Ready_Layer_Static();
public:
	static CScene_Stage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free();
};

END