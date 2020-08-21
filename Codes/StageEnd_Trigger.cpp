#include "stdafx.h"
#include "..\Headers\StageEnd_Trigger.h"
#include "Balrog.h"
#include "Management.h"
#include "Observer_Boss.h"


USING(Client)

CStageEnd_Trigger::CStageEnd_Trigger(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CStageEnd_Trigger::CStageEnd_Trigger(const CStageEnd_Trigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStageEnd_Trigger::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CStageEnd_Trigger::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pNextStage = (_bool*)pArg;

	m_pTransformCom->Set_Scale(_vec3(10.f, 10.f, 10.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(112.f, 60.f, -128.f));

	m_pObserver = CObserver_Boss::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Boss::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_bIsAlive = true;

	return NOERROR;
}

_int CStageEnd_Trigger::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 1.0)
		m_TimeAcc = 0.0;

	m_pColliderCom->Update_Collider();

	return _int();
}

_int CStageEnd_Trigger::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f))
	{
		Set_Billboard();
		Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);
	}

	return _int();
}

HRESULT CStageEnd_Trigger::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStageEnd_Trigger::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_StageEnd", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For. Com_Collider
	CCollider::COLLIDER_DESC tDesc;
	tDesc.fRadius = 0.f;
	tDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDesc.vLocalPosition = _vec3(0.f, 2.f, 0.f);
	tDesc.vLocalScale = _vec3(5.f, 5.f, 5.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_Collider", (CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStageEnd_Trigger::SetUp_ConstantTable()
{
	if (m_pTransformCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

	_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(1))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CStageEnd_Trigger::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	m_pColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CStageEnd_Trigger::Set_Billboard()
{
	_matrix matView = m_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3 vRight = *(_vec3*)&matView.m[0];
	_vec3 vUp = *(_vec3*)&matView.m[1];
	_vec3 vLook = *(_vec3*)&matView.m[2];

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	return NOERROR;
}

CStageEnd_Trigger * CStageEnd_Trigger::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStageEnd_Trigger* pInstance = new CStageEnd_Trigger(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CStageEnd_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStageEnd_Trigger::Clone_GameObject(void * pArg)
{
	CStageEnd_Trigger* pInstance = new CStageEnd_Trigger(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CStageEnd_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStageEnd_Trigger::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pObserver);

	CGameObject::Free();
}
