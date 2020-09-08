matrix g_matWVP, g_matWorld, g_matView, g_matProj;

float g_fTimeAcc, g_fTimeFade, g_fAngle; //라디안임

bool g_bPortalFade, g_bIsFade;

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

PS_MESH_OUT PS_TORNADO(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.rgb *= 3.f;

	if (g_bIsFade == false)
	{
		if (g_fTimeAcc <= In.vTexUV.x)
			Out.vColor.a = 0.f;
	}
	else
	{
		if (g_fTimeAcc >= In.vTexUV.x)
			Out.vColor.a = 0.f;
	}

	/*if (g_fTimeAcc <= In.vTexUV.x)
		Out.vColor.a = 0.f;*/
	

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_HIT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(SrcSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_SHOULDER(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vUV = (In.vTexUV * 2.f - 1.f);

	float2 vWaveUV = float2(vUV.x * cos(g_fAngle) - vUV.y * sin(g_fAngle), vUV.x * sin(g_fAngle) + vUV.y * cos(g_fAngle));
	float2 vReverseWaveUV = float2(vUV.x * cos(-g_fAngle) - vUV.y * sin(-g_fAngle), vUV.x * sin(-g_fAngle) + vUV.y * cos(-g_fAngle));

	vWaveUV = (vWaveUV + 1.f) / 2.f;
	vReverseWaveUV = (vReverseWaveUV + 1.f) / 2.f;

	vector vWave = tex2D(DstSampler, vWaveUV);
	vector vWaveReverse = tex2D(DstSampler, vReverseWaveUV);
	vector vRing = tex2D(SrcSampler, In.vTexUV);
	vector vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	vector vColor = (vRing + (vWaveReverse * vWave)) * vDiffuse;

	Out.vColor = vColor;

	return Out;
}

PS_MESH_OUT PS_EARTHQUAKE_CIRCLE(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	Out.vColor = tex2D(SrcSampler, In.vTexUV);

	if (g_fTimeAcc <= In.vTexUV.x)
		Out.vColor.a = 0.f;
	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_MESH_OUT PS_EARTHQUAKE_CYLINDER(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	float2 vShadeSrc = In.vTexUV + float2(0.f, -g_fTimeAcc * 2.f);
	float2 vShadeDst = In.vTexUV;

	vector vSrc = tex2D(SrcSampler, vShadeSrc);
	vector vDst = tex2D(DstSampler, vShadeDst);

	Out.vColor = vSrc * vDst;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_MESH_OUT PS_EARTHQUAKE_RING(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	float2 vShadeUV = In.vTexUV + float2(g_fTimeAcc * 10.f, 0.f);
	float2 vShadeReverseUV = In.vTexUV - float2(g_fTimeAcc * 10.f, 0.f);
	
	vector vColor = tex2D(SrcSampler, vShadeUV);
	vector vColorReverse = tex2D(SrcSampler, vShadeReverseUV);

	Out.vColor = vColor + vColorReverse;

	vector vDst = tex2D(DstSampler, In.vTexUV);

	Out.vColor *= vDst;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_BUFF_CIRCLE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vUV = (In.vTexUV * 2.f - 1.f);

	float2 vWaveUV = float2(vUV.x * cos(g_fAngle) - vUV.y * sin(g_fAngle), vUV.x * sin(g_fAngle) + vUV.y * cos(g_fAngle));
	float2 vReverseWaveUV = float2(vUV.x * cos(-g_fAngle) - vUV.y * sin(-g_fAngle), vUV.x * sin(-g_fAngle) + vUV.y * cos(-g_fAngle));

	vWaveUV = (vWaveUV + 1.f) / 2.f;
	vReverseWaveUV = (vReverseWaveUV + 1.f) / 2.f;

	vector vWave = tex2D(SrcSampler, vWaveUV);
	vector vWaveReverse = tex2D(SrcSampler, vReverseWaveUV);

	vector vLength = vector(vWaveUV.x - 0.5f, vWaveUV.y - 0.5f, 0.f, 0.f);

	float fLength = length(vLength);

	if (fLength >= 0.5f)
		vWave.a = 0.f;

	vLength = vector(vReverseWaveUV.x - 0.5f, vReverseWaveUV.y - 0.5f, 0.f, 0.f);

	fLength = length(vLength);

	if (fLength >= 0.5f)
		vWaveReverse.a = 0.f;

	vector vColor = vWaveReverse + vWave;

	float2 vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthInfo = tex2D(DepthSampler, vTexUV);

	float fOldViewZ = vDepthInfo.y * 500.0f;
	float fViewZ = In.vProjPos.w;

	vColor.a = vColor.a * saturate(fOldViewZ - fViewZ) * 2.f;

	Out.vColor = vColor;

	return Out;
}

PS_MESH_OUT PS_BUFF_CYLINDER(PS_MESH_IN In)
{
	PS_MESH_OUT Out = (PS_MESH_OUT)0;

	In.vTexUV.y -= g_fTimeAcc;

	Out.vColor = tex2D(SrcSampler, In.vTexUV);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.0f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_TORCH(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(SrcSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_BREATH(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vColor = tex2D(SrcSampler, In.vTexUV);

	float2 vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector vDepthInfo = tex2D(DepthSampler, vTexUV);

	float fOldViewZ = vDepthInfo.y * 500.0f;
	float fViewZ = In.vProjPos.w;

	vColor.a = vColor.a * saturate(fOldViewZ - fViewZ) * 2.f;

	Out.vColor = vColor;

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

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_TORNADO();
	}

	pass Hit_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = one;//one;//SrcAlpha;
		DestBlend = one;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HIT();
	}

	pass Shoulder_Rendering
	{
		cullmode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		zwriteenable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SHOULDER();
	}

	pass Earthquake_Circle_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_EARTHQUAKE_CIRCLE();
	}

	pass Earthquake_Cylinder_Rendering
	{
		//cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_EARTHQUAKE_CYLINDER();
	}

	pass Earthquake_Ring_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_EARTHQUAKE_RING();
	}

	pass Buff_Circle_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BUFF_CIRCLE();
	}

	pass Buff_Cylinder_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;//one;//SrcAlpha;
		DestBlend = InvSrcAlpha;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MESH_MAIN();
		PixelShader = compile ps_3_0 PS_BUFF_CYLINDER();
	}

	pass Torch_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;//one;//SrcAlpha;
		DestBlend = one;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_TORCH();
	}

	pass Breath_Rendering
	{
		cullmode = none;

		zwriteenable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;//one;//SrcAlpha;
		DestBlend = one;//InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BREATH();
	}
}