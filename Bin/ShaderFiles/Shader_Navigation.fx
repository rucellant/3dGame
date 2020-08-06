matrix g_matWVP, g_matWorld, g_matView, g_matProj;

bool g_bIsSelect;

struct VS_IN
{
	float3	vPosition : POSITION;
};

struct VS_OUT
{
	float4	vPosition : POSITION;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	// 월드변환 
	vector vPosition;

	vPosition = mul(vector(In.vPosition, 1.f), g_matWVP);

	Out.vPosition = vPosition;

	return Out;	
}


struct PS_IN
{
	float4	vPosition : POSITION;
};

struct PS_OUT
{
	vector  vColor : COLOR0;	
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (g_bIsSelect)
		Out.vColor = vector(0.f, 1.f, 0.f, 1.f); // rgba
	else
		Out.vColor = vector(1.f, 0.f, 0.f, 1.f); // rgba

	return Out;	
}

technique Default_Technique
{
	pass Default_Rendering
	{
		fillmode = wireframe;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}













