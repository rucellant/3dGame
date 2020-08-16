#include "stdafx.h"
#include "..\Headers\Door_Trigger.h"
#include "UI.h"
#include "Crystal.h"
#include "Management.h"

USING(Client)

CDoor_Trigger::CDoor_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDoor_Trigger::CDoor_Trigger(const CDoor_Trigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDoor_Trigger::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDoor_Trigger::Ready_GameObject_Clone(void * pArg)
{
	m_bIsAlive = true;

	return NOERROR;
}

_int CDoor_Trigger::Update_GameObject(_double TimeDelta)
{
	list<CGameObject*>::iterator iter = m_ObjectLst.begin();
	for (; iter != m_ObjectLst.end();)
	{
		if (((CCrystal*)(*iter))->GetHitCount() >= 3)
			iter = m_ObjectLst.erase(iter);
		else
			++iter;
	}

	if (m_ObjectLst.empty())
	{
		if (m_TimeWait <= 5.0)
			m_TimeWait += TimeDelta;
	}

	if (m_TimeWait >= 5.0 &&m_TimeWait < 10.0)
	{
		m_bIsActive = true;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Trigger", this);

		list<CGameObject*>* pUILayer = m_pManagement->Get_Layer(g_eScene, L"Layer_UI");

		for (auto& pUI : *pUILayer)
			m_pManagement->Push_GameObject(g_eScene, L"Layer_UI", pUI);

		m_pManagement->Play_Camera(g_eScene, L"Camera_Free");
	}

	return _int();
}

_int CDoor_Trigger::LateUpdate_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CDoor_Trigger::Render_GameObject()
{
	return NOERROR;
}

HRESULT CDoor_Trigger::Register_GameObject(CGameObject * pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	m_ObjectLst.push_back(pGameObject);

	return NOERROR;
}

CDoor_Trigger * CDoor_Trigger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDoor_Trigger* pInstance = new CDoor_Trigger(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CDoor_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoor_Trigger::Clone_GameObject(void * pArg)
{
	CDoor_Trigger* pInstance = new CDoor_Trigger(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CDoor_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor_Trigger::Free()
{
	CGameObject::Free();
}
