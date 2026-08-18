struct VertexInput
{
    float3 position : VERTEX_POSITION;
    float2 uv : VERTEX_UVTEXCOORDS;
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer BloomConstants : register(b8)
{
    float c_bloomThreshold;
    float3 padding;
};

Texture2D<float4> t_hdrTexture : register(t6);
SamplerState      s_hdrSampler : register(s6);

VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;
    output.position = float4( input.position, 1.0f );
    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
    return output;
}

float4 PixelMain(VertexToPixel input) : SV_Target0
{
    static const float3 luminanceWeights = float3( 0.2126f, 0.7152f, 0.0722f );

    float3 hdrColor  = t_hdrTexture.Sample( s_hdrSampler, input.uv ).rgb;
    float brightness = dot( hdrColor, luminanceWeights );

    if ( brightness <= c_bloomThreshold )
    {
        hdrColor = float3( 0.0f, 0.0f, 0.0f );
    }

    return float4( hdrColor, 1.0f );
}