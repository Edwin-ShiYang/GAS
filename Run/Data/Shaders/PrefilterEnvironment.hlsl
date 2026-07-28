//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
    float3 a_tangent : VERTEX_TANGENT;
    float3 a_bitangent : VERTEX_BITANGENT;
    float3 a_normal : VERTEX_NORMAL;
};

//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn
{
    float4 v_position : SV_Position;
    float3 v_direction : TEXCOORD0;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 c_renderToClip;
    float4x4 c_cameraToRender;
    float4x4 c_worldToCamera;
    float3 c_cameraWorldPos;
    float EMPTY_PADDING0;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
    float4x4 c_modelToWorld;
    float4 c_modelTint;
};

//------------------------------------------------------------------------------------------------
cbuffer PrefilterConstants : register(b6)
{
    float c_roughness;
    float3 PREFILTER_PADDING;
};

//------------------------------------------------------------------------------------------------
TextureCube<float4> t_environmentCubemap : register(t11);
SamplerState s_environmentSampler : register(s11);

//------------------------------------------------------------------------------------------------
static const float PI = 3.14159265359f;
static const uint SAMPLE_COUNT = 1024u;

//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain(VertexInput input)
{
    VertexOutPixelIn output;

    float4 modelPosition = float4(input.a_position, 1.0f);
    float4 worldPosition = mul(c_modelToWorld, modelPosition);
    float4 cameraPosition = mul(c_worldToCamera, worldPosition);
    float4 renderPosition = mul(c_cameraToRender, cameraPosition);
    
    output.v_position = mul(c_renderToClip, renderPosition);
    output.v_position.x = -output.v_position.x;
    output.v_direction = worldPosition.xyz - c_cameraWorldPos;

    return output;
}

//------------------------------------------------------------------------------------------------
float RadicalInverseVanDerCorput(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10f;
}

//------------------------------------------------------------------------------------------------
float2 Hammersley(uint sampleIndex, uint sampleCount)
{
    return float2(float(sampleIndex) / float(sampleCount), RadicalInverseVanDerCorput(sampleIndex));
}

//------------------------------------------------------------------------------------------------
float3 ImportanceSampleGGX(float2 randomSample, float3 normal, float roughness)
{
    float alpha = roughness * roughness;

    float phi = 2.0f * PI * randomSample.x;
    float cosTheta = sqrt((1.0f - randomSample.y) / (1.0f + (alpha * alpha - 1.0f) * randomSample.y));
    float sinTheta = sqrt(max(1.0f - cosTheta * cosTheta, 0.0f));

    float3 halfwayVector;
    halfwayVector.x = cos(phi) * sinTheta;
    halfwayVector.y = sin(phi) * sinTheta;
    halfwayVector.z = cosTheta;

    float3 upDirection = abs(normal.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
    float3 tangent = normalize(cross(upDirection, normal));
    float3 bitangent = cross(normal, tangent);

    float3 sampleDirection = tangent * halfwayVector.x + bitangent * halfwayVector.y + normal * halfwayVector.z;
    return normalize(sampleDirection);
}

//------------------------------------------------------------------------------------------------
float DistributionGGX(float3 normal, float3 halfwayVector, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSquared = alpha * alpha;
    float normalDotHalfway = saturate(dot(normal, halfwayVector));
    float normalDotHalfwaySquared = normalDotHalfway * normalDotHalfway;

    float denominator = normalDotHalfwaySquared * (alphaSquared - 1.0f) + 1.0f;
    denominator = PI * denominator * denominator;

    return alphaSquared / max(denominator, 0.000001f);
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(VertexOutPixelIn input) : SV_Target0
{
    float3 normal = normalize(input.v_direction);
    float3 reflectionDirection = normal;
    float3 viewDirection = reflectionDirection;

    float3 prefilteredColor = float3(0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    uint environmentWidth;
    uint environmentHeight;
    uint environmentMipCount;
    t_environmentCubemap.GetDimensions(0, environmentWidth, environmentHeight, environmentMipCount);

    float environmentResolution = float(environmentWidth);
    float texelSolidAngle = 4.0f * PI / (6.0f * environmentResolution * environmentResolution);

    for (uint sampleIndex = 0u; sampleIndex < SAMPLE_COUNT; ++sampleIndex)
    {
        float2 randomSample = Hammersley(sampleIndex, SAMPLE_COUNT);
        float3 halfwayVector = ImportanceSampleGGX(randomSample, normal, c_roughness);
        float3 lightDirection = normalize(2.0f * dot(viewDirection, halfwayVector) * halfwayVector - viewDirection);

        float normalDotLight = saturate(dot(normal, lightDirection));

        if (normalDotLight > 0.0f)
        {
            float normalDotHalfway = saturate(dot(normal, halfwayVector));
            float halfwayDotView = saturate(dot(halfwayVector, viewDirection));
            float distribution = DistributionGGX(normal, halfwayVector, c_roughness);
            float probabilityDensity = distribution * normalDotHalfway / max(4.0f * halfwayDotView, 0.0001f);
            float sampleSolidAngle = 1.0f / (float(SAMPLE_COUNT) * probabilityDensity + 0.0001f);
            float sourceMipLevel = c_roughness <= 0.0f ? 0.0f : 0.5f * log2(sampleSolidAngle / texelSolidAngle);

            prefilteredColor += t_environmentCubemap.SampleLevel(s_environmentSampler, lightDirection, sourceMipLevel).rgb * normalDotLight;
            totalWeight += normalDotLight;
        }
    }

    prefilteredColor /= max(totalWeight, 0.0001f);
    return float4(prefilteredColor, 1.0f);
}