//------------------------------------------------------------------------------------------------
float3 DecodeRGBToXYZ(float3 color)
{
    return (color * 2.0) - 1.0;
}

//------------------------------------------------------------------------------------------------
struct Light
{
    float4 c_color;
    float3 c_worldPosition;
    float  EMPTY_PADDING;
    float3 c_spotForward;
    float  c_ambience; 
    float  c_innerRadius;
    float  c_outerRadius; 
    float  c_innerDotThreshold; 
    float  c_outerDotThreshold; 
};


//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 a_position     : VERTEX_POSITION;
    float4 a_color        : VERTEX_COLOR;
    float2 a_uvTexCoords  : VERTEX_UVTEXCOORDS;
    float3 a_tangent      : VERTEX_TANGENT;
    float3 a_bitangent    : VERTEX_BITANGENT;
    float3 a_normal       : VERTEX_NORMAL;
    int4   a_jointIndices : VERTEX_JOINTINDICES;
    float4 a_jointWeights : VERTEX_JOINTWEIGHTS;
	
	uint   a_vertexID	 : SV_VertexID;
};

//------------------------------------------------------------------------------------------------
struct VertexOutPixelIn 
{
	float4 v_position		: SV_Position;
	float4 v_color			: SURFACE_COLOR;
	float2 v_uvTexCoords	: SURFACE_UVTEXCOORDS;
    float3 v_worldPos       : WORLD_POSITION;
    float3 v_worldTangent   : WORLD_TANGENT;
    float3 v_worldBitangent : WORLD_BITANGENT;
    float3 v_worldNormal    : WORLD_NORMAL;
    
    float4 v_lightClipPosition : LIGHT_CLIP_POSITION;
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4	c_renderToClip;	
	float4x4	c_cameraToRender;
	float4x4	c_worldToCamera;
    float3      c_cameraWorldPos;
    float       EMPTY_PADDING0;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register( b3 )
{
	float4x4	c_modelToWorld;		
	float4		c_modelTint;	
};

//------------------------------------------------------------------------------------------------
#define MAX_LIGHTS 8
cbuffer LightConstants : register( b4 )
{
    float4   c_sunColor;
    float3   c_sunNormal;
    int      c_numLights;
    Light    c_lightsArray[ MAX_LIGHTS ];
    float4x4 c_lightViewMatrix;
    float4x4 c_lightCameraToRenderMatrix;
    float4x4 c_lightProjectionMatrix;
};

//------------------------------------------------------------------------------------------------
cbuffer SkinConstants : register(b7)
{
    float4x4 c_skinMatrices[ 128 ];
};

//------------------------------------------------------------------------------------------------
cbuffer MaterialConstants : register( b9 )
{
    float  c_metallic;
    float  c_roughness;
    float  c_ambientOcclusion;
    float  EMPTY_PADDING2;
    float4 c_emissiveColor;
    float  c_emissiveIntensity;
    float3 EMPTY_PADDING3;
};

//------------------------------------------------------------------------------------------------
Texture2D<float4>	t_diffuseTexture    : register( t0 );
SamplerState		s_diffuseSampler    : register( s0 );
                                        
Texture2D<float4>   t_normalTexture     : register( t1 );
SamplerState        s_normalSampler     : register( s1 );

Texture2D<float4>   t_specGlossEmitTexture : register(t2);
SamplerState        s_specGlossEmitSampler : register(s2);
                                        
Texture2D<float4>   t_roughnessTexture  : register( t3 );
SamplerState        s_roughnessSampler  : register( s3 );
                                        
Texture2D<float4>   t_aoTexture         : register( t4 );
SamplerState        s_aoSampler         : register( s4 );
                                        
Texture2D<float4>   t_metallicTexture   : register( t5 );
SamplerState        s_metallicSampler   : register( s5 );

Texture2D<float> t_shadowMapTexture : register(t10);
SamplerComparisonState s_shadowMapSampler : register(s10);

TextureCube<float4> t_irradianceCubemap : register(t12);
SamplerState s_irradianceSampler : register(s12);


TextureCube<float4> t_prefilteredCubemap : register(t13);
SamplerState s_prefilteredSampler : register(s13);

Texture2D<float2> t_brdfLUT : register(t14);
SamplerState s_brdfLUTSampler : register(s14);


//------------------------------------------------------------------------------------------------
VertexOutPixelIn VertexMain( VertexInput input )
{
	VertexOutPixelIn output;
	
	float4 modelPos = float4( input.a_position, 1.0 );	
    
    int joint0 = input.a_jointIndices.x;
    int joint1 = input.a_jointIndices.y;
    int joint2 = input.a_jointIndices.z;
    int joint3 = input.a_jointIndices.w;
    
    float weight0 = input.a_jointWeights.x;
    float weight1 = input.a_jointWeights.y;
    float weight2 = input.a_jointWeights.z;
    float weight3 = input.a_jointWeights.w;
    
    float4 p0 = mul(c_skinMatrices[joint0], modelPos);
    float4 p1 = mul(c_skinMatrices[joint1], modelPos);
    float4 p2 = mul(c_skinMatrices[joint2], modelPos);
    float4 p3 = mul(c_skinMatrices[joint3], modelPos);
    
    float4 skinnedPos;
    skinnedPos.x = p0.x * weight0 + p1.x * weight1 + p2.x * weight2 + p3.x * weight3;
    skinnedPos.y = p0.y * weight0 + p1.y * weight1 + p2.y * weight2 + p3.y * weight3;
    skinnedPos.z = p0.z * weight0 + p1.z * weight1 + p2.z * weight2 + p3.z * weight3;
    skinnedPos.w = 1.0f;
    
    
    float4 worldPos     = mul(c_modelToWorld, skinnedPos );
	float4 cameraPos	= mul( c_worldToCamera, worldPos );		
	float4 renderPos	= mul( c_cameraToRender, cameraPos );	
	float4 clipPos		= mul( c_renderToClip, renderPos );	
	
    float4 modelTangent     = float4( input.a_tangent, 0.0 );
    float4 modelBitangent   = float4( input.a_bitangent, 0.0 );
    float4 modelNormal      = float4( input.a_normal, 0.0 );
                            
    float4 worldTangent     = mul( c_modelToWorld, modelTangent );
    float4 worldBitangent   = mul( c_modelToWorld, modelBitangent );
    float4 worldNormal      = mul( c_modelToWorld, modelNormal );
    
    float4 lightCameraPos   = mul( c_lightViewMatrix, worldPos );
    float4 lightRenderPos   = mul(c_lightCameraToRenderMatrix, lightCameraPos);
    float4 lightClipPos     = mul( c_lightProjectionMatrix, lightRenderPos );
    output.v_lightClipPosition = lightClipPos;
    
	output.v_position		= clipPos;
	output.v_color			= input.a_color;
    output.v_uvTexCoords    = input.a_uvTexCoords;
    
    output.v_worldPos       = worldPos.xyz;
    output.v_worldTangent   = worldTangent.xyz;
    output.v_worldBitangent = worldBitangent.xyz;
    output.v_worldNormal    = worldNormal.xyz;
	
	return output;
}


//------------------------------------------------------------------------------------------------
float3 FresnelSchlickRoughness( float cosTheta, float3 baseReflectivity, float roughness )
{
    float3 grazingReflectivity = max( float3( 1.0f - roughness, 1.0f - roughness, 1.0f - roughness ), baseReflectivity );
    return baseReflectivity + ( grazingReflectivity - baseReflectivity ) * pow( saturate( 1.0f - cosTheta ), 5.0f );
}


//------------------------------------------------------------------------------------------------
float4 PixelMain( VertexOutPixelIn input ) : SV_Target0
{
    float2 uvCoords = input.v_uvTexCoords;
    float4 diffuseTexel = t_diffuseTexture.Sample(s_diffuseSampler, uvCoords);
    diffuseTexel.rgb = pow(diffuseTexel.rgb, float3(2.2f, 2.2f, 2.2f));
    
    float4 roughnessTexel = t_roughnessTexture.Sample(s_roughnessSampler, uvCoords);
    float4 normalTexel = t_normalTexture.Sample(s_normalSampler, uvCoords);
    
    float4 surfaceColor = input.v_color; // vertex color
    float4 modelColor = c_modelTint;
    
    float ambientOcclusion = saturate(t_aoTexture.Sample(s_aoSampler, uvCoords).r * c_ambientOcclusion);
	                    
    float3 pixelNormalTBNSpace = normalize(DecodeRGBToXYZ(normalTexel.rgb));
    
    float3 surfaceTangentWorldSpace = normalize(input.v_worldTangent);
    float3 surfaceBitangentWorldSpace = normalize(input.v_worldBitangent);
    float3 surfaceNormalWorldSpace = normalize(input.v_worldNormal);
    float3x3 tbnToWorld = float3x3(surfaceTangentWorldSpace, surfaceBitangentWorldSpace, surfaceNormalWorldSpace);
    float3 pixelNormalWorldSpace = normalize(mul(pixelNormalTBNSpace, tbnToWorld));
    
    float3 pixelToCameraDir = normalize(c_cameraWorldPos - input.v_worldPos);
    float3 surfaceToLightDirection = normalize(-c_sunNormal);
    float3 halfwayDirection = normalize(pixelToCameraDir + surfaceToLightDirection);
                                        
    float normalDotLight = saturate(dot(pixelNormalWorldSpace, surfaceToLightDirection));
    float normalDotCamera = saturate(dot(pixelNormalWorldSpace, pixelToCameraDir));
    float normalDotHalfway = saturate(dot(pixelNormalWorldSpace, halfwayDirection));
    float halfwayDotCamera = saturate(dot(halfwayDirection, pixelToCameraDir));
                                        
    float roughness = clamp(roughnessTexel.r * c_roughness, 0.04f, 1.0f);
    float roughnessSquared = roughness * roughness;
    float roughnessFourthPower = roughnessSquared * roughnessSquared;
    float normalDotHalfwaySquared = normalDotHalfway * normalDotHalfway;
    float distributionDenominatorBase = normalDotHalfwaySquared * (roughnessFourthPower - 1.0f) + 1.0f;
    float distributionDenominator = 3.14159265359f * distributionDenominatorBase * distributionDenominatorBase;
    float normalDistribution = roughnessFourthPower / max(distributionDenominator, 0.000001f);
                                        
    float4 diffuseColor = diffuseTexel * surfaceColor * modelColor;
    if (diffuseColor.a <= 0.001f)
    {
        discard;
    }
    
    float3 baseColor = diffuseColor.rgb;
    float metallic = saturate(t_metallicTexture.Sample(s_metallicSampler, uvCoords).r * c_metallic);
    
    float3 dielectricBaseReflectivity = float3(0.04f, 0.04f, 0.04f);
    float3 baseReflectivity = lerp(dielectricBaseReflectivity, baseColor, metallic);
    float oneMinusHalfwayDotCamera = 1.0f - halfwayDotCamera;
    float fresnelPower = pow(oneMinusHalfwayDotCamera, 5.0f);
    float3 fresnelReflectance = baseReflectivity + (1.0f - baseReflectivity) * fresnelPower;
    
    float geometryRoughness = roughness + 1.0f;
    float geometryFactor = geometryRoughness * geometryRoughness / 8.0f;
    float geometryFromCameraDenominator = normalDotCamera * (1.0f - geometryFactor) + geometryFactor;
    float geometryFromCamera = normalDotCamera / max(geometryFromCameraDenominator, 0.000001f);
    float geometryFromLightDenominator = normalDotLight * (1.0f - geometryFactor) + geometryFactor;
    float geometryFromLight = normalDotLight / max(geometryFromLightDenominator, 0.000001f);
    float geometryOcclusion = geometryFromCamera * geometryFromLight;
    
    
    float3 specularNumerator = normalDistribution * fresnelReflectance * geometryOcclusion;
    float specularDenominator = 4.0f * normalDotCamera * normalDotLight;
    float3 specularReflection = specularNumerator / max(specularDenominator, 0.000001f);
	

    float3 specularEnergyContribution = fresnelReflectance;
    float3 diffuseEnergyContribution = float3(1.0f, 1.0f, 1.0f) - specularEnergyContribution;
    diffuseEnergyContribution *= 1.0f - metallic;
    
    float3 diffuseReflection = diffuseEnergyContribution * baseColor / 3.14159265359f;
    float3 incomingLightRadiance = c_sunColor.rgb * c_sunColor.w;
    
    float3 directLighting = (diffuseReflection + specularReflection) * incomingLightRadiance * normalDotLight;
    

    // Shadow mapping
    float3 shadowNDC = input.v_lightClipPosition.xyz / input.v_lightClipPosition.w;
    float2 shadowUV = float2( shadowNDC.x * 0.5f + 0.5f, -shadowNDC.y * 0.5f + 0.5f );
    float currentDepth = shadowNDC.z;
    bool insideShadowMap = shadowUV.x >= 0.0f && shadowUV.x <= 1.0f && shadowUV.y >= 0.0f && shadowUV.y <= 1.0f && currentDepth >= 0.0f && currentDepth <= 1.0f;
    float shadowFactor = 1.0f;
    if (insideShadowMap)
    {
        uint shadowWidth;
        uint shadowHeight;

        t_shadowMapTexture.GetDimensions( shadowWidth, shadowHeight );
        float2 texelSize = 1.0f / float2( shadowWidth, shadowHeight );
        float bias = max( 0.0008f * (1.0f - normalDotLight), 0.0001f );

        float compareDepth = currentDepth - bias;
        float visibility = 0.0f;
        float totalWeight = 0.0f;

        // 5x5 weighted PCF.
        [unroll]
            for (int y = -2; y <= 2; ++y)
            {
            [unroll]
                for (int x = -2; x <= 2; ++x)
                {
                    float weight = ( 3.0f - abs( ( float ) x ) ) * ( 3.0f - abs( ( float ) y ) );

                    float2 sampleUV =
                    shadowUV +
                    float2(x, y) * texelSize;

                    float sampleVisibility =
                    t_shadowMapTexture.SampleCmpLevelZero( s_shadowMapSampler, sampleUV, compareDepth );

                    visibility += sampleVisibility * weight;
                    totalWeight += weight;
                }
            }

            visibility /= totalWeight;
            shadowFactor = lerp( 0.25f, 1.0f, visibility
        );
    }

   directLighting *= shadowFactor;


    float3 iblFresnel = FresnelSchlickRoughness(normalDotCamera, baseReflectivity, roughness);
    float3 specularIBLEnergy = iblFresnel;
    float3 diffuseIBLEnergy = (1.0f - specularIBLEnergy) * (1.0f - metallic);

    float3 irradiance = t_irradianceCubemap.Sample(s_irradianceSampler, pixelNormalWorldSpace).rgb;
    float3 diffuseIBL = irradiance * baseColor;

    float3 reflectionDirection = reflect(-pixelToCameraDir, pixelNormalWorldSpace);
    float  maxReflectionLOD = 4.0f;
    
    float3 prefilteredColor = t_prefilteredCubemap.SampleLevel(s_prefilteredSampler, reflectionDirection, roughness * maxReflectionLOD).rgb;
    
    
    float2 integratedBRDF = t_brdfLUT.Sample(s_brdfLUTSampler, float2(normalDotCamera, roughness)).rg;
    float3 specularIBL = prefilteredColor * (iblFresnel * integratedBRDF.x + integratedBRDF.y);

    float3 ambientLighting = (diffuseIBLEnergy * diffuseIBL + specularIBL) * ambientOcclusion;
    
    float3 emissiveTexture = t_specGlossEmitTexture.Sample(s_specGlossEmitSampler, input.v_uvTexCoords).rgb;
    float3 emissiveLighting = emissiveTexture * c_emissiveColor.rgb * c_emissiveIntensity;
    
    
    float3 finalColorRGB = directLighting + ambientLighting + emissiveLighting;
   return float4(finalColorRGB, diffuseColor.a);
}