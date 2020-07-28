#pragma once

#include "Base.h"
#include "Terrain.h"
#include "Client_Defines.h"

BEGIN(Client)

class CIOManager final : public CBase
{
	DECLARE_SINGLETON(CIOManager)
private:
	explicit CIOManager();
	virtual ~CIOManager() = default;
public:

private:

public:
	virtual void Free();
};

END