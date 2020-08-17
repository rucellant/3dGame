#include "stdafx.h"
#include "..\Headers\Door_Trigger.h"
#include "UI.h"
#include "Player.h"
#include "Portal.h"
#include "Crystal.h"
#include "Management.h"
#include "Camera_Event.h"

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

	if (m_ObjectLst.empty() && !m_bIsActive)
	{
		m_TimeWait += TimeDelta;

		if (m_TimeWait >= 5.0)
		{
			// 1. �̺�Ʈ ī�޶� ��ġ ����
			CCamera_Event* pCamera_Event = (CCamera_Event*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Camera", 2);
			pCamera_Event->Set_Position(_vec3(-34.f, 27.f, 188.f));
			// 2. �̺�Ʈ ī�޶� Ÿ�� ����
			CPortal* pPortal = (CPortal*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Interact");
			pCamera_Event->Set_Target((CTransform*)pPortal->Get_Component(L"Com_Transform"));
			// 3. UI �� ���� ����
			list<CGameObject*>* pUILayer = m_pManagement->Get_Layer(g_eScene, L"Layer_UI");

			for (auto& pUI : *pUILayer)
				m_pManagement->Push_GameObject(g_eScene, L"Layer_UI", pUI);
			// 4. �̺�Ʈ ī�޶� �۵� ������
			m_bIsActive = true;
			m_pManagement->Play_Camera(g_eScene, L"Camera_Event");
			// 5. �÷��̾� �۵�����
			CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");
			pPlayer->SetIsControl(false);
		}
	}

	if (m_bIsActive)
	{
		if (m_TimeWait < 10.0)
		{
			m_TimeWait += TimeDelta;

			CCamera_Event* pCamera_Event = (CCamera_Event*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Camera", 2);
			pCamera_Event->Move_Forward();
		}
		else if (m_TimeWait > 10.0 && m_TimeWait <= 15.0)
		{
			m_TimeWait += TimeDelta;
			CPortal* pPortal = (CPortal*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Interact");
			pPortal->SetFade(true);
		}
		else
		{
			// 1. Ʈ���� Ȱ������
			m_bIsActive = false;
			m_pManagement->Push_GameObject(g_eScene, L"Layer_Trigger", this);
			// 2. UI �ٽ� �� ����
			list<CGameObject*>* pUILayer = m_pManagement->Get_Layer(g_eScene, L"Layer_UI");

			for (auto& pUI : *pUILayer)
				m_pManagement->Pop_GameObject(g_eScene, L"Layer_UI");
			// 3. �ٽ� �÷��̾� ī�޶� ����
			m_pManagement->Play_Camera(g_eScene, L"Camera_Player");
			// 4. �÷��̾� �۵�����
			CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(g_eScene, L"Layer_Player");
			pPlayer->SetIsControl(true);
		}
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
