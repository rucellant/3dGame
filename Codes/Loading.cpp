#include "stdafx.h"
#include "..\Headers\Loading.h"
#include "Management.h"

USING(Client)

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)	
{
	
}

HRESULT CLoading::Ready_Loading(SCENEID eSceneID)
{	
	m_eSceneID = eSceneID;

	InitializeCriticalSection(&m_CS);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 1, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
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
		break;	
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoading->m_CS);	

	return _uint(0);
}

HRESULT CLoading::Ready_Logo()
{
	return NOERROR;
}

HRESULT CLoading::Ready_Stage()
{

	return NOERROR;
}

HRESULT CLoading::Ready_Boss()
{
	return NOERROR;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, SCENEID eSceneID)
{
	CLoading* pInstance = new CLoading(pGraphic_Device);

	if (FAILED(pInstance->Ready_Loading(eSceneID)))
	{
		MSG_BOX("CLoading Created Failed");
		Safe_Release(pInstance);
	}
	
	return pInstance;

	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//m_pGraphic_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);


}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITY);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CS);	
}
