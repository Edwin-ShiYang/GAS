struct VertexInput
{
    float3 a_position : VERTEX_POSITION;
    float4 a_color : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

cbuffer CameraConstants : register(b2)
{
    float4x4 c_renderToClip;
    float4x4 c_cameraToRender;
    float4x4 c_worldToCamera;
};

cbuffer ModelConstants : register(b3)
{
    float4x4 c_modelToWorld;
    float4 c_modelTint;
};

cbuffer MaterialConstants : register(b9)
{
    float c_metallic;
    float c_roughness;
    float c_ambientOcclusion;
    float padding0;
    float4 c_emissiveColor;
    float c_emissiveIntensity;
    float3 padding1;
};

Texture2D<float4> t_diffuseTexture : register(t0);
SamplerState s_diffuseSampler : register(s0);

//-----------------------------------------------------------------------------------------------
VertexToPixel VertexMain(VertexInput input)
{
    VertexToPixel output;

    float4 modelPosition = float4(input.a_position, 1.0f);
    float4 worldPosition = mul(c_modelToWorld, modelPosition);
    float4 cameraPosition = mul(c_worldToCamera, worldPosition);
    float4 renderPosition = mul(c_cameraToRender, cameraPosition);

    output.position = mul(c_renderToClip, renderPosition);
    output.color = input.a_color * c_modelTint;
    output.uv = input.a_uvTexCoords;

    return output;
}

//-----------------------------------------------------------------------------------------------
float4 PixelMain(VertexToPixel input) : SV_Target0
{
    float4 textureColor = t_diffuseTexture.Sample(s_diffuseSampler, input.uv);
    float alpha = textureColor.a * input.color.a;

    clip(alpha - 0.001f);

    float3 hdrColor =
        textureColor.rgb *
        input.color.rgb *
        c_emissiveColor.rgb *
        c_emissiveIntensity *
        alpha;

    return float4(hdrColor, alpha);
}