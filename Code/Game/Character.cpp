#include "Game/Character.hpp"
#include "Game/Game.hpp"
#include "Game/CharacterAnimationController.hpp"

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, std::string const& name )
    : SkeletalMeshActor( game, name )
{
    m_animationController = new CharacterAnimationController( m_game->m_clock, this );
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    m_animationController->Update();
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    RenderMesh();
}