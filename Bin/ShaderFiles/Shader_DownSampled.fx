vector      g_vTCDownSampleOffsets[16];

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

PS_OUT PS_DOWNSAMPLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAvg = 0.f;

	for (int i = 0; i < 16; i++)
		vAvg += tex2D(SrcSampler, In.vTexUV + float2(g_vTCDownSampleOffsets[i].x, g_vTCDownSampleOffsets[i].y));

	vAvg *= (1.0f / 16.0f);

	Out.vColor = vAvg;

	return Out;
}

technique Default_Technique
{
	pass DownSample_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DOWNSAMPLE();
	}
}