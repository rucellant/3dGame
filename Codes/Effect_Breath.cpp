#include "stdafx.h"
#include "..\Headers\Effect_Breath.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Observer_Boss.h"


USING(Client)


CEffect_Breath::CEffect_Breath(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Breath::CEffect_Breath(const CEffect_Breath & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Breath::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Breath::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	CTransform::STATEDESC tStateDesc;
	tStateDesc.fRotationPerSec = D3DXToRadian(90.f);
	tStateDesc.fSpeedPerSec = 20.f;

	m_pTransformCom->Set_StateDesc(tStateDesc);

	m_bIsAlive = false;

	m_pObserver_Boss = CObserver_Boss::Create();
	if (m_pObserver_Boss == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Boss::GetInstance()->Subscribe((CObserver*)m_pObserver_Boss)))
		return E_FAIL;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Breath", this);

	/*_vec3 vLook = _vec3(1.f, 1.f, 1.f);
	D3DXVec3Normalize(&vLook, &vLook);

	_vec3 vUp = _vec3(0.f, 1.f, 0.f);

	_vec3 vRight;

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vUp, &vLook, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	qwe = m_pTransformCom->Get_WorldMatrix();*/

	return NOERROR;
}

_int CEffect_Breath::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	m_pColliderCom->Update_Collider();

	m_pTransformCom->Move_Forward(TimeDelta);

	if (!m_bIsDmg)
	{
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Breath_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &m_bIsDmg)))
			return E_FAIL;
	}
	
	if (m_TimeAcc >= 1.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Breath", this);
	}

	return _int();
}

_int CEffect_Breath::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	Set_Billboard();
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CEffect_Breath::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(10)))
		return E_FAIL;

	/*if (m_TimeAcc >= 1.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Breath", this);
	}*/

	m_pColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CEffect_Breath::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver_Boss->GetData(CSubject_Boss::TYPE_MOUSE);

	// Circle
	_vec3 vRight = *(_vec3*)&matTarget.m[0];
	_vec3 vUp = *(_vec3*)&matTarget.m[1];
	_vec3 vLook = *(_vec3*)&matTarget.m[2];
	_vec3 vPosition = *(_vec3*)&matTarget.m[3];

	D3DXVec3Normalize(&vRight, &vRight);
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vLook, &vLook);

	vPosition += vLook;

	_matrix matRotation;
	D3DXMatrixRotationAxis(&matRotation, &vUp, D3DXToRadian(35.f));

	D3DXVec3TransformNormal(&vRight, &vRight, &matRotation);
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3TransformNormal(&vLook, &vLook, &matRotation);
	D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * 3.f);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * 3.f);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * 3.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pColliderCom->Update_Collider();

	m_bIsDmg = false;

	return NOERROR;
}

HRESULT CEffect_Breath::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Breath", L"Com_Texture", (CComponent**)&m_pTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Collider
	CCollider::COLLIDER_DESC tDesc;
	tDesc.fRadius = 0.f;
	tDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDesc.vLocalScale = _vec3(1.f, 1.f, 1.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_Collider", (CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Breath::SetUp_ConstantTable()
{
	if (m_pTransformCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;
	//m_matBillboard
	_matrix matWVP = m_matBillboard * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);
	//_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matBillboard, sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
		return E_FAIL;

	_int iTextureIndex = _int(m_TimeAcc * m_pTextureCom->Get_Size());
	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(iTextureIndex))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Breath::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CEffect_Breath::Set_Billboard()
{
	/*m_matBillboard = m_pTransformCom->Get_WorldMatrix();

	_matrix matView = m_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, nullptr, &matView);

	_vec3 vRight = *(_vec3*)&matView.m[0];
	_vec3 vUp = *(_vec3*)&matView.m[1];
	_vec3 vLook = *(_vec3*)&matView.m[2];

	memcpy((_vec3*)&m_matBillboard.m[0],)
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));*/

	_matrix ViewMatrix = m_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	_vec3 vRight = *(_vec3*)&ViewMatrix.m[0];
	_vec3 vUp = *(_vec3*)&ViewMatrix.m[1];
	_vec3 vLook = *(_vec3*)&ViewMatrix.m[2];

	m_matBillboard = *D3DXMatrixIdentity(&m_matBillboard);

	vRight = vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	vUp = vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP);
	vLook = vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
	_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	memcpy((_vec3*)&m_matBillboard.m[0], &vRight, sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[1], &vUp, sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[2], &vLook, sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[3], &vPosition, sizeof(_vec3));

	/*memcpy((_vec3*)&m_matBillboard.m[0], vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT), sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[1], vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP), sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[2], vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK), sizeof(_vec3));
	memcpy((_vec3*)&m_matBillboard.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION), sizeof(_vec3));*/

	return NOERROR;
}

CEffect_Breath * CEffect_Breath::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Breath* pInstance = new CEffect_Breath(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Breath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Breath::Clone_GameObject(void * pArg)
{
	CEffect_Breath* pInstance = new CEffect_Breath(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Breath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Breath::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pObserver_Boss);

	CEffect::Free();
}
