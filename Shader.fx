

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
	float2 Coord : COORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 Coord : COORD;
};

Texture2D diffuseTex : register(t0);
SamplerState samp : register(s0);


VertexOut VS(VertexIn vin)
{
	VertexOut vout;



	float4x4 modelView = mul(viewMatrix, worldMatrix); 
	float4x4 modelViewProjection = mul(projectionMatrix, modelView);
	float4 pos = mul(modelViewProjection, float4(vin.PosL, 1.0f));

	vout.PosH = pos;
	vout.Coord = vin.Coord;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return diffuseTex.Sample(samp, pin.Coord);
	//return pin.Coord;
}
