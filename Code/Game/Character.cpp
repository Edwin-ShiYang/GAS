#include "Game/Character.hpp"
#include "Game/Game.hpp"
#include "Game/CharacterAnimationController.hpp"
#include "Game/PropDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/Prop.hpp"
#include "Game/MeleeAttackAbility.hpp"
#include "Game/AnimationSetDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, std::string const& name )
    : m_game( game )
{
    m_actorDef = PropDefinition::GetDefinitionById( name );
    GUARANTEE_OR_DIE( m_actorDef, Stringf( "actorDef is not found" ) )

    //   SkeletalModel* skeletalModel = ModelImporter::CreateOrGetSkeletonModelFromFile( m_actorDef->m_skeletalMeshDef->m_filePath );
    //   GUARANTEE_OR_DIE( skeletalModel, Stringf( "SkeletonModel is not found" ) )

    //    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_actorDef->m_skeletalMeshDef->m_axes[ "x" ], m_actorDef->m_skeletalMeshDef->m_axes[ "y" ], m_actorDef->m_skeletalMeshDef->m_axes[ "z" ] );
    //    AddComponent( new SkeletalMeshComponent( this, skeletalModel ) );

    //    m_position = m_actorDef->m_spawnPosition;

    //   m_animationController = new CharacterAnimationController( m_game->m_clock, this );

    m_asc = new AbilitySystemComponent( this );
    GUARANTEE_OR_DIE( m_actorDef->m_ascDef, "Character has no ASC definition" )

    m_asc->InitializeAttributes( m_actorDef->m_ascDef->GetAttributes() );
    GrantDefaultAbilities();
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
    delete m_animationController;
    m_animationController = nullptr;

    delete m_asc;
    m_asc = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    m_asc->Update();
    //m_animationController->Update();
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitSkinned );
    // g_engine->m_render->SetMaterialConstants( m_actorDef->m_skeletalMeshDef->m_metallic, m_actorDef->m_skeletalMeshDef->m_roughness, m_actorDef->m_skeletalMeshDef->m_ambientOcclusion, m_actorDef->m_skeletalMeshDef->m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->UnbindPBRTextures();
    m_asc->DebugRender();
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

//-----------------------------------------------------------------------------------------------
void Character::GrantDefaultAbilities()
{
    for ( GameplayAbilityDefinition const* abilityDef : m_actorDef->m_ascDef->m_abilityDefs )
    {
        if ( !abilityDef ) continue;

        if ( abilityDef->m_type == "MeleeAttack" )
        {
            GameplayAbility* meleeAttackAbility = new MeleeAttackAbility();
            meleeAttackAbility->m_definition    = abilityDef;
            m_asc->GrantAbility( meleeAttackAbility );
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Character::PlayAbilityAnimation( std::string const& animationName )
{
    m_animationController->m_clip = m_actorDef->m_animSetDef->m_animClips[ animationName ];
}

//-----------------------------------------------------------------------------------------------
Mat44 Character::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform = Actor::GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );

    return modelToWorldTransform;
}