#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_VRect;
END

BEGIN(Client)

class CUI abstract : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype();// 원형객체
	virtual HRESULT Ready_GameObject_Clone(void* pArg);// 복사객체(실제사용되는객체)
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int LateUpdate_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
protected:
	_float		m_fX, m_fY, m_fSizeX, m_fSizeY;
protected:
	_double		m_TimeDelta = 0.0;
	_double		m_TimeAcc = 0.0;
public:
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END