matrix		g_matWorld, g_matView, g_matProj;

float		g_fSize, g_fViewPortHeight;

texture		g_ParticleTexture;

sampler	ParticleSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_ParticleTexture;
};


struct VS_IN
{
	float3	vPosition : POSITION;
	float4	vColor : COLOR;
	float2	vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float4	vColor : COLOR;
	float2	vTexUV : TEXCOORD0;
	float	vSize : PSIZE;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	// 월드변환 

	vector		vPosition;

	vPosition = vector(In.vPosition, 1.f);

	vPosition = mul(vPosition, g_matView);

	float fDistance = distance(vector(0.f, 0.f, 0.f, 0.f), vPosition);

	vPosition = mul(vPosition, g_matProj);

	float fSize = (g_fSize * g_fViewPortHeight) / fDistance;

	Out.vPosition = vPosition;
	Out.vColor = In.vColor;//vector(1.f, 1.f, 1.f, 1.f);
	Out.vTexUV = In.vTexUV;
	Out.vSize = fSize;

	return Out;
}

struct PS_IN
{
	float4	vPosition : POSITION;
	float4	vColor : COLOR;
	float2	vTexUV : TEXCOORD0;
	float	vSize : PSIZE;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In) : COLOR
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = tex2D(ParticleSampler, In.vTexUV);

	Out.vColor = Out.vColor * vector(1.f, 1.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_BUFF(PS_IN In) : COLOR
{
	PS_OUT		Out = (PS_OUT)0;

Out.vColor = tex2D(ParticleSampler, In.vTexUV);

Out.vColor = Out.vColor * In.vColor;

return Out;
}

technique Default_Technique
{
	pass Particle_Rendering
	{
		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha; //SrcAlpha;
		DestBlend = InvSrcAlpha;//one;

		PointSpriteEnable = true;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}