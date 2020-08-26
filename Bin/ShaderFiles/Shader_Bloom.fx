float g_arrHBloomWeights[9];
float g_arrHBloomOffsets[9];
float g_arrVBloomWeights[9];
float g_arrVBloomOffsets[9];

texture		g_SrcTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SrcTexture;
	magfilter = linear;
	minfilter = linear;
};

struct PS_IN
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
};

PS_OUT PS_HBLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = 0.f;

	for (int i = 0; i < 9; i++)
		vColor += (tex2D(SrcSampler, In.vTexUV + float2(g_arrHBloomOffsets[i], 0.f)) * g_arrHBloomWeights[i]);

	Out.vColor = vector(vColor.rgb, 1.f);

	return Out;
}

PS_OUT PS_VBLOOM(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = 0.f;

	for (int i = 0; i < 9; i++)
		vColor += (tex2D(SrcSampler, In.vTexUV + float2(0.f, g_arrVBloomOffsets[i])) * g_arrVBloomWeights[i]);

	Out.vColor = vector(vColor.rgb, 1.f);

	return Out;
}

technique Default_Technique
{
	pass HBloom_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_HBLOOM();
	}

	pass VBloom_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_VBLOOM();
	}
}