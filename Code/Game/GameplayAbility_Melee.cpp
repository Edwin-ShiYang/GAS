#include "Game/GameplayAbility_Melee.hpp"
#include "Game/Character.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/Animation/Animator.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "App.hpp"
#include "Game.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"
#include "Engine/VFX/TextEffect.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_Melee::ActivateAbility()
{
    GameplayAbility::ActivateAbility();

    SkeletalMeshComponent* skeletalMeshComponent = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >();
    GUARANTEE_OR_DIE( skeletalMeshComponent, Stringf( "skeletalMeshComponent is not found!" ) );

    // Animation
    GameplayTask_PlayAnimationAndWait& animationTask = m_ownerASC->CreateGameplayTask< GameplayTask_PlayAnimationAndWait >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    animationTask.Activate();

    // notify
    skeletalMeshComponent->m_animator->BindNotify( "HitStart", [ this ]() { m_isHitDetectionEnabled = true; } );
    skeletalMeshComponent->m_animator->BindNotify( "HitEnd", [ this ]() { m_isHitDetectionEnabled = false; } );

    m_hitTargets.clear();
    return true;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Melee::UpdateAbility()
{
    if ( !m_isActive )
    {
        return;
    }

    if ( m_isHitDetectionEnabled )
    {
        Vec3 forward       = m_ownerASC->m_owner->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
        Vec2 sectorForward = Vec2( forward.x, forward.y ).GetNormalized();
        Vec2 sectorTip     = Vec2( m_ownerASC->m_owner->m_position.x, m_ownerASC->m_owner->m_position.y );

        for ( Actor* actor : g_app->m_game->m_actors )
        {
            if ( actor == nullptr )
            {
                continue;
            }

            Vec2 actorPosition = Vec2( actor->m_position.x, actor->m_position.y );
            if ( !IsPointInsideDirectedSector2D( actorPosition, sectorTip, sectorForward, 120.f, 1.5f ) )
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
            if ( targetCharacter == nullptr )
            {
                continue;
            }

            AbilitySystemComponent* targetASC = targetCharacter->GetAbilitySystemComponent();
            if ( targetASC == nullptr )
            {
                continue;
            }

            m_hitTargets.push_back( actor );

            GameplayEffect gameplayEffect;
            gameplayEffect.m_gameplayEffectDef = m_definition->m_gameplayEffectDef;
            float             damageAmount     = m_ownerASC->ApplyGameplayEffectToTarget( gameplayEffect, targetASC );

            GameplayEventData gameplayEventData;
            gameplayEventData.m_instigator = m_ownerASC->m_owner;
            gameplayEventData.m_target     = targetASC->m_owner;
            targetASC->SendGameplayEvent( GameplayTagManager::Get().RequestTag( "Event.HitReact" ), gameplayEventData );

            GameplayCueParameters parameters;
            parameters.m_instigator = m_ownerASC->m_owner;
            parameters.m_target     = targetASC->m_owner;
            targetASC->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.Combat.HitImpact" ), parameters );

            std::string damageText = std::to_string( static_cast< int >( damageAmount ) );
            TextEffect* effect     = new TextEffect( damageText, Rgba8::RED, Rgba8::RED, 1.0f, 1.0f );
            effect->m_position     = Vec3( targetASC->m_owner->m_position.x, targetASC->m_owner->m_position.y, 1.0f );
            effect->m_duration     = 0.5f;
            effect->m_velocity     = ( targetASC->m_owner->m_position - m_ownerASC->m_owner->m_position ).GetNormalized();

            g_engine->m_vfxSystem->SpawnTextEffect( effect );

            //SpriteSheetEffect* effect = new SpriteSheetEffect( "Data/VFX/Sprite-sheet-sheet.png", IntVec2( 5, 1 ), 0, 4, 10.f, SpriteAnimPlaybackType::LOOP );
            //SpriteSheetEffect* effect = new SpriteSheetEffect( "Data/VFX/ice.png", IntVec2( 5, 3 ), 0, 10, 5.f, SpriteAnimPlaybackType::ONCE );

            /*
            SpriteSheetEffect* effect = new SpriteSheetEffect( "Data/VFX/ice_1.png", IntVec2( 4, 4 ), 0, 15, 10.f, SpriteAnimPlaybackType::ONCE );
            effect->m_position        = Vec3( targetASC->m_owner->m_position.x, targetASC->m_owner->m_position.y, 1.0f );

            effect->m_lifeSpan = 1.5f;
            g_engine->m_vfxSystem->SpawnEffect( effect );
            */
        }
    }
}

void GameplayAbility_Melee::DebugRender() const
{
    if ( !m_isHitDetectionEnabled )
    {
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );

    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );

    Mat44 sectorTransform;
    sectorTransform.AppendTranslation3D( owningCharacter->m_position + Vec3( 0.f, 0.f, 0.01f ) );

    EulerAngles yawOnlyOrientation;
    yawOnlyOrientation.m_yawDegrees   = owningCharacter->m_orientation.m_yawDegrees;
    yawOnlyOrientation.m_pitchDegrees = 0.f;
    yawOnlyOrientation.m_rollDegrees  = 0.f;
    sectorTransform.Append( yawOnlyOrientation.GetAsMatrix_IFwd_JLeft_KUp() );

    g_engine->m_render->SetModelConstants( sectorTransform );

    std::vector< Vertex > verts;
    AddVertsForSector( verts, Vec3::ZERO, 1.5f, 0.f, 120.f, Rgba8::GREEN );

    g_engine->m_render->DrawVertexArray( verts );
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Melee::EndAbility()
{
    GameplayAbility::EndAbility();
}

/*
    WaitDelayTask& delayTask = m_ownerASC->CreateGameplayTask< WaitDelayTask >( *this, 1.0f );
    delayTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    delayTask.Activate();
    */