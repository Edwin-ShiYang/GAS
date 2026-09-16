#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class AnimationSetDefinition;
class AbilitySystemComponentDefinition;
class AnimationGraphDefinition;
class EquipmentDefinition;

//-----------------------------------------------------------------------------------------------
class CharacterDefinition
{
public:
    CharacterDefinition()  = default;
    ~CharacterDefinition() = default;

    void                                       LoadFromXmlElement( XmlElement const& element );
    void                                       LoadWeaponDefsFromXmlElement( XmlElement const* element );

    static void                                InitializeDefinitions();
    static void                                ClearDefinitions();
    static CharacterDefinition const&          GetDefinitionById( std::string const& id );
    static std::vector< CharacterDefinition* > s_definitions;

    std::string                                m_id;
    std::string                                m_skeletalMesh;

    AbilitySystemComponentDefinition const*    m_ascDef            = nullptr;
    AnimationSetDefinition*                    m_animationSetDef   = nullptr;
    AnimationGraphDefinition const*            m_animationGraphDef = nullptr;
    std::vector< EquipmentDefinition const* >  m_weaponDefs;
    std::string                                m_primaryAbility;

    // AI
    bool                                       m_aiEnabled   = false;
    float                                      m_sightRadius = 0.f;
    std::string                                m_attackType;

    //Physics
    float                                      m_runSpeed      = 0.f;
    float                                      m_turnSpeed     = 0.f;
    float                                      m_attackRange   = 0.f;
    float                                      m_physicsRadius = 0.f;
    float                                      m_physicsHeight = 0.f;

    //Projectile
    std::string                                m_projectileId;
    Vec3                                       m_projectileSpawnOffset = Vec3::ZERO;
};