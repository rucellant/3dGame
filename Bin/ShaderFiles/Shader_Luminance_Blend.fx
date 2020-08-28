vector      g_vTCLumOffsets[4];
vector      g_vTCDSOffsets[9];

float	g_fPosX;

texture		g_LumTexture;

sampler LumSampler = sampler_state
{
	texture = g_LumTexture;
	magfilter = linear;
	minfilter = linear;
};

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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = tex2D(LumSampler, float2(0.5f, 0.5f));

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_DOWNSAMPLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = 0.f;

	for (int i = 0; i < 4; i++)
		vColor += tex2D(SrcSampler, In.vTexUV + float2(g_vTCLumOffsets[i].x, g_vTCLumOffsets[i].y));

	vColor /= 4.f;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_DOWNSAMPLE_SECOND(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = 0.f;

	for (int i = 0; i < 9; i++)
		vColor += tex2D(SrcSampler, In.vTexUV + float2(g_vTCDSOffsets[i].x, g_vTCDSOffsets[i].y));

	vColor /= 9.f;

	Out.vColor = vColor;
	
	return Out;
}

technique Default_Technique
{
	pass Luminance_Blend_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Luminance_DownSample_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DOWNSAMPLE();
	}

	pass Luminance_DownSample2_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DOWNSAMPLE_SECOND();
	}
}