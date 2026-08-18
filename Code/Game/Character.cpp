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

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, std::string const& name )
    : m_game( game )
{
    m_characterDef = CharacterDefinition::GetDefinitionById( name );
    GUARANTEE_OR_DIE( m_characterDef, "CharacterDefinition is not found" );

    m_skeletalMeshDef = SkeletalMeshDefinition::GetDefinitionById( m_characterDef->m_skeletalMesh );
    GUARANTEE_OR_DIE( m_skeletalMeshDef, "SkeletalMeshDefinition is not found" );

    SkeletalModel const& skeletalModel = g_engine->m_modelAssets->CreateOrGetSkeletalModel( m_skeletalMeshDef->m_filePath );

    m_asc = new AbilitySystemComponent( this );
    m_asc->InitializeAttributes( m_characterDef->m_ascDef->m_initialAttributeValues );
    m_asc->GrantAbilities( m_characterDef->m_ascDef->m_abilityDefs );
    AddComponent( m_asc );

    SkeletalMeshComponent* skeletalMeshComponent = new SkeletalMeshComponent( this, skeletalModel );
    skeletalMeshComponent->m_animator->Initialize( m_characterDef->m_animationGraphDef, m_characterDef->m_animationSetDef );
    AddComponent( skeletalMeshComponent );

    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_skeletalMeshDef->m_axes.at( "x" ), m_skeletalMeshDef->m_axes.at( "y" ), m_skeletalMeshDef->m_axes.at( "z" ) );
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    Actor::Update();

    if ( m_asc->HasActiveAbility() )
    {
        m_velocity = Vec3::ZERO;
        return;
    }

    float deltaSeconds    = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
    float distanceSquared = GetDistanceSquared3D( m_position, m_mouseTargetPos );

    if ( distanceSquared <= 0.1f )
    {
        m_velocity = Vec3::ZERO;
    }

    m_position += m_velocity * deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitSkinned );
    g_engine->m_render->SetMaterialConstants( m_skeletalMeshDef->m_metallic, m_skeletalMeshDef->m_roughness, m_skeletalMeshDef->m_ambientOcclusion, m_skeletalMeshDef->m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->UnbindPBRTextures();
    m_asc->DebugRender();
}

//-----------------------------------------------------------------------------------------------
Mat44 Character::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform = Actor::GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );
    return modelToWorldTransform;
}

//-----------------------------------------------------------------------------------------------
void Character::PossessedBy( PlayerController* playerController )
{
    m_playerController = playerController;
}

//-----------------------------------------------------------------------------------------------
AbilitySystemComponent* Character::GetAbilitySystemComponent() const
{
    return m_asc;
}

//-----------------------------------------------------------------------------------------------
AttributeSet* Character::GetAttributeSet() const
{
    return m_asc->m_attributeSet;
}