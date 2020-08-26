matrix g_matWVP, g_matWorld, g_matView, g_matProj;

float g_fTimeAcc, g_fTimeFade;

bool g_bPortalFade;

texture g_SrcTexture;

sampler	SrcSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_SrcTexture;
};

texture g_DstTexture;

sampler	DstSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_DstTexture;
};

texture g_DepthTexture;

sampler	DepthSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_DepthTexture;
};

texture g_DiffuseTexture;

sampler	DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


struct VS_IN
{
	float3	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
};

struct VS_MESH_IN
{
	float3	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float2	vTexUV : TEXCOORD0;
	float3	vTangent : TANGENT;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

struct VS_MESH_OUT
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
	Out.vProjPos = vPosition;

	return Out;
}

VS_MESH_OUT VS_MESH_MAIN(VS_MESH_IN In)
{
	VS_MESH_OUT Out = (VS_MESH_OUT)0;

	vector vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	float3 vBinormal = cross(In.vTangent, In.vNormal);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_matWorld));
	Out.vBinormal = normalize(mul(vector(vBinormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vProjPos = vPosition;

	return Out;
}

struct PS_IN
{
	float4	vPosition : POSITION;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
};

struct PS_MESH_IN
{
	float4	vPosition : POSITION;
	float3	vNormal : NORMAL;
	float3	vTangent : TANGENT;
	float3	vBinormal : BINORMAL;
	float2	vTexUV : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
};

struct PS_MESH_OUT
{
	vector  vColor : COLOR0;
	vector	vNormal : COLOR1;
	vector	vDepth : COLOR2;
};

PS_OUT PS_PORTAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vSrc = tex2D(SrcSampler, In.vTexUV);

	vSrc *= 0.5f;

	float2 vShadeTexUV = In.vTexUV;

	vShadeTexUV.y = vShadeTexUV.y - g_fTimeAcc * 0.2f;

	vector vDst = tex2D(DstSampler, vShadeTexUV);
	
	Out.vColor = vSrc * vDst;

	float2 vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthInfo = tex2D(DepthSampler, vTexUV);

	float fOldViewZ = vDepthInfo.y * 500.0f;
	float fViewZ = In.vProjPos.w;

	Out.vColor.a = Out.vColor.a * saturate(fOldViewZ - fViewZ) * 2.f;

	if (g_bPortalFade == true)
		Out.vColor.a = Out.vColor.a * g_fTimeFade;
	

	return Out;
}

PS_MESH_OUT PS_MESH_MAIN(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	/*vector vTangentNormal = tex2D(NormalSampler, In.vTexUV);
	vTangentNormal = normalize(vTangentNormal * 2 - 1);
	float3x3 TBN = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	TBN = transpose(TBN);

	float3 vNormal = mul(TBN, vTangentNormal.xyz);*/

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

technique Default_Technique
{
	pass Portal_Rendering
	{
		cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = one;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_PORTAL();
	}

	pass Mesh_Rendering
	{
		cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_MESH_MAIN();
	}
}