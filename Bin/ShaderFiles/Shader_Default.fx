matrix g_matWVP, g_matWorld, g_matView, g_matProj;

texture g_DiffuseTexture;

sampler	DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
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

PS_OUT PS_BACKGROUND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

technique Default_Technique
{
	pass Background_Rendering
	{
		AlphaBlendEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BACKGROUND();
	}
}