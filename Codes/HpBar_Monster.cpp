#include "stdafx.h"
#include "..\Headers\HpBar_Monster.h"
#include "Monster.h"
#include "Management.h"


USING(Client)

CHpBar_Monster::CHpBar_Monster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CHpBar_Monster::CHpBar_Monster(const CHpBar_Monster & rhs)
	: CUI(rhs)
{
}

HRESULT CHpBar_Monster::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CHpBar_Monster::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTargetMonster = (CMonster*)pArg;
	if (m_pTargetMonster == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_OnTarget()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vec3(2.f, 0.2f, 2.f));

	m_bIsAlive = true;

	return NOERROR;
}

_int CHpBar_Monster::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 10.0)
		m_TimeAcc = 0.0;

	SetUp_OnTarget();

	return _int();
}

_int CHpBar_Monster::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (!m_pFrustumCom->Culling_ToFrustum(m_pTransformCom, 2.f) && m_pTargetMonster->GetIsActive())
	{
		Set_Billboard();
		Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);
	}
		

	return _int();
}

HRESULT CHpBar_Monster::Render_GameObject()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;

		if (FAILED(Render(i + 2)))
			return E_FAIL;
	}
	

	return NOERROR;
}

HRESULT CHpBar_Monster::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For. Com_Frustum
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Frustum", L"Com_Frustum", (CComponent**)&m_pFrustumCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_HpBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CHpBar_Monster::SetUp_ConstantTable(_uint iIndex)
{
	if (m_pTransformCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

	if (iIndex == 0)
	{
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
	}		
	else
	{
		_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * m_pManagement->Get_Transform(D3DTS_VIEW) * m_pManagement->Get_Transform(D3DTS_PROJECTION);

		*(_vec3*)&matWVP.m[0] = 0.81f * *(_vec3*)&matWVP.m[0];
		*(_vec3*)&matWVP.m[1] = 0.5f * *(_vec3*)&matWVP.m[1];

		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix))))
			return E_FAIL;

		m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(1));
		m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(2));

		_float fTimeDelta = _float(m_TimeAcc);
		m_pShaderCom->Set_Value("g_fTimeDelta", &fTimeDelta, sizeof(_float));

		CMonster::MONSTERINFO tInfo = m_pTargetMonster->GetMonsterInfo();

		_float fRatio = tInfo.iCurHp / _float(tInfo.iMaxHp);
		m_pShaderCom->Set_Value("g_fRatio", &fRatio, sizeof(_float));
	}

	

	return NOERROR;
}

HRESULT CHpBar_Monster::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CHpBar_Monster::SetUp_OnTarget()
{
	CMesh_Dynamic* pTargetMesh = (CMesh_Dynamic*)m_pTargetMonster->Get_Component(L"Com_Mesh");
	if (pTargetMesh == nullptr)
		return E_FAIL;

	D3DXFRAME_DERIVED* pFrame_Derived = pTargetMesh->Get_FrameDerived("Bip01");
	if (pFrame_Derived == nullptr)
		return E_FAIL;

	_matrix matTargetWorld = ((CTransform*)m_pTargetMonster->Get_Component(L"Com_Transform"))->Get_WorldMatrix();

	_matrix matWorld = pFrame_Derived->CombinedTransformationMatrix * matTargetWorld;
	matWorld.m[3][1] += 2.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, *(_vec3*)&matWorld.m[3]);

	return NOERROR;
}

HRESULT CHpBar_Monster::Set_Billboard()
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

CHpBar_Monster * CHpBar_Monster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpBar_Monster* pInstance = new CHpBar_Monster(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CHpBar_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpBar_Monster::Clone_GameObject(void * pArg)
{
	CHpBar_Monster* pInstance = new CHpBar_Monster(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CHpBar_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Monster::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pFrustumCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pTargetMonster);

	CUI::Free();
}
