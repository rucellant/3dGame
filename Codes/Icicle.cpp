#include "stdafx.h"
#include "..\Headers\Icicle.h"
#include "Management.h"
#include "CollisionMgr.h"
#include "Particle_Dead.h"


USING(Client)

CIcicle::CIcicle(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CIcicle::CIcicle(const CIcicle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CIcicle::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CIcicle::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&m_tObjDesc.matWorld.m[3]);

	m_bIsAlive = true;

	return NOERROR;
}

_int CIcicle::Update_GameObject(_double TimeDelta)
{
	// ���⼭ �浹 �˻� ��û
	if (!m_bIsActive) // �÷��̾ ��帧�� �����ڽ��� �浹���� �ʾ��� �� �����ڽ��� �浹�ߴ� ���� ��û�ϴ� �浹 �˻�
	{
		_bool bMode = true;
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Icicle_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &bMode)))
			return E_FAIL;
	}
	else // �÷��̾ ��帧�� �������ڽ��� �浹�ߴ� ���� �˻� ��û�ϴ� �浹 �˻�
	{
		m_TimeAcc += TimeDelta;

		_vec3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fVelocity = m_fVelocity + m_fAccelaration * _float(m_TimeAcc);
		vPosition.y = vPosition.y - fVelocity;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		_bool bMode = false	;
		if (FAILED(CCollisionMgr::GetInstance()->Collision_Icicle_Player(g_eScene, this, L"Layer_Player", L"Com_HitBox", &bMode)))
			return E_FAIL;

		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y <= 18.f)
		{
			// ����Ʈ�� �������
			m_bIsActive = false;
			m_pManagement->Push_GameObject(g_eScene, L"Layer_Interact", this);

			// ���⼭ ��Ƽũ ����
			CParticle_Dead* pParticle = (CParticle_Dead*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Dead");

			if (pParticle == nullptr)
			{
				if (FAILED(m_pManagement->Add_GameObject_Clone(g_eScene, L"Layer_Particle_Dead", L"GameObject_Particle_Dead")))
					return E_FAIL;

				pParticle = (CParticle_Dead*)m_pManagement->Pop_GameObject(g_eScene, L"Layer_Particle_Dead");
			}

			_vec3 vOrigin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			vOrigin.y += 3.f;

			pParticle->Activate(vOrigin);
		}
	}

	// ��帧�� ���� Y�� ���Ϸ� �������ų� �÷��̾�� �浹������ ���� ���·� ��ȯ
	//if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).y <= 10.f || m_bIsCollision)
	//{
	//	// ���⼭ ������ ��ƼŬ �߻���Ű�� ��帧�� ����
	//	m_pManagement->Push_GameObject(g_eScene, L"Layer_Interact", this);
	//}

	m_pDmgColliderCom->Update_Collider();
	m_pIntersectColliderCom->Update_Collider();

	return _int();
}

_int CIcicle::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, m_tObjDesc.fFrustumRadius))
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT CIcicle::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pMeshCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	//m_pDmgColliderCom->Render_Collider();
	//m_pIntersectColliderCom->Render_Collider();

	return NOERROR;
}

HRESULT CIcicle::Add_Component(void * pArg)
{
	m_tObjDesc = *(OBJDESC*)pArg;

	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Mesh_Static", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Frustum
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// For. Com_Mesh
	if (CGameObject::Add_Component(g_eScene, L"Component_Mesh_Icicle", L"Com_Mesh", (CComponent**)&m_pMeshCom))
		return E_FAIL;

	// For. Com_DmgBox
	CCollider::COLLIDER_DESC tDmgDesc;
	tDmgDesc.fRadius = 0.f;
	tDmgDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tDmgDesc.vLocalPosition = _vec3(0.f, 0.f, 0.f);
	tDmgDesc.vLocalScale = _vec3(5.f, 5.f, 5.f);

	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_DmgBox", (CComponent**)&m_pDmgColliderCom, &tDmgDesc))
		return E_FAIL;

	// For. Com_IntersectBox
	CCollider::COLLIDER_DESC tIntersectDesc;
	tIntersectDesc.fRadius = 0.f;
	tIntersectDesc.pTargetMatrix = m_pTransformCom->Get_WorldMatrixPointer();
	tIntersectDesc.vLocalPosition = _vec3(0.f, -40.f, 0.f);
	tIntersectDesc.vLocalScale = _vec3(10.f, 5.f, 10.f);

	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Collider_AABB", L"Com_IntersectBox", (CComponent**)&m_pIntersectColliderCom, &tIntersectDesc))
		return E_FAIL;

	return NOERROR;
}

HRESULT CIcicle::SetUp_ConstantTable()
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

	return NOERROR;
}

HRESULT CIcicle::Render(_uint iPassIndex)
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

CIcicle * CIcicle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CIcicle* pInstance = new CIcicle(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CIcicle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIcicle::Clone_GameObject(void * pArg)
{
	CIcicle* pInstance = new CIcicle(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CIcicle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIcicle::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDmgColliderCom);
	Safe_Release(m_pIntersectColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);

	CGameObject::Free();
}
