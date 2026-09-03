#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Math/Mat44.hpp"
#include <Engine/Math/Vec4.hpp>

//-----------------------------------------------------------------------------------------------
extern NamedStrings g_gameConfigBlackboard;

//-----------------------------------------------------------------------------------------------
enum class CameraMode
{
    FreeFly,
    TopDown
};

//-----------------------------------------------------------------------------------------------
enum class Faction
{
    UNKNOWN = -1,
    GOOD,
    EVIL,
    NEUTRAL,
    COUNT
};

//-----------------------------------------------------------------------------------------------
enum class ActorType
{
    UNKNOWN = -1,
    GOOD_PLAYER,
    EVIL_MELEE_SKELETON,
    COUNT
};

//-----------------------------------------------------------------------------------------------
struct Light
{
    float c_color[ 4 ];
    Vec3  c_worldPosition;
    float pad0;
    Vec3  c_spotForward;
    float c_ambience;
    float c_innerRadius;
    float c_outerRadius;
    float c_innerDotThreshold;
    float c_outerDotThreshold;
};

//-----------------------------------------------------------------------------------------------
struct LightConstants
{
    float c_sunColor[ 4 ];
    Vec3  c_sunNormal;
    int   c_numLights;
    Light c_lightsArray[ 8 ];
    Mat44 c_lightViewMatrix;
    Mat44 c_lightCameraToRenderMatrix;
    Mat44 c_lightProjectionMatrix;
    Vec4  c_iblSettings;
};

constexpr char const* PROP_DEFINITION         = "Data/Definitions/PropDefinitions.xml";
constexpr char const* WEAPON_DEFINITION       = "Data/Definitions/WeaponDefinitions.xml";
constexpr char const* ANIMATIONSET_DEFINITION = "Data/Definitions/AnimationSetDefinitions.xml";
constexpr char const* CUBE_TEXTURE            = "Data/Textures/prototype_dark.png";
constexpr char const* CYLINDER_TEXTURE        = "Data/Textures/texture_02.png";
constexpr float       IMGUI_LINEWIDTH         = 150.f;