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
    float2 v_uv : TEXCOORD0;
};

//------------------------------------------------------------------------------------------------
static const float PI = 3.14159265359f;
static const uint SAMPLE_COUNT = 1024u;

//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain(VertexInput input)
{
    VertexOutPixelIn output;
    output.v_position = float4(input.a_position, 1.0f);
    output.v_uv = float2(input.a_uvTexCoords.x, 1.0f - input.a_uvTexCoords.y);
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

    return normalize(tangent * halfwayVector.x + bitangent * halfwayVector.y + normal * halfwayVector.z);
}

//------------------------------------------------------------------------------------------------
float GeometrySchlickGGX(float normalDotDirection, float roughness)
{
    float alpha = roughness;
    float k = alpha * alpha * 0.5f;
    float denominator = normalDotDirection * (1.0f - k) + k;
    return normalDotDirection / max(denominator, 0.0001f);
}

//------------------------------------------------------------------------------------------------
float GeometrySmith(float3 normal, float3 viewDirection, float3 lightDirection, float roughness)
{
    float normalDotView = saturate(dot(normal, viewDirection));
    float normalDotLight = saturate(dot(normal, lightDirection));
    float geometryView = GeometrySchlickGGX(normalDotView, roughness);
    float geometryLight = GeometrySchlickGGX(normalDotLight, roughness);
    return geometryView * geometryLight;
}

//------------------------------------------------------------------------------------------------
float2 IntegrateBRDF(float normalDotView, float roughness)
{
    float3 viewDirection;
    viewDirection.x = sqrt(max(1.0f - normalDotView * normalDotView, 0.0f));
    viewDirection.y = 0.0f;
    viewDirection.z = normalDotView;

    float3 normal = float3(0.0f, 0.0f, 1.0f);

    float coefficientA = 0.0f;
    float coefficientB = 0.0f;

    for (uint sampleIndex = 0u; sampleIndex < SAMPLE_COUNT; ++sampleIndex)
    {
        float2 randomSample = Hammersley(sampleIndex, SAMPLE_COUNT);
        float3 halfwayVector = ImportanceSampleGGX(randomSample, normal, roughness);
        float3 lightDirection = normalize(2.0f * dot(viewDirection, halfwayVector) * halfwayVector - viewDirection);

        float normalDotLight = saturate(lightDirection.z);
        float normalDotHalfway = saturate(halfwayVector.z);
        float viewDotHalfway = saturate(dot(viewDirection, halfwayVector));

        if (normalDotLight > 0.0f)
        {
            float geometry = GeometrySmith(normal, viewDirection, lightDirection, roughness);
            float geometryVisibility = geometry * viewDotHalfway / max(normalDotHalfway * normalDotView, 0.0001f);
            float fresnelCoefficient = pow(1.0f - viewDotHalfway, 5.0f);

            coefficientA += (1.0f - fresnelCoefficient) * geometryVisibility;
            coefficientB += fresnelCoefficient * geometryVisibility;
        }
    }

    return float2(coefficientA, coefficientB) / float(SAMPLE_COUNT);
}

//------------------------------------------------------------------------------------------------
float2 PixelMain(VertexOutPixelIn input) : SV_Target0
{
    float normalDotView = saturate(input.v_uv.x);
    float roughness = saturate(input.v_uv.y);
    return IntegrateBRDF(normalDotView, roughness);
}