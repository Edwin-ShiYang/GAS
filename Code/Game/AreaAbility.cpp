#include "AreaAbility.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/AbilitySystem/PlayAnimationAndWaitTask.hpp"
#include "Character.hpp"
#include <Engine/Core/Engine.cpp>
#include <Engine/Math/Mat44.hpp>
#include <Engine/Core/VertexUtils.hpp>
#include "App.hpp"
#include "Game.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/VFX/SpriteSheetEffect.hpp"
#include "Engine/VFX/VFXSystem.hpp"

//-----------------------------------------------------------------------------------------------
bool AreaAbility::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    m_hitTargets.clear();

    Actor*                 owneringActor         = m_ownerASC->m_owner;
    SkeletalMeshComponent* skeletalMeshComponent = owneringActor->GetComponentByClass< SkeletalMeshComponent >();
    if ( !skeletalMeshComponent || !skeletalMeshComponent->m_animator )
    {
        EndAbility();
        return false;
    }

    PlayAnimationAndWaitTask& animationTask = m_ownerASC->CreateGameplayTask< PlayAnimationAndWaitTask >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    animationTask.Activate();

    skeletalMeshComponent->m_animator->BindNotify( "HitStart", [ this ]() { m_isAreaDetectionEnabled = true; } );
    skeletalMeshComponent->m_animator->BindNotify( "HitEnd", [ this ]() { m_isAreaDetectionEnabled = false; } );

    return true;
}

//-----------------------------------------------------------------------------------------------
void AreaAbility::UpdateAbility()
{
    if ( !m_isActive || !m_isAreaDetectionEnabled ) return;
    for ( Actor* actor : g_app->m_game->m_characters )
    {
        if ( actor == nullptr ) { continue; }
        Vec2 actorPosition = Vec2( actor->m_position.x, actor->m_position.y );
        if ( !IsPointInsideDisc2D( actorPosition, Vec2( m_ownerASC->m_owner->m_position.x, m_ownerASC->m_owner->m_position.y ), 5.0f ) )
        {
            continue;
        }

        bool alreadyHit = false;
        for ( Actor* hitTarget : m_hitTargets )
        {
            if ( hitTarget == actor )
            {
                alreadyHit = true;
                break;
            }
        }

        if ( alreadyHit )
        {
            continue;
        }

        Character* targetCharacter = dynamic_cast< Character* >( actor );
        if ( targetCharacter == nullptr || targetCharacter == m_ownerASC->m_owner )
        {
            continue;
        }

        AbilitySystemComponent* targetASC = targetCharacter->GetAbilitySystemComponent();
        if ( targetASC == nullptr )
        {
            continue;
        }

        m_hitTargets.push_back( actor );

        SpriteSheetEffect* effect = new SpriteSheetEffect( "Data/VFX/Sprite-sheet.png", IntVec2( 5, 1 ), 0, 4, 5.f, SpriteAnimPlaybackType::ONCE );
        effect->m_position        = Vec3( targetASC->m_owner->m_position.x, targetASC->m_owner->m_position.y, 1.0f );

        effect->m_lifeSpan = 1.0f;
        g_engine->m_vfxSystem->SpawnSpriteSheetEffect( effect );
    }
}

//-----------------------------------------------------------------------------------------------
void AreaAbility::DebugRender() const
{
    if ( !m_isAreaDetectionEnabled )
    {
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );

    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );

    Mat44 discTransform;
    discTransform.AppendTranslation3D( owningCharacter->m_position + Vec3( 0.f, 0.f, 0.01f ) );

    g_engine->m_render->SetModelConstants( discTransform );

    std::vector< Vertex > verts;
    AddVertsForDisc3D( verts, Vec3::ZERO, 5.0f, Rgba8::GREEN );

    g_engine->m_render->DrawVertexArray( verts );
}

//-----------------------------------------------------------------------------------------------
void AreaAbility::EndAbility()
{
    GameplayAbility::EndAbility();
    Animator* animator = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >()->m_animator;
    animator->UnbindNotify( "AnimEnd" );
}