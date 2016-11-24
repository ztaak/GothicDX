

cbuffer perApplication : register(b0)
{
	matrix projectionMatrix;
}

cbuffer perFrame : register(b1)
{
	matrix viewMatrix;
}

cbuffer perObject : register(b2)
{
	matrix worldMatrix;
}

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;



	float4x4 modelView = mul(viewMatrix, worldMatrix); 
	float4x4 modelViewProjection = mul(projectionMatrix, modelView);
	float4 pos = mul(modelViewProjection, float4(vin.PosL, 1.0f));

	vout.PosH = pos;
	vout.Color = float4(vin.Color, 1.0f);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
