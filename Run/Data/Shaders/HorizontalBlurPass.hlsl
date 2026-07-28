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

Texture2D<float4> t_brightPassTexture : register(t7);
SamplerState s_brightPassSampler : register(s7);

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
    float texelWidth = 1.0f / c_width;
    float3 blurredColor = float3(0.0f, 0.0f, 0.0f);

    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-8.0f * texelWidth, 0.0f)).rgb * 0.020f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-7.0f * texelWidth, 0.0f)).rgb * 0.025f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-6.0f * texelWidth, 0.0f)).rgb * 0.035f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-5.0f * texelWidth, 0.0f)).rgb * 0.045f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-4.0f * texelWidth, 0.0f)).rgb * 0.060f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-3.0f * texelWidth, 0.0f)).rgb * 0.075f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-2.0f * texelWidth, 0.0f)).rgb * 0.090f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(-1.0f * texelWidth, 0.0f)).rgb * 0.105f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv).rgb * 0.110f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(1.0f * texelWidth, 0.0f)).rgb * 0.105f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(2.0f * texelWidth, 0.0f)).rgb * 0.090f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(3.0f * texelWidth, 0.0f)).rgb * 0.075f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(4.0f * texelWidth, 0.0f)).rgb * 0.060f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(5.0f * texelWidth, 0.0f)).rgb * 0.045f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(6.0f * texelWidth, 0.0f)).rgb * 0.035f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(7.0f * texelWidth, 0.0f)).rgb * 0.025f;
    blurredColor += t_brightPassTexture.Sample(s_brightPassSampler, input.uv + float2(8.0f * texelWidth, 0.0f)).rgb * 0.020f;

    return float4(blurredColor, 1.0f);
}