#include "Game/GameplayAbility_Projectile.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/Character.hpp"
#include "Game/Projectile.hpp"
#include "Game/CharacterDefinition.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/GameFramework/MovementComponent.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"
#include <Engine/AbilitySystem/GameplayTask_WaitGameplayEvent.hpp>

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_Projectile::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    Actor*                 owneringActor         = m_ownerASC->m_owner;
    SkeletalMeshComponent* skeletalMeshComponent = owneringActor->GetComponentByClass< SkeletalMeshComponent >();
    if ( !skeletalMeshComponent || !skeletalMeshComponent->m_animator )
    {
        EndAbility();
        return false;
    }

    GameplayTag                     spawnEventTag    = GameplayTagManager::Get().RequestTag( "Event.Projectile.Spawn" );
    GameplayTask_WaitGameplayEvent& waitGameplayTask = m_ownerASC->CreateGameplayTask< GameplayTask_WaitGameplayEvent >( *this, spawnEventTag );
    waitGameplayTask.OnEvent( [ this ]( [[maybe_unused]] GameplayEventData const& data ) {
        SpawnProjectile();  // this->SpawnProjectile
    } );
    waitGameplayTask.Activate();

    skeletalMeshComponent->m_animator->BindNotify( "SpawnProjectile", [ asc = m_ownerASC, spawnEventTag ]() {
        GameplayEventData data;
        data.m_instigator = asc->m_owner;  // AbilitySystemComponent* asc = this->m_ownerASC;
        asc->SendGameplayEvent( spawnEventTag, data );
    } );

    GameplayTask_PlayAnimationAndWait& animationTask = m_ownerASC->CreateGameplayTask< GameplayTask_PlayAnimationAndWait >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    animationTask.Activate();

    return true;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Projectile::EndAbility()
{
    GameplayAbility::EndAbility();
    Animator* animator = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >()->m_animator;
    animator->UnbindNotify( "SpawnProjectile" );
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Projectile::UpdateAbility()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Projectile::SpawnProjectile()
{
    Character*                  ownerCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );
    ProjectileDefinition const& projectileDef  = ProjectileDefinition::GetDefinitionById( ownerCharacter->m_characterDef.m_projectileId );
    Projectile*                 projectile     = new Projectile( projectileDef, g_app->m_game );
    Mat44                       ownerTransform = ownerCharacter->Actor::GetModelToWorldTransform();
    Vec3                        spawnPosition  = ownerTransform.TransformPosition3D( ownerCharacter->m_projectileSpawnPoint.m_position );
    Vec3                        targetPosition = g_app->m_game->m_playerCharacer->m_position;
    targetPosition.z                           = spawnPosition.z;

    Vec3 direction                              = ownerTransform.GetIBasis3D().GetNormalized();
    projectile->m_position                      = spawnPosition;
    projectile->m_movementComponent->m_velocity = direction * 8.0f;

    if ( projectileDef.m_visualType == ProjectileVisualType::MESH )
    {
        projectile->m_orientation.m_yawDegrees = direction.GetOrientationAboutZDegrees();
    }

    g_app->m_game->SpawnProjectile( projectile );
}