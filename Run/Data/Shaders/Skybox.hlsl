struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
    float3 a_tangent : VERTEX_TANGENT;
    float3 a_bitangent : VERTEX_BITANGENT;
    float3 a_normal : VERTEX_NORMAL;
};

struct VertexOutPixelIn
{
    float4 v_position : SV_Position;
    float3 v_direction : TEXCOORD0;
};

cbuffer CameraConstants : register(b2)
{
    float4x4 c_renderToClip;
    float4x4 c_cameraToRender;
    float4x4 c_worldToCamera;
    float3 c_cameraWorldPos;
    float EMPTY_PADDING0;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 c_modelToWorld;
    float4 c_modelTint;
};

TextureCube<float4> t_environmentCubemap : register(t11);
SamplerState s_environmentSampler : register(s11);

VertexOutPixelIn VertexMain(VertexInput input)
{
    VertexOutPixelIn output;
    float4 worldPos = mul(c_modelToWorld, float4(input.a_position, 1.f));
    float4 cameraPos = mul(c_worldToCamera, worldPos);
    float4 renderPos = mul(c_cameraToRender, cameraPos);
    float4 clipPos = mul(c_renderToClip, renderPos);
    output.v_position = clipPos.xyww;
    output.v_direction = worldPos.xyz - c_cameraWorldPos;
    return output;
}

float4 PixelMain(VertexOutPixelIn input) : SV_Target0
{
    return t_environmentCubemap.SampleLevel(s_environmentSampler, normalize(input.v_direction), 0.0f );
}