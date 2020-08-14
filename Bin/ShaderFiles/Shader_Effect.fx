matrix g_matWVP, g_matWorld, g_matView, g_matProj;

float g_fTimeAcc;

texture g_SrcTexture;

sampler	SrcSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_SrcTexture;
};

texture g_DstTexture;

sampler	DstSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_DstTexture;
};

texture g_DepthTexture;

sampler	DepthSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_DepthTexture;
};

struct VS_IN
{
	float3	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	vector vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
};

PS_OUT PS_PORTAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSrc = tex2D(SrcSampler, In.vTexUV);

	vSrc *= 0.5f;

	float2 vShadeTexUV = In.vTexUV;

	//vShadeTexUV.x = 3.f * cos(2.f * g_fTimeAcc + In.vTexUV.x * 10.f);
	vShadeTexUV.y = vShadeTexUV.y - g_fTimeAcc * 0.2f;

	vector vDst = tex2D(DstSampler, vShadeTexUV);
	
	Out.vColor = vSrc * vDst;

	float2 vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthInfo = tex2D(DepthSampler, vTexUV);

	float fOldViewZ = vDepthInfo.y * 500.0f;
	float fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ) * 2.f;

	return Out;
}

technique Default_Technique
{
	pass Portal_Rendering
	{
		cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = one;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_PORTAL();
	}
}