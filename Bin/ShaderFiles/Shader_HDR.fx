float       g_fExposure;                          // A user configurable bias to under/over expose the image
float       g_fGaussianScalar;                    // Used in the post-processing, but also useful here
float       g_rcp_bloom_tex_w;                  // The reciprocal WIDTH of the texture in 'bloom'
float       g_rcp_bloom_tex_h;                  // The reciprocal HEIGHT of the texture in 'bloom'

texture		g_SrcTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SrcTexture;
	minfilter = linear;
	magfilter = linear;
};

texture		g_LumTexture;

sampler LumSampler = sampler_state
{
	texture = g_LumTexture;
	minfilter = linear;
	magfilter = linear;
};

texture		g_BloomTexture;

sampler BloomSampler = sampler_state
{
	texture = g_BloomTexture;
	minfilter = linear;
	magfilter = linear;
};

texture		g_FinalTexture;

sampler FinalSampler = sampler_state
{
	texture = g_FinalTexture;
	minfilter = linear;
	magfilter = linear;
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

	vector vSrc = pow(tex2D(SrcSampler, In.vTexUV), 1.0 / 2.2); //tex2D(SrcSampler, In.vTexUV);
	vector vLum = tex2D(LumSampler, float2(0.5f, 0.5f));

	float xWeight = frac(In.vTexUV.x / g_rcp_bloom_tex_w) - 0.5;
	float xDir = xWeight;
	xWeight = abs(xWeight);
	xDir /= xWeight;
	xDir *= g_rcp_bloom_tex_w;

	float yWeight = frac(In.vTexUV.y / g_rcp_bloom_tex_h) - 0.5;
	float yDir = yWeight;
	yWeight = abs(yWeight);
	yDir /= yWeight;
	yDir *= g_rcp_bloom_tex_h;

	vector vBloom = ((1.f - xWeight) * (1.f - yWeight)) * tex2D(BloomSampler, In.vTexUV);
	vBloom += (xWeight * (1.f - yWeight)) * tex2D(BloomSampler, In.vTexUV + float2(xDir, 0.f));
	vBloom += (yWeight * (1.f - xWeight)) * tex2D(BloomSampler, In.vTexUV + float2(0.f, yDir));
	vBloom += (xWeight * yWeight) * tex2D(BloomSampler, In.vTexUV + float2(xDir, yDir));

	vector vFinal = vSrc + 0.25f * vBloom;

	float fLp = (g_fExposure / vLum.r) * max(vFinal.r, max(vFinal.g, vFinal.b));
	float fLmSqr = (vLum.g + g_fGaussianScalar * vLum.g) * (vLum.g + g_fGaussianScalar * vLum.g);

	float fToneScalar = (fLp * (1.f + (fLp / (fLmSqr)))) / (1.f + fLp);

	vector vColor = vFinal * fToneScalar;

	vColor.a = 1.0f;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_FINAL(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = tex2D(FinalSampler, In.vTexUV);

	Out.vColor = vColor;

	return Out;
}

technique Default_Technique
{
	pass HDR_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}	

	pass Final_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_FINAL();

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
	}
}