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

Texture2D<float4> t_hdrTexture : register(t6);
SamplerState s_hdrSampler : register(s6);

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
    float3 hdrColor = t_hdrTexture.Sample(s_hdrSampler, input.uv).rgb;
    float brightness = dot(hdrColor, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 1.5f)
    {
        return float4(hdrColor, 1.0f);
    }
    return float4(0, 0, 0, 1);
}