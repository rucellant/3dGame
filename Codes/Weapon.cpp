#include "stdafx.h"
#include "..\Headers\Weapon.h"
#include "Management.h"


USING(Client)

CWeapon::CWeapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CWeapon::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(0.2f, 0.2f, 0.2f));

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	Update_Collider();

	m_bIsAlive = true;

	return NOERROR;
}

_int CWeapon::Update_GameObject(_double TimeDelta)
{
	Update_Matrix();

	Update_Collider();

	return _int();
}

_int CWeapon::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CWeapon::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	m_pColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CWeapon::Add_Component(void * pArg)
{
	// For. Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Weapon", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For. Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For. Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For. Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Weapon", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// For. Com_Collider
	CCollider::COLLIDER_DESC tDesc;
	tDesc.fRadius = 0.f;
	tDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDesc.vLocalPosition = _vec3(0.f, 10.f, 0.f);
	tDesc.vLocalScale = _vec3(2.f, 20.f, 2.f);

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_OBB", L"Com_Collider", (CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeapon::SetUp_ConstantTable()
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
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

	_matrix matCamera = m_pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixInverse(&matCamera, nullptr, &CManagement::GetInstance()->Get_Transform(D3DTS_VIEW));

	if (FAILED(m_pShaderCom->Set_Value("g_vCamPosition", &matCamera.m[3][0], sizeof(_vec4))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeapon::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	_ulong dwNumSubset = m_pMeshCom->Get_NumSubset();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_Texture("g_NormalTexture", m_pMeshCom->Get_MaterialTexture(i, MESHTEXTURE::TYPE_NORMAL))))
			return E_FAIL;

		m_pShaderCom->Commit_Change();

		m_pMeshCom->Render_Mesh(i);
	}

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CWeapon::Update_Matrix()
{
	_matrix matWorld = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_RIGHTHAND);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_vec3*)&matWorld.m[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_vec3*)&matWorld.m[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_vec3*)&matWorld.m[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&matWorld.m[3]);
	m_pTransformCom->Set_Scale(_vec3(0.2f, 0.2f, 0.2f));

	return NOERROR;
}

HRESULT CWeapon::Update_Collider()
{
	m_pColliderCom->Update_Collider();

	return NOERROR;
}

CWeapon * CWeapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon* pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeapon::Clone_GameObject(void * pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	Safe_Release(m_pObserver);

	CGameObject::Free();
}
