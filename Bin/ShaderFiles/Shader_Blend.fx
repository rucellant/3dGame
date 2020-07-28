

texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
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

	vector		vDiffuse = tex2D(DiffuseSampler, In.vTexUV);
	vector		vShade = tex2D(ShadeSampler, In.vTexUV);
	vector		vSpecular = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = vDiffuse;// *vShade + vSpecular;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		AlphaTestEnable = true;
		AlphaFunc = Greater;
		AlphaRef = 0;

		ZWriteEnable = false;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}		
}