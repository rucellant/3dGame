matrix g_matWVP, g_matWorld, g_matView, g_matProj;

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

PS_OUT PS_SK_SLOT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vBase = tex2D(SrcSampler, In.vTexUV);

	vector vSK = tex2D(DstSampler, In.vTexUV);

	Out.vColor = vBase + vSK;

	return Out;
}

technique Default_Technique
{
	pass Background_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SK_SLOT();
	}
}