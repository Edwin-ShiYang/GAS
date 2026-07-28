//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
    float3 a_tangent : VERTEX_TANGENT;
    float3 a_bitangent : VERTEX_BITANGENT;
    float3 a_normal : VERTEX_NORMAL;
    uint a_vertexID : SV_VertexID;
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
TextureCube<float4> t_environmentCubemap : register(t11);
SamplerState s_environmentSampler : register(s11);

//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain(VertexInput input)
{
    VertexOutPixelIn output;

    float4 modelPosition = float4(input.a_position, 1.0f);
    float4 worldPosition = mul(c_modelToWorld, modelPosition);
    float4 cameraPosition = mul(c_worldToCamera, worldPosition);
    float4 renderPosition = mul(c_cameraToRender, cameraPosition);
    float4 clipPosition = mul(c_renderToClip, renderPosition);

    clipPosition.x = -clipPosition.x;

    output.v_position = clipPosition;
    output.v_direction = worldPosition.xyz - c_cameraWorldPos;

    return output;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(VertexOutPixelIn input) : SV_Target0
{
    static const float PI = 3.14159265359f;
    static const float SAMPLE_DELTA = 0.05f;

    float3 normal = normalize(input.v_direction);
    float3 referenceUp = abs(normal.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
    float3 tangent = normalize(cross(referenceUp, normal));
    float3 bitangent = normalize(cross(normal, tangent));

    float3 irradiance = float3(0.0f, 0.0f, 0.0f);
    float sampleCount = 0.0f;

    for (float phi = 0.0f; phi < 2.0f * PI; phi += SAMPLE_DELTA)
    {
        for (float theta = 0.0f; theta < 0.5f * PI; theta += SAMPLE_DELTA)
        {
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);
            float3 tangentSample = float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
            float3 sampleDirection = tangentSample.x * tangent + tangentSample.y * bitangent + tangentSample.z * normal;
            irradiance += t_environmentCubemap.Sample(s_environmentSampler, sampleDirection).rgb * cosTheta * sinTheta;
            sampleCount += 1.0f;
        }
    }

    irradiance = PI * irradiance / max(sampleCount, 1.0f);
    return float4(irradiance, 1.0f);
}