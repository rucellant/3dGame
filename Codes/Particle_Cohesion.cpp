#include "stdafx.h"
#include "..\Headers\Particle_Cohesion.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "Management.h"
#include "Observer_Player.h"

USING(Client)

CParticle_Cohesion::CParticle_Cohesion(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CParticle(pGraphic_Device)
{
}

CParticle_Cohesion::CParticle_Cohesion(const CParticle_Cohesion & rhs)
	: CParticle(rhs)
{
}

HRESULT CParticle_Cohesion::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CParticle_Cohesion::Ready_GameObject_Clone(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_bIsAlive = false;

	m_pObserver = CObserver_Player::Create();
	if (m_pObserver == nullptr)
		return E_FAIL;

	if (FAILED(CSubject_Player::GetInstance()->Subscribe((CObserver*)m_pObserver)))
		return E_FAIL;

	m_pManagement->Push_GameObject(g_eScene, L"Layer_Particle_Cohesion", this);

	return NOERROR;
}

_int CParticle_Cohesion::Update_GameObject(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_RIGHTHAND);

	m_vOrigin = *(_vec3*)&matTarget.m[3];

	// 파티클 초기화
	_int iEmitPerFrame = 0;

	for (auto& Attribute : m_ParticleLst)
	{
		if (m_TimeAcc >= 0.5)
			break;

		if (!Attribute.bIsAlive)
		{
			Attribute.bIsAlive = true;
			Attribute.vAcceleration = _vec3(0.f, 0.f, 0.f);
			Attribute.vDir = _vec3(0.f, 0.f, 0.f);
			_vec3 vPosition;
			GetRandVec(&vPosition, &_vec3(-5.f, -5.f, -5.f), &_vec3(5.f, 5.f, 5.f));
			Attribute.vPosition = vPosition + m_vOrigin;
			Attribute.vVelocity = _vec3(0.f, 0.f, 0.f);

			iEmitPerFrame++;
		}

		if (iEmitPerFrame >= _int(m_fEmitRate / 60.f))
			break;
	}

	for (auto& Attribute : m_ParticleLst)
	{
		if (!Attribute.bIsAlive)
			continue;

		_vec3 vDir = m_vOrigin - Attribute.vPosition;
		_float fLength = D3DXVec3Length(&vDir);
		
		if (fLength <= 0.1f)
		{
			Attribute.bIsAlive = false;
			continue;
		}

		D3DXVec3Normalize(&vDir, &vDir);
		Attribute.vPosition += (vDir * _float(TimeDelta) * 10.f);
	}

	return _int();
}

_int CParticle_Cohesion::LateUpdate_GameObject(_double TimeDelta)
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

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT CParticle_Cohesion::Render_GameObject()
{
	if (m_pShaderCom == nullptr || m_pTextureCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(Render(0)))
		return E_FAIL;

	m_RenderingParticleLst.clear();

	if (m_TimeAcc >= 1.0)
	{
		m_TimeAcc = 0.0;
		m_pManagement->Push_GameObject(g_eScene, L"Layer_Particle_Cohesion", this);
	}

	return NOERROR;
}

HRESULT CParticle_Cohesion::Activate()
{
	_matrix matTarget = *(_matrix*)m_pObserver->GetData(CSubject_Player::TYPE_RIGHTHAND);

	m_vOrigin = *(_vec3*)&matTarget.m[3];

	if (m_ParticleLst.empty()) // 비었다!
	{
		for (_uint i = 0; i < m_iNumParticle; i++)
		{
			ATTRIBUTE tAttr;
			tAttr.bIsAlive = false;
			tAttr.vAcceleration = _vec3(0.f, 0.f, 0.f);
			tAttr.vPosition = _vec3(0.f, 0.f, 0.f);
			tAttr.vVelocity = _vec3(0.f, 0.f, 0.f);
			tAttr.vDir = _vec3(0.f, 0.f, 0.f);

			m_ParticleLst.push_back(tAttr);
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOrigin);

	return NOERROR;
}

HRESULT CParticle_Cohesion::Add_Component(void * pArg)
{
	// For. Com_Shader
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Shader_Particle", L"Com_Shader", (CComponent**)&m_pShaderCom))
		return E_FAIL;

	// For. Com_Texture
	if (CGameObject::Add_Component(SCENE_STATIC, L"Component_Texture_Particle_Cohesion", L"Com_Texture", (CComponent**)&m_pTextureCom))
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

HRESULT CParticle_Cohesion::SetUp_ConstantTable()
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

HRESULT CParticle_Cohesion::Render(_uint iPassIndex)
{
	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(iPassIndex);

	if (FAILED(Render_Particle()))
		return E_FAIL;

	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CParticle_Cohesion::Render_Particle()
{
	LPDIRECT3DDEVICE9 pGraphic_Device = Get_Graphic_Device();

	if (pGraphic_Device == nullptr || m_pVB == nullptr)
		return E_FAIL;

	Safe_AddRef(pGraphic_Device);

	pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iStride);
	pGraphic_Device->SetFVF(m_dwFVF);

	Safe_Release(pGraphic_Device);

	/*for (auto& Attribute : m_RenderingParticleLst)
		Attribute.vPosition += m_vOrigin;*/

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

CParticle_Cohesion * CParticle_Cohesion::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticle_Cohesion* pInstance = new CParticle_Cohesion(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed while Creating CParticle_Cohesion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticle_Cohesion::Clone_GameObject(void * pArg)
{
	CParticle_Cohesion* pInstance = new CParticle_Cohesion(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pArg)))
	{
		MSG_BOX("Failed while Creating CParticle_Cohesion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Cohesion::Free()
{
	m_ParticleLst.clear();

	Safe_Release(m_pObserver);

	CParticle::Free();
}
