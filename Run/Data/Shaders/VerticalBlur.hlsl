struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer PostProcessConstants : register(b5)
{
    float c_width;
    float c_height;
    float pad0;
    float pad1;
};

Texture2D<float4> t_horizontalBlurTexture : register(t8);
SamplerState s_horizontalBlurSampler : register(s8);

VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;
    output.position = float4(input.a_position, 1.0f);
    output.uv = float2(input.a_uvTexCoords.x, 1.0f - input.a_uvTexCoords.y);
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

    float2 texelOffset = float2(0.0f, 1.0f / c_height);
    float3 blurredColor = t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv).rgb * weights[0];

    [unroll]
    for (int offset = 1; offset <= blurRadius; ++offset)
    {
        float2 sampleOffset = texelOffset * float(offset);

        blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv - sampleOffset).rgb * weights[offset];
        blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + sampleOffset).rgb * weights[offset];
    }

    return float4(blurredColor, 1.0f);
}