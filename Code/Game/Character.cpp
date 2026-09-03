#include "Game/Character.hpp"
#include "Game/CharacterDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"
#include "Engine/Animation/Animator.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Model/ModelAssets.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "Weapon.hpp"
#include "HitReactAbility.hpp"
#include "Engine/GameFramework/CylinderComponent.hpp"
#include "AIController.hpp"

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, CharacterDefinition const& characterDef )
    : m_game( game )
    , m_characterDef( characterDef )
{
    m_skeletalMeshDef = SkeletalMeshDefinition::GetDefinitionById( m_characterDef.m_skeletalMesh );
    GUARANTEE_OR_DIE( m_skeletalMeshDef, "SkeletalMeshDefinition is not found" );

    for ( WeaponDefinition const* weaponDef : m_characterDef.m_weaponDefs )
    {
        if ( !weaponDef ) { continue; };
        m_weapons.push_back( new Weapon( this, *weaponDef ) );
    }

    m_asc = new AbilitySystemComponent( this );
    m_asc->InitializeAttributes( m_characterDef.m_ascDef->m_initialAttributeValues );
    m_asc->GrantAbilities( m_characterDef.m_ascDef->m_abilityDefs );

    m_asc->GiveAbility( new HitReactAbility(), GameplayTagManager::Get().RequestTag( "Event.HitReact" ) );  // todo
    AddComponent( m_asc );

    SkeletalModel const&   skeletalModel         = g_engine->m_modelAssets->CreateOrGetSkeletalModel( m_skeletalMeshDef->m_filePath );
    SkeletalMeshComponent* skeletalMeshComponent = new SkeletalMeshComponent( this, skeletalModel );
    skeletalMeshComponent->m_animator->Initialize( m_characterDef.m_animationGraphDef, m_characterDef.m_animationSetDef );
    AddComponent( skeletalMeshComponent );
    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_skeletalMeshDef->m_axes.at( "x" ), m_skeletalMeshDef->m_axes.at( "y" ), m_skeletalMeshDef->m_axes.at( "z" ) );

    CylinderComponent* cylinderComponent = new CylinderComponent( this, 0.f, 1.8f, 0.5f );
    AddComponent( cylinderComponent );
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    Actor::Update();

    for ( Weapon* weapon : m_weapons )
    {
        if ( weapon ) { weapon->Update(); }
    }

    SkeletalMeshComponent* skeletalMeshComponent = GetComponentByClass< SkeletalMeshComponent >();

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
        m_velocity = Vec3::ZERO;
        return;
    }

    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );

    m_position += m_velocity * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitSkinned );
    g_engine->m_render->SetMaterialConstants( m_skeletalMeshDef->m_metallic, m_skeletalMeshDef->m_roughness, m_skeletalMeshDef->m_ambientOcclusion, m_skeletalMeshDef->m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->UnbindPBRTextures();

    for ( Weapon* weapon : m_weapons )
    {
        if ( weapon ) { weapon->Render(); }
    }

    m_asc->DebugRender();
}

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