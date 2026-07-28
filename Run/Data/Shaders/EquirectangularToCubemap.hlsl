//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 a_position    : VERTEX_POSITION;
    float4 a_color       : VERTEX_COLOR;
    float2 a_uvTexCoords : VERTEX_UVTEXCOORDS;
    float3 a_tangent     : VERTEX_TANGENT;
    float3 a_bitangent   : VERTEX_BITANGENT;
    float3 a_normal      : VERTEX_NORMAL;
    uint   a_vertexID    : SV_VertexID;
};

//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn
{
    float4 v_position  : SV_Position;
    float3 v_direction : TEXCOORD0;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 c_renderToClip;
    float4x4 c_cameraToRender;
    float4x4 c_worldToCamera;
    float3   c_cameraWorldPos;
    float    EMPTY_PADDING0;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
	float4x4	c_modelToWorld;	
	float4		c_modelTint;
};

//------------------------------------------------------------------------------------------------
Texture2D<float4>	t_cubemapTexture : register( t11 );
SamplerState		s_cubemapSampler : register( s11 );

//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain( VertexInput input )
{
	VertexOutPixelIn output;
	
	float4 modelPos     = float4( input.a_position, 1.0 );
	float4 worldPos		= mul( c_modelToWorld,   modelPos  );	
	float4 cameraPos	= mul( c_worldToCamera,  worldPos  );		
	float4 renderPos	= mul( c_cameraToRender, cameraPos );	
	float4 clipPos		= mul( c_renderToClip,   renderPos );		
    clipPos.x = -clipPos.x;
	output.v_position	= clipPos;
    output.v_direction  = worldPos.xyz - c_cameraWorldPos;
	
	return output; 
}

//------------------------------------------------------------------------------------------------
float2 DirectionToEquirectangularUV(float3 direction)
{
    static const float INV_PI     = 0.318309886f;
    static const float INV_TWO_PI = 0.159154943f;

    direction = normalize(direction);

    float2 uv;
    uv.x = atan2(direction.y, direction.x) * INV_TWO_PI + 0.5f;
    uv.y = 0.5f + asin(clamp(direction.z, -1.0f, 1.0f)) * INV_PI;
    
	return uv;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain( VertexOutPixelIn input) : SV_Target0
{
    float3 direction = normalize(input.v_direction);
    float2 uv = DirectionToEquirectangularUV(direction);

    return t_cubemapTexture.Sample( s_cubemapSampler, uv );
}
