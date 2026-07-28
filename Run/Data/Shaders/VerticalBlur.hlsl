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

    output.uv.x = input.a_uvTexCoords.x;
    output.uv.y = 1.0f - input.a_uvTexCoords.y;

    return output;
}

float4 PixelMain(VertexToPixel input) : SV_Target0
{
    float texelHeight = 1.0f / c_height;
    float3 blurredColor = float3(0.0f, 0.0f, 0.0f);

    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -8.0f * texelHeight)).rgb * 0.020f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -7.0f * texelHeight)).rgb * 0.025f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -6.0f * texelHeight)).rgb * 0.035f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -5.0f * texelHeight)).rgb * 0.045f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -4.0f * texelHeight)).rgb * 0.060f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -3.0f * texelHeight)).rgb * 0.075f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -2.0f * texelHeight)).rgb * 0.090f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, -1.0f * texelHeight)).rgb * 0.105f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv).rgb * 0.110f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 1.0f * texelHeight)).rgb * 0.105f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 2.0f * texelHeight)).rgb * 0.090f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 3.0f * texelHeight)).rgb * 0.075f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 4.0f * texelHeight)).rgb * 0.060f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 5.0f * texelHeight)).rgb * 0.045f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 6.0f * texelHeight)).rgb * 0.035f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 7.0f * texelHeight)).rgb * 0.025f;
    blurredColor += t_horizontalBlurTexture.Sample(s_horizontalBlurSampler, input.uv + float2(0.0f, 8.0f * texelHeight)).rgb * 0.020f;

    return float4(blurredColor, 1.0f);
}