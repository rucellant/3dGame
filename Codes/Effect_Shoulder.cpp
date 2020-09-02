#include "stdafx.h"
#include "..\Headers\Effect_Shoulder.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CEffect_Shoulder::CEffect_Shoulder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{
}

CEffect_Shoulder::CEffect_Shoulder(const CEffect_Shoulder & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Shoulder::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Shoulder::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_bIsAlive = false;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Shoulder", this);

	return NOERROR;
}

_int CEffect_Shoulder::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;
	m_TimeDelta = TimeDelta;

	m_pTransformCom->Set_Scale(_vec3(_float(m_TimeAcc * 3.0) + 1.f, _float(m_TimeAcc * 3.0) + 1.f, _float(m_TimeAcc * 3.0) + 1.f));

	return _int();
}

_int CEffect_Shoulder::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CEffect_Shoulder::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(3)))
		return E_FAIL;

	if (m_TimeAcc >= 1.5)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Effect_Shoulder", this);
	}

	return NOERROR;
}

HRESULT CEffect_Shoulder::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_MATRIX);

	_vec3 vRight = *(_vec3*)matTarget.m[0];
	_vec3 vUp = *(_vec3*)matTarget.m[1];
	_vec3 vLook = *(_vec3*)matTarget.m[2];
	_vec3 vPosition = *(_vec3*)matTarget.m[3];

	D3DXVec3Normalize(&vLook, &vLook);

	vPosition += vLook * 2.f;
	vPosition.y += 2.f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 1.f));

	return NOERROR;
}

HRESULT CEffect_Shoulder::Add_Component(void * pArg)
{
	// For Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture_Wave
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Shoulder_Wave", L"Com_Texture_Wave", (CComponent**)&m_pWaveTextureCom))
		return E_FAIL;

	// For. Com_Texture_Diffuse
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Effect_Shoulder_Diffuse", L"Com_Texture_Diffuse", (CComponent**)&m_pDiffuseTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Mesh
	/*if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Mesh_Effect_Shoulder_Circle", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;*/

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Shoulder::SetUp_ConstantTable()
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

	if (FAILED(m_pShaderCom->Set_Texture("g_SrcTexture", m_pWaveTextureCom->Get_Texture(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Texture("g_DstTexture", m_pWaveTextureCom->Get_Texture(1))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pDiffuseTextureCom->Get_Texture(0))))
		return E_FAIL;

	_float fTheta = D3DXToRadian(_float(m_TimeAcc) * 360.f);
	if (FAILED(m_pShaderCom->Set_Value("g_fAngle", &fTheta, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Shoulder::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	/*_ulong dwNumSubset = m_pMeshCom->Get_NumSubset();

	for (_ulong i = 0; i < dwNumSubset; ++i)
		m_pMeshCom->Render_Mesh(i);*/

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CEffect_Shoulder * CEffect_Shoulder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Shoulder* pInstance = new CEffect_Shoulder(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CEffect_Shoulder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Shoulder::Clone_GameObject(void * pArg)
{
	CEffect_Shoulder* pInstance = new CEffect_Shoulder(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CEffect_Shoulder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Shoulder::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pWaveTextureCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pMeshCom);

	Safe_Release(m_pObserver);

	CEffect::Free();
}
