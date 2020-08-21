#include "stdafx.h"
#include "..\Headers\MpBar.h"
#include "Player.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CMpBar::CMpBar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CMpBar::CMpBar(const CMpBar & rhs)
	: CUI(rhs)
{
}

HRESULT CMpBar::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CMpBar::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	// ¹ÙÅÁ
	m_tDesc[0].fSizeX = 140.f;
	m_tDesc[0].fSizeY = 10.f;
	m_tDesc[0].fX = g_iWinCX * 0.5f + 180.f;
	m_tDesc[0].fY = 0.0f + g_iWinCY * 0.8f;

	// Ã¼·Â
	m_tDesc[1].fSizeX = 114.5f;
	m_tDesc[1].fSizeY = 6.f;
	m_tDesc[1].fX = g_iWinCX * 0.5f + 180.f;
	m_tDesc[1].fY = g_iWinCY * 0.8f;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_eType = TYPE_PLAYER;

	m_bIsAlive = true;

	return NOERROR;
}

_int CMpBar::Update_GameObject(_double TimeDelta)
{
	m_TimeDelta += TimeDelta;

	if (m_TimeDelta >= 10.0)
		m_TimeDelta = 0.0;

	return _int();
}

_int CMpBar::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMpBar::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	for (_uint i = 0; i < 2; i++)
	{
		if (FAILED(SetUp_ConstantTable(i)))
			return E_FAIL;
		
		if (FAILED(Render((i + 1) * 2)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CMpBar::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_MpBar", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMpBar::SetUp_ConstantTable(_uint iIndex)
{
	if (m_pTransformCom == nullptr || m_pShaderCom == nullptr)
		return E_FAIL;

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
	else
	{
		m_pShaderCom->Set_Texture("g_SrcTexture", m_pTextureCom->Get_Texture(1));
		m_pShaderCom->Set_Texture("g_DstTexture", m_pTextureCom->Get_Texture(2));

		_float fTimeDelta = _float(m_TimeDelta);
		m_pShaderCom->Set_Value("g_fTimeDelta", &fTimeDelta, sizeof(_float));

		CPlayer::PLAYERINFO tInfo = *(CPlayer::PLAYERINFO*)m_pObserver->GetData(CSubject_Player::TYPE_INFO);
		_float fRatio = tInfo.iCurMp / _float(tInfo.iMaxMp);
		m_pShaderCom->Set_Value("g_fRatio", &fRatio, sizeof(_float));
	}

	return NOERROR;
}

HRESULT CMpBar::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

CMpBar * CMpBar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMpBar* pInstance = new CMpBar(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CMpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMpBar::Clone_GameObject(void * pArg)
{
	CMpBar* pInstance = new CMpBar(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CMpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMpBar::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pObserver);

	CUI::Free();
}
