#include "stdafx.h"
#include "..\Headers\SK_Slot.h"
#include "Player.h"
#include "Management.h"
#include "Observer_Player.h"


USING(Client)


CSK_Slot::CSK_Slot(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CSK_Slot::CSK_Slot(const CSK_Slot & rhs)
	: CUI(rhs)
{
}

HRESULT CSK_Slot::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSK_Slot::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;
	
	for (int i = 0; i < SK_END; i++)
	{
		m_SK[i].fSizeX = 20.f;
		m_SK[i].fSizeY = 20.f;
		m_SK[i].fX = g_iWinCX * 0.5f - 110.f + 40.f * i;
		m_SK[i].fY = g_iWinCY * 0.8f + 50.f;
		m_SK[i].TimeAcc = 0.0;
		m_SK[i].TimeWait = 5.0;
		m_SK[i].bUsable = true;
	}

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_eType = TYPE_PLAYER;

	m_bIsAlive = true;

	return NOERROR;
}

_int CSK_Slot::Update_GameObject(_double TimeDelta)
{
	/*if (m_pManagement->KeyUp(KEY_UP))
	m_TmpDuration += 0.01;
	if (m_pManagement->KeyUp(KEY_DOWN))
	m_TmpDuration -= 0.01;
	if (m_pManagement->KeyUp(KEY_RIGHT))
	m_TmpPeriod += 0.001;
	if (m_pManagement->KeyUp(KEY_LEFT))
	m_TmpPeriod -= 0.001;*/

	if (FAILED(Cool_Time(TimeDelta)))
		return E_FAIL;

	if (m_pManagement->KeyUp(KEY_1))
	{
		if (FAILED(SetUp_PlayerSK(SK_TORNADO)))
			return -1;
	}
	else if (m_pManagement->KeyUp(KEY_2))
	{
		if (FAILED(SetUp_PlayerSK(SK_SHOULDER)))
			return -1;
	}
	else if (m_pManagement->KeyUp(KEY_3))
	{
		if (FAILED(SetUp_PlayerSK(SK_EARTHQUAKE)))
			return -1;
	}
	else if (m_pManagement->KeyUp(KEY_4))
	{
		if (FAILED(SetUp_PlayerSK(SK_SWORDWAVE)))
			return -1;
	}
	else if (m_pManagement->KeyUp(KEY_5))
	{
		if (FAILED(SetUp_PlayerSK(SK_BUFF)))
			return -1;
	}

	return _int();
}

_int CSK_Slot::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CSK_Slot::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pVIBufferCom == nullptr)
		return E_FAIL;

	for (_uint i = 0; i < SK_END; i++)
	{
		if (FAILED(SetUp_ConstantTable((SK_ID)i)))
			return E_FAIL;

		_uint iPassIndex = 0;

		if (!m_SK[i].bUsable)
			iPassIndex = 1;
		else
			iPassIndex = 0;

		if (FAILED(Render(iPassIndex)))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CSK_Slot::Add_Component(void * pArg)
{
	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_SK", L"Com_Texture", (CComponent**)&m_pSKTextureCom)))
		return E_FAIL;

	// For.Com_BaseTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_SK_Base", L"Com_BaseTexture", (CComponent**)&m_pBaseTextureCom)))
		return E_FAIL;

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_VIBuffer_VRect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSK_Slot::SetUp_ConstantTable(SK_ID eID)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vec3(m_SK[eID].fX - (g_iWinCX >> 1), (g_iWinCY >> 1) - m_SK[eID].fY, 0.f));
	m_pTransformCom->Set_Scale(_vec3(m_SK[eID].fSizeX, m_SK[eID].fSizeY, 1.f));

	_matrix matTransform;
	D3DXMatrixIdentity(&matTransform);

	_matrix ProjMatrix;
	D3DXMatrixOrthoLH(&ProjMatrix, _float(g_iWinCX), _float(g_iWinCY), 0.f, 1.f);

	_matrix matWVP = m_pTransformCom->Get_WorldMatrix() * matTransform * ProjMatrix;

	m_pShaderCom->Set_Value("g_matWorld", m_pTransformCom->Get_WorldMatrixPointer(), sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matView", &matTransform, sizeof(_matrix));
	m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_matrix));

	m_pShaderCom->Set_Value("g_matWVP", &matWVP, sizeof(_matrix));

	m_pShaderCom->Set_Texture("g_SrcTexture", m_pBaseTextureCom->Get_Texture(0));
	m_pShaderCom->Set_Texture("g_DstTexture", m_pSKTextureCom->Get_Texture(_uint(eID)));

	if (!m_SK[eID].bUsable)
	{
		_float fRatio = 1.f - _float(m_SK[eID].TimeAcc / m_SK[eID].TimeWait);
		m_pShaderCom->Set_Value("g_fRatio", &fRatio, sizeof(_float));
	}	

	return NOERROR;
}

HRESULT CSK_Slot::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	m_pVIBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CSK_Slot::Cool_Time(_double TimeDelta)
{
	for (_uint i = 0; i < SK_END; i++)
	{
		if (!m_SK[i].bUsable)
			m_SK[i].TimeAcc += TimeDelta;

		if (m_SK[i].TimeWait <= m_SK[i].TimeAcc)
		{
			m_SK[i].TimeAcc = 0.0;
			m_SK[i].bUsable = true;
		}
	}

	return NOERROR;
}

HRESULT CSK_Slot::SetUp_PlayerSK(SK_ID eID)
{
	if (eID >= SK_END)
		return E_FAIL;

	if (*(CPlayer::STATE*)m_pObserver->GetData(CSubject_Player::TYPE_STATE) == CPlayer::SK)
		return NOERROR;

	if (!m_SK[eID].bUsable)
		return NOERROR;

	HRESULT hr = 0;

	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(SCENE_STAGE, L"Layer_Player");

	switch (eID)
	{
	case SK_TORNADO:
		hr = pPlayer->SetUp_PlayerSK(SK_TORNADO, 0.13, 0.12);
		break;
	case SK_SHOULDER:
		hr = pPlayer->SetUp_PlayerSK(SK_SHOULDER, 0.28, 0.286);
		break;
	case SK_EARTHQUAKE:
		hr = pPlayer->SetUp_PlayerSK(SK_EARTHQUAKE, 0.2, 0.2);
		break;
	case SK_SWORDWAVE:
		hr = pPlayer->SetUp_PlayerSK(SK_SWORDWAVE, 0.2, 0.181);
		break;
	case SK_BUFF:
		hr = pPlayer->SetUp_PlayerSK(SK_BUFF, 0.2, 0.171);
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	m_SK[eID].bUsable = false;

	return NOERROR;
}

CSK_Slot * CSK_Slot::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSK_Slot* pInstance = new CSK_Slot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CSK_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSK_Slot::Clone_GameObject(void * pArg)
{
	CSK_Slot* pInstance = new CSK_Slot(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CSK_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSK_Slot::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pSKTextureCom);
	Safe_Release(m_pBaseTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pObserver);

	CUI::Free();
}
