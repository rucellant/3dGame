

// 방향성 광원.
vector		g_vLightDir;

// 점광원 
vector		g_vLightPos;
float		g_fRange;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector		g_vCamPosition;

matrix		g_matViewInv;
matrix		g_matProjInv;

float		g_fPower = 50.f;

float		g_fFar = 500.f;

// 노멀 타겟에 대한 정보.
texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};


struct PS_IN
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector  vShade : COLOR0;
	vector  vSpecular : COLOR1;
};

PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float	fViewZ = vDepthInfo.y * g_fFar;

	// xyz -> 0 ~ 1
	// xyz -> -1 ~ 1
	vector	vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);	

	// 빛 : 1001
	// 명암 : 1001

	vector	vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector	vPosition;

	// 투영스페이스 상의 2차원 좌표. (월드스페이스 * 뷰스페이스 * 투영행렬 / w)
	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.r;
	vPosition.w = 1.0f;

	// 월드스페이스 * 뷰스페이스 * 투영행렬.
	vPosition = vPosition * fViewZ;

	// 월드스페이스 * 뷰스페이스 
	vPosition = mul(vPosition, g_matProjInv);

	// 월드스페이스 
	vPosition = mul(vPosition, g_matViewInv);

	vector	vLook = vPosition - g_vCamPosition;

	float	fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), g_fPower);
	

	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse) * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float	fViewZ = vDepthInfo.y * g_fFar;

	vector	vPosition;

	vPosition.x = In.vTexUV.x * 2.f - 1.f;
	vPosition.y = In.vTexUV.y * -2.f + 1.f;
	vPosition.z = vDepthInfo.r;
	vPosition.w = 1.0f;

	vPosition = vPosition * fViewZ;
	vPosition = mul(vPosition, g_matProjInv);
	vPosition = mul(vPosition, g_matViewInv);

	vector	vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	vector	vLightDir = vPosition - g_vLightPos;
	float	fDistance = length(vLightDir);
	vector	vReflect = reflect(normalize(vLightDir), vNormal);	
	vector	vLook = vPosition - g_vCamPosition;
	float	fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), g_fPower);
	float	fAtt = saturate((g_fRange - fDistance) / g_fRange);


	Out.vShade = (g_vLightDiffuse * g_vMtrlDiffuse);
	Out.vShade = Out.vShade * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade = Out.vShade * fAtt;
	Out.vShade.a = 1.f;

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

technique Default_Technique
{
	pass Directional
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_DIRECTIONAL();
	}		

	pass Point
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}
}