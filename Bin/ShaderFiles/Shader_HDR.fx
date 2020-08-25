static const int    MAX_SAMPLES = 16;    // 최대샘플링수
float2 g_SampleOffsets[MAX_SAMPLES];	// 샘플링위치
float4 g_SampleWeights[MAX_SAMPLES];

float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;

texture		g_SrcTexture;

sampler SrcSampler = sampler_state
{
	texture = g_SrcTexture;
	/*minfilter = linear;
	magfilter = linear;
	mipfilter = linear;*/
	minfilter = point;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture		g_ScaledTexture;

sampler ScaledSampler = sampler_state
{
	texture = g_ScaledTexture;
	minfilter = point;
	magfilter = point;
};

texture		g_BrightTexture;

sampler BrightSampler = sampler_state
{
	texture = g_BrightTexture;
	minfilter = point;
	AddressU = Clamp;
	AddressV = Clamp;
};

texture		g_BlurTexture;

sampler BlurSampler = sampler_state
{
	texture = g_BlurTexture;
	magfilter = linear;
	minfilter = linear;
};

texture		g_HDRTexture;

sampler HDRSampler = sampler_state
{
	texture = g_HDRTexture;
	magfilter = point;
	minfilter = point;
};


sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);
sampler s4 : register(s4);
sampler s5 : register(s5);
sampler s6 : register(s6);
sampler s7 : register(s7);

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

	vector vSource = pow(tex2D(SrcSampler, In.vTexUV), 1.0 / 2.2);
	vector vHdr = tex2D(HDRSampler, In.vTexUV);

	vector vColor = vSource + vHdr;

	//vColor = saturate((vColor * (A * vColor + B)) / (vColor * (C * vColor + D) + E));

	Out.vColor = vColor;// / (vColor + 1);

	return Out;
}

PS_OUT PS_SCALED(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vSample = 0.f;

	for (int i = 0; i < 16; i++) 
		vSample += tex2D(SrcSampler, In.vTexUV + g_SampleOffsets[i]);

	Out.vColor = vSample / 16;

	return Out;
}

PS_OUT PS_BRIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vSample = tex2D(ScaledSampler, In.vTexUV);

	vSample.rgb -= 1.f;

	vSample = 3.0f * max(vSample, 0.0f);

	Out.vColor = vSample;

	return Out;
}

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vSample = 0.f;

	for (int i = 0; i < 13; i++) 
		vSample += g_SampleWeights[i] * tex2D(BrightSampler, In.vTexUV + g_SampleOffsets[i]);
		
	Out.vColor = vSample;

	return Out;
}

PS_OUT PS_BLURAFTER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = 0.f;

	for (int i = 0; i < 8; i++)
		vColor += g_SampleWeights[i] * tex2D(BlurSampler, In.vTexUV + g_SampleOffsets[i]);

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MERGETEXTURE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = (tex2D(s0, In.vTexUV) + tex2D(s1, In.vTexUV) + tex2D(s2, In.vTexUV) + tex2D(s3, In.vTexUV) + tex2D(s4, In.vTexUV) + tex2D(s5, In.vTexUV)) / 6.0f;

	return Out;
}


technique Default_Technique
{
	pass Source_Rendering
	{
		/*AlphaTestEnable = true;
		AlphaFunc = Greater;
		AlphaRef = 0;

		ZWriteEnable = false;*/
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}		

	pass Scaled_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_SCALED();
	}

	pass Scaled_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BRIGHT();
	}

	pass Blur_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLUR();
	}

	pass BlurAfter_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLURAFTER();
	}

	pass MergeTexture_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLURAFTER();
		
		magfilter[0] = Point;
		minfilter[0] = Point;
		magfilter[1] = Point;
		minfilter[1] = Point;
		magfilter[2] = Point;
		minfilter[2] = Point;
		magfilter[3] = Point;
		minfilter[3] = Point;
		magfilter[4] = Point;
		minfilter[4] = Point;
		magfilter[5] = Point;
		minfilter[5] = Point;
	}
}