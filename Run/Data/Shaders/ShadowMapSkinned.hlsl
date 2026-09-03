// ShadowMapSkinned.hlsl

//------------------------------------------------------------------------------------------------
struct Light
{
    float4 c_color;
    float3 c_worldPosition;
    float EMPTY_PADDING;

    float3 c_spotForward;
    float c_ambience;

    float c_innerRadius;
    float c_outerRadius;
    float c_innerDotThreshold;
    float c_outerDotThreshold;
};

//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
    float3 a_tangent : VERTEX_TANGENT;
    float3 a_bitangent : VERTEX_BITANGENT;
    float3 a_normal : VERTEX_NORMAL;
    int4 a_jointIndices : VERTEX_JOINTINDICES;
    float4 a_jointWeights : VERTEX_JOINTWEIGHTS;
};

//------------------------------------------------------------------------------------------------
struct VertexOutput
{
    float4 v_position : SV_Position;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
    float4x4 c_modelToWorld;
    float4 c_modelTint;
};

//------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 8

cbuffer LightConstants : register(b4)
{
    float4 c_sunColor;
    float3 c_sunNormal;
    int c_numLights;

    Light c_lightsArray[MAX_LIGHTS];

    float4x4 c_lightViewMatrix;
    float4x4 c_lightCameraToRenderMatrix;
    float4x4 c_lightProjectionMatrix;
};

//------------------------------------------------------------------------------------------------
cbuffer SkinConstants : register(b7)
{
    float4x4 c_skinMatrices[128];
};

//------------------------------------------------------------------------------------------------
float4 CalculateSkinnedPosition(VertexInput input)
{
    float4 modelPosition = float4(input.a_position, 1.0f);

    float4 skinnedPosition =
        mul(
            c_skinMatrices[input.a_jointIndices.x],
            modelPosition) * input.a_jointWeights.x;

    skinnedPosition +=
        mul(
            c_skinMatrices[input.a_jointIndices.y],
            modelPosition) * input.a_jointWeights.y;

    skinnedPosition +=
        mul(
            c_skinMatrices[input.a_jointIndices.z],
            modelPosition) * input.a_jointWeights.z;

    skinnedPosition +=
        mul(
            c_skinMatrices[input.a_jointIndices.w],
            modelPosition) * input.a_jointWeights.w;

    skinnedPosition.w = 1.0f;

    return skinnedPosition;
}

//------------------------------------------------------------------------------------------------
VertexOutput VertexMain(VertexInput input)
{
    VertexOutput output;

    float4 skinnedPosition =
        CalculateSkinnedPosition(input);

    float4 worldPosition =
        mul(c_modelToWorld, skinnedPosition);

    float4 lightCameraPosition =
        mul(c_lightViewMatrix, worldPosition);

    float4 lightRenderPosition =
        mul(
            c_lightCameraToRenderMatrix,
            lightCameraPosition);

    output.v_position =
        mul(
            c_lightProjectionMatrix,
            lightRenderPosition);

    return output;
}

//------------------------------------------------------------------------------------------------
void PixelMain(VertexOutput input)
{
}