vector      g_vTCLumOffsets[4]; 
vector      g_vTCDSOffsets[9];

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

PS_OUT PS_GRAYSCALEDOWNSAMPLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float fAverage = 0.0f;
	float fMaximum = -1e20;
	vector vColor = 0.0f;
	float3 vWeight = float3(0.299f, 0.587f, 0.114f);

	for (int i = 0; i < 4; i++)
	{
		vColor = tex2D(SrcSampler, In.vTexUV + float2(g_vTCLumOffsets[i].x, g_vTCLumOffsets[i].y));

		float fGreyValue = max(vColor.r, max(vColor.g, vColor.b));

		fMaximum = max(fMaximum, fGreyValue);
		fAverage += (0.25f * log(1e-5 + fGreyValue));
	}
		
	fAverage = exp(fAverage);

	Out.vColor = vector(fAverage, fMaximum, 0.f, 1.f);

	return Out;
}

PS_OUT PS_DOWNSAMPLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float fAverage = 0.0f;
	float fMaximum = -1e20;
	vector vColor = 0.0f;

	float3 vWeight = float3(0.299f, 0.587f, 0.114f);

	for (int i = 0; i < 9; i++)
	{
		vColor = tex2D(SrcSampler, In.vTexUV + float2(g_vTCDSOffsets[i].x, g_vTCDSOffsets[i].y));
		fAverage += vColor.r;
		fMaximum = max(fMaximum, vColor.g);
	}

	fAverage /= 9.f;

	Out.vColor = vector(fAverage, fMaximum, 0.f, 1.f);

	return Out;
}


technique Default_Technique
{
	pass Source_To_Luminance_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_GRAYSCALEDOWNSAMPLE();

		zwriteenable = false;
	}

	pass Down_Sample_Rendering
	{
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DOWNSAMPLE();

		zwriteenable = false;
	}
}