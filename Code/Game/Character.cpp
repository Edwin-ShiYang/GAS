#include "Game/Character.hpp"
#include "Game/CharacterDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerController.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"
#include "Engine/Animation/Animator.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/GameFramework/MovementComponent.hpp"
#include "Engine/Model/ModelAssets.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "Equipment.hpp"
#include "Engine/GameFramework/CylinderComponent.hpp"
#include "AIController.hpp"
#include <Engine/Core/VertexUtils.hpp>
#include <Engine/Math/MathUtils.hpp>

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, CharacterDefinition const& characterDef )
    : m_game( game )
    , m_characterDef( characterDef )
{
    m_skeletalMeshDef = SkeletalMeshDefinition::GetDefinitionById( m_characterDef.m_skeletalMesh );
    GUARANTEE_OR_DIE( m_skeletalMeshDef, "SkeletalMeshDefinition is not found" );

    for ( EquipmentDefinition const* weaponDef : m_characterDef.m_weaponDefs )
    {
        if ( !weaponDef ) { continue; };
        m_equipments.push_back( new Equipment( this, *weaponDef ) );
    }

    m_asc = new AbilitySystemComponent( this );
    m_asc->InitializeAttributes( m_characterDef.m_ascDef->m_initialAttributeValues );
    m_asc->GrantAbilities( m_characterDef.m_ascDef->m_grantedAbilityDefs );

    //m_asc->GiveAbility( new HitReactAbility(), GameplayTagManager::Get().RequestTag( "Event.HitReact" ) );  // todo

    AddComponent( m_asc );

    SkeletalModel const&   skeletalModel         = g_engine->m_modelAssets->CreateOrGetSkeletalModel( m_skeletalMeshDef->m_filePath );
    SkeletalMeshComponent* skeletalMeshComponent = new SkeletalMeshComponent( this, skeletalModel );
    skeletalMeshComponent->m_animator->Initialize( m_characterDef.m_animationGraphDef, m_characterDef.m_animationSetDef );
    AddComponent( skeletalMeshComponent );
    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_skeletalMeshDef->m_axes.at( "x" ), m_skeletalMeshDef->m_axes.at( "y" ), m_skeletalMeshDef->m_axes.at( "z" ) );

    CylinderComponent* cylinderComponent = new CylinderComponent( this, 0.f, 1.8f, 0.5f );
    AddComponent( cylinderComponent );

    MovementComponent* movementComponent = new MovementComponent( this );
    AddComponent( movementComponent );

    m_projectileSpawnPoint.m_position = m_characterDef.m_projectileSpawnOffset;
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    Actor::Update();

    SkeletalMeshComponent* skeletalMeshComponent = GetComponentByClass< SkeletalMeshComponent >();
    MovementComponent*     movementComponent     = GetComponentByClass< MovementComponent >();
    CylinderComponent*     cylinderComponent     = GetComponentByClass< CylinderComponent >();
    cylinderComponent->m_debugRender             = m_game->m_showDebugRender;

    if ( skeletalMeshComponent && skeletalMeshComponent->m_animator )
    {
        Vec3  localRootMotion   = skeletalMeshComponent->m_animator->ConsumeRootMotionDelta();
        Vec3  engineRootMotion  = m_toEngineMatrix.TransformVectorQuantity3D( localRootMotion );
        Mat44 characterRotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

        Vec3  worldRootMotion = characterRotation.TransformVectorQuantity3D( engineRootMotion );
        worldRootMotion.z     = 0.f;
        m_position += worldRootMotion;
    }

    if ( m_asc->HasActiveAbility() )
    {
        movementComponent->m_velocity = Vec3::ZERO;
        return;
    }

    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
    m_position += movementComponent->m_velocity * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitSkinned );
    g_engine->m_render->SetMaterialConstants( m_skeletalMeshDef->m_metallic, m_skeletalMeshDef->m_roughness, m_skeletalMeshDef->m_ambientOcclusion, m_skeletalMeshDef->m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->UnbindPBRTextures();

    Mat44 modelToWorldTransform         = Actor::GetModelToWorldTransform();
    Mat44 projectileSpawnPointTransform = m_projectileSpawnPoint.GetModelToWorldTransform();
    modelToWorldTransform.Append( projectileSpawnPointTransform );

    g_engine->m_render->BindShader( ShaderType::Default );
    std::vector< Vertex > verts;
    AddVertsForUVSphereZ3D( verts, Vec3::ZERO, 0.2f );
    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->SetModelConstants( modelToWorldTransform, Rgba8::WHITE );
    g_engine->m_render->DrawVertexArray( verts );
    g_engine->m_render->BindTexture( nullptr );

    RenderIndicator();
    m_asc->DebugRender();
}

//-----------------------------------------------------------------------------------------------
void Character::RenderIndicator() const
{
    std::string texturePath;

    /*
    "Physical",
        "Fire",
        "Poison",
        "Wind",
        "Frost",
        "Lightning"
        */

    if ( m_game->m_waveType == 1 )
    {
        texturePath = "Data/Images/FireIndicator.png";
    }
    else if ( m_game->m_waveType == 2 )
    {
        texturePath = "Data/Images/PoisonIndicator.png";
    }
    else if ( m_game->m_waveType == 3 )
    {
        texturePath = "Data/Images/WindIndicator.png";
    }
    else if ( m_game->m_waveType == 4 )
    {
        texturePath = "Data/Images/FrostIndicator.png";
    }
    else if ( m_game->m_waveType == 5 )
    {
        texturePath = "Data/Images/LightningIndicator.png";
    }

    if ( texturePath.empty() )
    {
        return;
    }
    Texture*              texture = g_engine->m_render->CreateOrGetTextureFromFile( texturePath.c_str() );
    std::vector< Vertex > verts;

    AddVertsForQuad3D( verts, Vec3( 0.0f, -0.5f, -0.5f ), Vec3( 0.0f, 0.5f, -0.5f ), Vec3( 0.0f, 0.5f, 0.5f ), Vec3( 0.0f, -0.5f, 0.5f ) );
    g_engine->m_render->BindShader( ShaderType::VFX );
    g_engine->m_render->BindTextureWithSampler( { texture, SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE } );
    g_engine->m_render->SetMaterialConstants( 0.f, 0.5f, 1.f, m_game->m_indicatorIntensity );
    g_engine->m_render->SetBlendMode( BlendMode::ADDITIVE );
    g_engine->m_render->SetDepthMode( DepthMode::READ_ONLY_LESS_EQUAL );

    Mat44 transform;
    transform.SetTranslation3D( m_position + Vec3( 0.f, 0.f, 0.01f ) );
    transform.AppendYRotation( -90.f );
    transform.AppendScaleUniform3D( 2.f );

    g_engine->m_render->SetModelConstants( transform );

    g_engine->m_render->DrawVertexArray( verts );

    g_engine->m_render->UnbindTexture( ShaderResourceSlot::DIFFUSE );
    g_engine->m_render->SetMaterialConstants();

    g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
    g_engine->m_render->BindShader( ShaderType::Default );
}

//-----------------------------------------------------------------------------------------------
void Character::RenderShadow() const
{
    g_engine->m_render->BindShader( ShaderType::ShadowMapSkinned );
    if ( SkeletalMeshComponent* skeletalMeshComponent = GetComponentByClass< SkeletalMeshComponent >() )
    {
        skeletalMeshComponent->Render();
    }
    g_engine->m_render->BindShader( ShaderType::Default );
}

//-----------------------------------------------------------------------------------------------
Mat44 Character::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform = Actor::GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );
    return modelToWorldTransform;
}

//-----------------------------------------------------------------------------------------------
void Character::PossessedBy( Controller* playerController )
{
    if ( !m_characterDef.m_aiEnabled )
    {
        m_controller = playerController;
    }
}

//-----------------------------------------------------------------------------------------------
Vec2 Character::GetPositionXY()
{
    return Vec2( m_position.x, m_position.y );
}

//-----------------------------------------------------------------------------------------------
void Character::SetPositionXY( Vec2 positionXY )
{
    m_position.x = positionXY.x;
    m_position.y = positionXY.y;
}

//-----------------------------------------------------------------------------------------------
bool Character::IsDead() const
{
    return m_isDead;
}