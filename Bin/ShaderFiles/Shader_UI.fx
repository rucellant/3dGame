matrix g_matWVP, g_matWorld, g_matView, g_matProj;

float g_fRatio;

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

struct VS_IN
{
	float3	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	vector vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
};

PS_OUT PS_SK_SLOT_USABLE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vBase = tex2D(SrcSampler, In.vTexUV);

	vector vSK = tex2D(DstSampler, In.vTexUV);

	Out.vColor = vBase + vSK;

	return Out;
}

PS_OUT PS_SK_SLOT_UNUSABLE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vBase = tex2D(SrcSampler, In.vTexUV);

	vector vSK = tex2D(DstSampler, In.vTexUV);

	if (In.vTexUV.y <= g_fRatio)
		vSK *= 0.3f;

	Out.vColor = vBase + vSK;

	return Out;
}

technique Default_Technique
{
	pass Slot_Usable_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SK_SLOT_USABLE();
	}

	pass Slot_Unusable_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SK_SLOT_UNUSABLE();
	}
}