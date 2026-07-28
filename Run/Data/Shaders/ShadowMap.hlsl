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
	float3	a_position		: VERTEX_POSITION;		
	float4	a_color			: VERTEX_COLOR;
	float2	a_uvTexCoords	: VERTEX_UVTEXCOORDS; 
	
	uint	a_vertexID	    : SV_VertexID;
};

//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn 
{
	float4 v_position		: SV_Position;
	float4 v_color			: SURFACE_COLOR;
	float2 v_uvTexCoords	: SURFACE_UVTEXCOORDS;
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
#define MAX_LIGHTS 8
cbuffer LightConstants : register(b4)
{
    float4   c_sunColor;
    float3   c_sunNormal;
    int      c_numLights;
    Light    c_lightsArray [ MAX_LIGHTS ];
    float4x4 c_lightViewMatrix;
    float4x4 c_lightCameraToRenderMatrix;
    float4x4 c_lightProjectionMatrix;
};


//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain( VertexInput input )
{
	VertexOutPixelIn output;
	
    float4 modelPos       = float4( input.a_position, 1.0 );
    float4 worldPos       = mul( c_modelToWorld, modelPos );
    float4 lightCameraPos = mul( c_lightViewMatrix, worldPos );
    float4 lightRenderPos = mul(c_lightCameraToRenderMatrix, lightCameraPos);
    float4 clipPos        = mul( c_lightProjectionMatrix, lightRenderPos );
	
	output.v_position		= clipPos;
	output.v_color			= input.a_color;
	output.v_uvTexCoords	= input.a_uvTexCoords;
	
	return output;
}


//------------------------------------------------------------------------------------------------
void PixelMain(VertexOutPixelIn input) 
{ 
}

