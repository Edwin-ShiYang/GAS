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

Texture2D<float4> t_verticalBlurTexture : register(t9);
SamplerState s_verticalBlurSampler : register(s9);

    //------------------------------------------------------------------------------------------
float3 ApplyACESToneMapping(float3 hdrColor)
{
    float curveA = 2.51f;
    float curveB = 0.03f;
    float curveC = 2.43f;
    float curveD = 0.59f;
    float curveE = 0.14f;

    float3 toneMappedColor = hdrColor * (curveA * hdrColor + curveB) / (hdrColor * (curveC * hdrColor + curveD) + curveE);
    return saturate(toneMappedColor);
}

    //------------------------------------------------------------------------------------------
VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;

    output.position = float4(input.a_position, 1.0f);

    output.uv.x = input.a_uvTexCoords.x;
    output.uv.y = 1.0f - input.a_uvTexCoords.y;

    return output;
}

    //------------------------------------------------------------------------------------------
float4 PixelMain(VertexToPixel input) : SV_Target0
{
    float3 hdrColor =
            t_hdrTexture.Sample(s_hdrSampler, input.uv).rgb;

    float3 bloomColor =
            t_verticalBlurTexture.Sample(s_verticalBlurSampler, input.uv).rgb;

    float exposure = 0.8f;
    float bloomIntensity = 0.35f;

    float3 combinedHDRColor =
            hdrColor * exposure +
            bloomColor * bloomIntensity;

    float3 toneMappedColor =
            ApplyACESToneMapping(combinedHDRColor);

    float3 displayColor =
            pow(saturate(toneMappedColor), 1.0f / 2.2f);

    return float4(displayColor, 1.0f);
}