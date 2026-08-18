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

cbuffer ToneMappingConstants : register(b10)
{
    float c_exposure;
    float c_bloomIntensity;
    float padding0;
    float padding1;
};

Texture2D<float4> t_hdrTexture : register(t6);
SamplerState s_hdrSampler : register(s6);

Texture2D<float4> t_verticalBlurTexture : register(t9);
SamplerState s_verticalBlurSampler : register(s9);

//-----------------------------------------------------------------------------------------------
float3 ApplyACESToneMapping(float3 hdrColor)
{
    static const float curveA = 2.51f;
    static const float curveB = 0.03f;
    static const float curveC = 2.43f;
    static const float curveD = 0.59f;
    static const float curveE = 0.14f;

    float3 numerator = hdrColor * (curveA * hdrColor + curveB);
    float3 denominator = hdrColor * (curveC * hdrColor + curveD) + curveE;

    return saturate(numerator / denominator);
}

//-----------------------------------------------------------------------------------------------
VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;
    output.position = float4(input.a_position, 1.0f);
    output.uv = float2(input.a_uvTexCoords.x, 1.0f - input.a_uvTexCoords.y);
    return output;
}

//-----------------------------------------------------------------------------------------------
float4 PixelMain(VertexToPixel input) : SV_Target0
{
    static const float inverseGamma = 1.0f / 2.2f;

    float3 hdrColor = t_hdrTexture.Sample(s_hdrSampler, input.uv).rgb;
    float3 bloomColor = t_verticalBlurTexture.Sample(s_verticalBlurSampler, input.uv).rgb;

    float3 combinedColor = hdrColor + bloomColor * c_bloomIntensity;
    float3 exposedColor = combinedColor * c_exposure;
    float3 toneMappedColor = ApplyACESToneMapping(exposedColor);
    float3 displayColor = pow(toneMappedColor, inverseGamma);

    return float4(displayColor, 1.0f);
}