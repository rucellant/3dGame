#include "stdafx.h"
#include "..\Headers\Particle_Crystal.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "Management.h"


USING(Client)


CParticle_Crystal::CParticle_Crystal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle(pGraphic_Device)
{
}

CParticle_Crystal::CParticle_Crystal(const CParticle_Crystal & rhs)
	: CParticle(rhs)
{
}

_bool Compare_View(const CParticle_Crystal::ATTRIBUTE & a, const CParticle_Crystal::ATTRIBUTE & b)
{
	_matrix	matView;

	D3DXMatrixInverse(&matView, nullptr, &CManagement::GetInstance()->Get_Transform(D3DTS_VIEW));

	_float fViewA = D3DXVec3Length(&(a.vPosition - *(_vec3*)&matView.m[3][0]));
	_float fViewB = D3DXVec3Length(&(b.vPosition - *(_vec3*)&matView.m[3][0]));

	if (fViewA >= fViewB)
		return false;
	else
		return true;
}

HRESULT CParticle_Crystal::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CParticle_Crystal::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_bIsAlive = false;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Particle_Crystal", this);

	return NOERROR;
}

_int CParticle_Crystal::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	for (auto& Attribute : m_ParticleLst)
	{
		if (!Attribute.bIsAlive)
		{
			Attribute.bIsAlive = true;
			Attribute.fAge = 0.f;
			Attribute.fLifeTime = 3.f;
			Attribute.vAcceleration = _vec3(0.f, 10.f, 0.f);
			Attribute.vColorFade = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			Attribute.vCurColor = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			Attribute.vPosition = _vec3(0.f, 0.f, 0.f);
			Attribute.vVelocity = _vec3(0.f, 10.f, 0.f);

			_vec3 vRandVec;
			GetRandVec(&vRandVec, &_vec3(-1.f, -1.f, -1.f), &_vec3(1.f, 1.f, 1.f));
			//vRandVec.y = 0.f;
			D3DXVec3Normalize(&vRandVec, &vRandVec);

			Attribute.vDir = vRandVec;
		}
		
		Attribute.fAge += _float(TimeDelta);
		
		if (Attribute.fLifeTime <= Attribute.fAge)
			Attribute.bIsAlive = false;
		else
			Attribute.vPosition += (Attribute.vDir * _float(TimeDelta * 1.5));
	}

	return _int();
}

_int CParticle_Crystal::LateUpdate_GameObject(_double TimeDelta)
{
	if (m_pRendererCom == nullptr)
		return -1;

	if (FAILED(Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
		return -1;

	for (auto& Attribute : m_ParticleLst)
	{
		if (Attribute.bIsAlive)
			m_RenderingParticleLst.push_back(Attribute);
	}

	//m_RenderingParticleLst.sort(Compare_View);

	if (m_bIsAlive)
		m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CParticle_Crystal::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pTextureCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	m_RenderingParticleLst.clear();

	if (m_TimeAcc >= 3.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Particle_Crystal", this);
	}

	return NOERROR;
}

HRESULT CParticle_Crystal::Activate(_vec3 vOrigin)
{
	m_vOrigin = vOrigin;

	if (m_ParticleLst.empty()) // ºñ¾ú´Ù!
	{
		for (_uint i = 0; i < m_iNumParticle; i++)
		{
			ATTRIBUTE tAttr;
			tAttr.bIsAlive = true;
			tAttr.fAge = 0.f;
			tAttr.fLifeTime = 3.f;
			tAttr.vAcceleration = _vec3(0.f, 10.f, 0.f);
			tAttr.vColorFade = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			tAttr.vCurColor = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			tAttr.vPosition = _vec3(0.f, 0.f, 0.f);
			tAttr.vVelocity = _vec3(0.f, 10.f, 0.f);

			_vec3 vRandVec;
			GetRandVec(&vRandVec, &_vec3(-1.f, -1.f, -1.f), &_vec3(1.f, 1.f, 1.f));
			//vRandVec.y = 0.f;
			D3DXVec3Normalize(&vRandVec, &vRandVec);
		
			tAttr.vDir = vRandVec;

			m_ParticleLst.push_back(tAttr);
		}
	}
	else
	{
		for (auto& Attribute : m_ParticleLst)
		{
			Attribute.bIsAlive = true;
			Attribute.fAge = 0.f;
			Attribute.fLifeTime = 3.f;
			Attribute.vAcceleration = _vec3(0.f, 10.f, 0.f);
			Attribute.vColorFade = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			Attribute.vCurColor = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
			Attribute.vPosition = _vec3(0.f, 0.f, 0.f);
			Attribute.vVelocity = _vec3(0.f, 10.f, 0.f);

			_vec3 vRandVec;
			GetRandVec(&vRandVec, &_vec3(-1.f, -1.f, -1.f), &_vec3(1.f, 1.f, 1.f));
			//vRandVec.y = 0.f;
			D3DXVec3Normalize(&vRandVec, &vRandVec);

			Attribute.vDir = vRandVec;
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOrigin);

	return NOERROR;
}

HRESULT CParticle_Crystal::Add_Component(void * pArg)
{
	// For. Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Particle", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Particle_Crystal", L"Com_Texture", (CComponent**)&m_pTextureCom))
		return E_FAIL;

	// For. Com_Renderer
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom))
		return E_FAIL;

	// For. Com_Transform
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom))
		return E_FAIL;

	// Description
	m_fEmitRate = 256.f;
	m_fSize = 0.7f;
	m_iNumParticle = 256;
	m_vOrigin = _vec3(0.f, 0.f, 0.f);

	m_dwVBSize = 128;
	m_dwVBOffset = 0;
	m_dwVBBatchSize = 32;

	D3DVIEWPORT9 ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);
	m_fViewportHeight = _float(ViewPort.Height);

	// VertexBuffer
	m_iStride = sizeof(VTXP);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
	if (FAILED(Get_Graphic_Device()->CreateVertexBuffer(
		m_iStride * m_dwVBSize,
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		m_dwFVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		nullptr)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CParticle_Crystal::SetUp_ConstantTable()
{
	if (m_pShaderCom == nullptr || m_pTransformCom == nullptr)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMatrix(), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_matrix))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Texture("g_ParticleTexture", m_pTextureCom->Get_Texture(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fViewPortHeight", &m_fViewportHeight, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSize", &m_fSize, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

HRESULT CParticle_Crystal::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	if (FAILED(Render_Particle()))
		return E_FAIL;

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CParticle_Crystal::Render_Particle()
{
	LPDIRECT3DDEVICE9 pGraphic_Device = Get_Graphic_Device();

	if (pGraphic_Device == nullptr || m_pVB == nullptr)
		return E_FAIL;

	Safe_AddRef(pGraphic_Device);

	pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
	pGraphic_Device->SetFVF(m_dwFVF);

	Safe_Release(pGraphic_Device);

	for (auto& Attribute : m_RenderingParticleLst)
		Attribute.vPosition += m_vOrigin;

	if (m_dwVBOffset >= m_dwVBSize)
		m_dwVBOffset = 0;

	VTXP* v = 0;

	m_pVB->Lock(
		m_dwVBOffset * m_iStride,
		m_dwVBBatchSize * m_iStride,
		(void**)&v,
		m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	_ulong dwNumParticlesInBatch = 0;

	for (auto& Attribute : m_RenderingParticleLst)
	{
		if (Attribute.bIsAlive)
		{
			v->vPosition = Attribute.vPosition;
			v++;

			dwNumParticlesInBatch++;

			if (dwNumParticlesInBatch == m_dwVBBatchSize)
			{
				m_pVB->Unlock();

				HRESULT hr = 0;
				hr = pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

				m_dwVBOffset += m_dwVBBatchSize;

				if (m_dwVBOffset >= m_dwVBSize)
					m_dwVBOffset = 0;

				m_pVB->Lock(
					m_dwVBOffset * m_iStride,
					m_dwVBBatchSize * m_iStride,
					(void**)&v,
					m_dwVBOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				dwNumParticlesInBatch = 0;
			}
		}
	}

	m_pVB->Unlock();

	HRESULT hr = 0;

	if (dwNumParticlesInBatch)
		hr = pGraphic_Device->DrawPrimitive(D3DPT_POINTLIST, m_dwVBOffset, m_dwVBBatchSize);

	m_dwVBOffset += m_dwVBBatchSize;

	return NOERROR;
}

CParticle_Crystal * CParticle_Crystal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticle_Crystal* pInstance = new CParticle_Crystal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CParticle_Crystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Crystal::Clone_GameObject(void * pArg)
{
	CParticle_Crystal* pInstance = new CParticle_Crystal(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CParticle_Crystal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Crystal::Free()
{
	m_ParticleLst.clear();

	CParticle::Free();
}
