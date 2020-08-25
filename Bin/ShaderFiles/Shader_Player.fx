matrix g_matWVP, g_matWorld, g_matView, g_matProj;

float g_fTimeExp;

texture g_DiffuseTexture;

sampler	DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture g_NormalTexture;

sampler	NormalSampler = sampler_state
{
	texture = g_NormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct VS_IN
{
	float3	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float3	vTangent : TANGENT;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float4	vNormal : NORMAL;
	float3	vTangent : TANGENT;
	float3	vBinormal : BINORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	vector vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	float3 vBinormal = cross(In.vTangent, In.vNormal);

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_matWorld));
	Out.vBinormal = normalize(mul(vector(vBinormal, 0.f), g_matWorld));

	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float3	vTangent : TANGENT;
	float3	vBinormal : BINORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
	vector	vNormal : COLOR1;
	vector	vDepth : COLOR2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	/*vector vColor = vector(0.f, 0.f, 0.f, 0.f);

	for (int i = 0; i < 7; i++)
	{
		vColor += tex2D(DiffuseSampler, float2(In.vTexUV.x + g_GaussFilterY[i].x * texScalar + texOffset,
			In.vTexUV.y + g_GaussFilterY[i].y * texScalar + texOffset)) * g_GaussFilterY[i].w;
	}

	for (int i = 0; i < 7; i++)
	{
		vColor += tex2D(DiffuseSampler, float2(In.vTexUV.x + g_GaussFilterX[i].x * texScalar + texOffset,
			In.vTexUV.y + g_GaussFilterX[i].y * texScalar + texOffset)) * g_GaussFilterX[i].w;
	}

	Out.vColor = vColor;*/

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	vector vTangentNormal = tex2D(NormalSampler, In.vTexUV);
	vTangentNormal = normalize(vTangentNormal * 2 - 1);
	float3x3 TBN = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	TBN = transpose(TBN);

	float3 vNormal = mul(TBN, vTangentNormal.xyz);

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}