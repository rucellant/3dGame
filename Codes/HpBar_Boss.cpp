#include "stdafx.h"
#include "..\Headers\HpBar_Boss.h"
#include "Balrog.h"
#include "Management.h"
#include "Observer_Balrog.h"


USING(Client)


CHpBar_Boss::CHpBar_Boss(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CHpBar_Boss::CHpBar_Boss(const CHpBar_Boss & rhs)
	: CUI(rhs)
{
}

HRESULT CHpBar_Boss::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CHpBar_Boss::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	// ¹ÙÅÁ 
	m_tDesc[0].fSizeX = 410.f;
	m_tDesc[0].fSizeY = 60.f;
	m_tDesc[0].fX = g_iWinCX * 0.5f;
	m_tDesc[0].fY = 0.0f + g_iWinCY * 0.06f;

	// ½ÇÁ¦ hp
	m_tDesc[1].fSizeX = 325.f;
	m_tDesc[1].fSizeY = 5.f;
	m_tDesc[1].fX = g_iWinCX * 0.5f + 30.f;
	m_tDesc[1].fY = 0.0f + g_iWinCY * 0.06f - 2.f;

	// ¶Ñ²±
	m_tDesc[2].fSizeX = 410.f;
	m_tDesc[2].fSizeY = 60.f;
	m_tDesc[2].fX = g_iWinCX * 0.5f;
	m_tDesc[2].fY = 0.0f + g_iWinCY * 0.06f;

	m_pObserver = CObserver_Balrog::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Balrog::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_eType = TYPE_BOSS;

	//m_bIsAlive = true;

	return NOERROR;
}

_int CHpBar_Boss::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta += TimeDelta;

	if (m_TimeDelta >= 10.0)
		m_TimeDelta = 0.0;

	return _int();
}

_int CHpBar_Boss::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CHpBar_Boss::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	for (_uint i = 0; i < 3; i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;

		if (i == 0)
		{
			if (FAILED(Render(2)))
				return E_FAIL;
		}

		if (i == 1)
		{
			if (FAILED(Render(3)))
				return E_FAIL;
		}

		if (i == 2)
		{
			if (FAILED(Render(2)))
				return E_FAIL;
		}
	}

	return NOERROR;
}

HRESULT CHpBar_Boss::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_HpBar_Boss", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CHpBar_Boss::SetUp_ConstantTable(_uint iIndex)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(m_tDesc[iIndex].fX - (g_iWinCX >> 1), (g_iWinCY >> 1) - m_tDesc[iIndex].fY, 0.f));
	m_pTransformCom->Set_Scale(_vec3(m_tDesc[iIndex].fSizeX, m_tDesc[iIndex].fSizeY, 1.f));

	_matrix matTransform;
	D3DXMatrixIdentity(&matTransform);

	_matrix ProjMatrix;
	D3DXMatrixOrthoLH(&ProjMatrix, _float(g_iWinCX), _float(g_iWinCY), 0.f, 1.f);

	_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * matTransform * ProjMatrix;

	m_pShaderCom->Set_Value("g_matWorld", m_pTransformCom->Get_WorldMatrixPointer(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &matTransform, sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_matrix));

	m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix));

	if (iIndex == 0)
		m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(0));
	else if(iIndex == 1)
	{
		m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(1));
		m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(2));

		_float fTimeDelta = _float(m_TimeDelta);
		m_pShaderCom->Set_Value("g_fTimeDelta", &fTimeDelta, sizeof(_float));

		CMonster::MONSTERINFO tInfo = *(CMonster::MONSTERINFO*)m_pObserver->GetData(CSubject_Balrog::TYPE_INFO);
		_float fRatio = tInfo.iCurHp / _float(tInfo.iMaxHp);
		m_pShaderCom->Set_Value("g_fRatio", &fRatio, sizeof(_float));
	}
	else
		m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(3));

	return NOERROR;
}

HRESULT CHpBar_Boss::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CHpBar_Boss * CHpBar_Boss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHpBar_Boss* pInstance = new CHpBar_Boss(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CHpBar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHpBar_Boss::Clone_GameObject(void * pArg)
{
	CHpBar_Boss* pInstance = new CHpBar_Boss(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CHpBar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar_Boss::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	CUI::Free();
}
