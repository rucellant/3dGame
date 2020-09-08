matrix g_matWVP, g_matWorld, g_matView, g_matProj, g_matLightView, g_matLightProj;

float g_fTimeExp;

vector g_vCamPosition;

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

texture		g_ShadowTexture;

sampler	ShadowSampler = sampler_state
{
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	texture = g_ShadowTexture;
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
	float4	vWorldPos : TEXCOORD2;
	float4	vShadowPosition : TEXCOORD3;
};

struct VS_SHADOWOUT
{
	float4	vPosition : POSITION;
	float4	vShadowPosition : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	vector vPosition, vShadowPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	vShadowPosition = mul(vector(In.vPosition, 1.f), g_matWorld);
	vShadowPosition = mul(vShadowPosition, g_matLightView);
	vShadowPosition = mul(vShadowPosition, g_matLightProj);

	vector vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vWorldPos = vWorldPos;

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vShadowPosition = vShadowPosition;

	float3 vBinormal = cross(In.vTangent, In.vNormal);

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_matWorld));
	Out.vBinormal = normalize(mul(vector(vBinormal, 0.f), g_matWorld));

	Out.vProjPos = vPosition;

	return Out;
}

VS_SHADOWOUT VS_SHADOW(VS_IN In)
{
	VS_SHADOWOUT		Out = (VS_SHADOWOUT)0;

	vector		vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWorld);

	vPosition = mul(vPosition, g_matView);
	vPosition = mul(vPosition, g_matProj);

	//vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;
	Out.vShadowPosition = vPosition;

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
	float4	vWorldPos : TEXCOORD2;
	float4	vShadowPosition : TEXCOORD3;
};

struct PS_SHADOWIN
{
	float4	vPosition : POSITION;
	float4	vShadowPosition : TEXCOORD1;
};

struct PS_OUT
{
	vector  vColor : COLOR0;
	vector	vNormal : COLOR1;
	vector	vDepth : COLOR2;
};

struct PS_SHADOWOUT
{
	vector  vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	//Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	vector vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	/*float CurDepth = In.vShadowPosition.z / In.vShadowPosition.w;
	float2 uv = In.vShadowPosition.xy / In.vShadowPosition.w;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;
	float ShadowDepth = tex2D(ShadowSampler, uv).r;
	 0.0000025f
	if (CurDepth > ShadowDepth + 0.0000004f)
		vDiffuse *= 0.5f;*/

	vector vTangentNormal = tex2D(NormalSampler, In.vTexUV);
	vTangentNormal = normalize(vTangentNormal * 2 - 1);
	float3x3 TBN = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	TBN = transpose(TBN);

	float3 vNormal = mul(TBN, vTangentNormal.xyz);

	vector vLook = normalize(g_vCamPosition - In.vWorldPos);

	float Rim = 1 - dot(vLook, vNormal);

	Rim *= Rim;

	vDiffuse.rgb += Rim;

	float CurDepth = In.vShadowPosition.z / In.vShadowPosition.w;
	float2 uv = In.vShadowPosition.xy / In.vShadowPosition.w;
	uv.y = -uv.y;
	uv = uv * 0.5 + 0.5;
	float ShadowDepth = tex2D(ShadowSampler, uv).r;
	// 0.0000025f
	if (CurDepth > ShadowDepth + 0.0000004f)
		vDiffuse *= 0.5f;

	Out.vColor = vDiffuse;

	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_SHADOWOUT PS_SHADOW(PS_SHADOWIN In)
{
	PS_SHADOWOUT		Out = (PS_SHADOWOUT)0;

	float Shadow = In.vShadowPosition.z / In.vShadowPosition.w;

	//Out.vColor = vector(Shadow.xxx, 1.f);

	Out.vColor = vector(Shadow, In.vShadowPosition.w / 500.f, 0.f, 1.f);

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Shadow_Rendering
	{
		VertexShader = compile vs_3_0 VS_SHADOW();
		PixelShader = compile ps_3_0 PS_SHADOW();
	}
}