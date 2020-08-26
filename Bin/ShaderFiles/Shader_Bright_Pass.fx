vector      g_vTCDownSampleOffsets[4];
float		g_fBrightPassThreshold;

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

PS_OUT PS_BRIGHTPASS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vAvg = 0.f;

	for (int i = 0; i < 4; i++)
		vAvg += tex2D(SrcSampler, In.vTexUV + float2(g_vTCDownSampleOffsets[i].x, g_vTCDownSampleOffsets[i].y));

	vAvg *= 0.25f;

	float fLuminance = max(vAvg.r, max(vAvg.g, vAvg.b));

	if (fLuminance < g_fBrightPassThreshold)
		vAvg = vector(0.0f, 0.0f, 0.0f, 1.0f);

	Out.vColor = vAvg;

	//Out.vColor = tex2D(SrcSampler, In.vTexUV);

	return Out;
}

technique Default_Technique
{
	pass Bright_Pass_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BRIGHTPASS();
	}
}