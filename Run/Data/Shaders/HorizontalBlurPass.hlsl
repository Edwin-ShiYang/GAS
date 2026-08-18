struct VertexInput
{
    float3 position : VERTEX_POSITION;
    float4 color : VERTEX_COLOR;
    float2 uv : VERTEX_UVTEXCOORDS;
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer PostProcessConstants : register(b5)
{
    float  c_width;
    float  c_height;
    float2 padding;
};

Texture2D<float4> t_brightPassTexture : register(t7);
SamplerState s_brightPassSampler : register(s7);

VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;
    output.position = float4(input.position, 1.0f);
    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
    return output;
}

float4 PixelMain(VertexToPixel input) : SV_Target0
{
    static const int blurRadius = 4;
    static const float weights[blurRadius + 1] =
    {
        0.227027f,
        0.194595f,
        0.121622f,
        0.054054f,
        0.016216f
    };

    float2 texelOffset = float2( 1.0f / c_width, 0.0f );
    float3 blurredColor = t_brightPassTexture.Sample(s_brightPassSampler, input.uv).rgb * weights[0];

    [unroll]
    for (int offset = 1; offset <= blurRadius; ++offset)
    {
        float2 sampleOffset = texelOffset * float(offset);

        blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv - sampleOffset).rgb * weights[offset];
        blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + sampleOffset).rgb * weights[offset];
    }

    return float4(blurredColor, 1.0f);
}