#include "WaveManager.hpp"
#include "Game.hpp"
#include "App.hpp"

//-----------------------------------------------------------------------------------------------
WaveManager::WaveManager( Game* game )
    : m_game( game )
    , m_nextWaveTimer( 3.0, game->m_clock )
{
}

//-----------------------------------------------------------------------------------------------
void WaveManager::StartWave()
{
    if ( !m_nextWaveTimer.IsStopped() )
    {
        return;
    }
    m_nextWaveTimer.Start();
}

//-----------------------------------------------------------------------------------------------
void WaveManager::Update()
{
    if ( m_nextWaveTimer.HasPeriodElapsed() )
    {
        m_nextWaveTimer.Stop();
        ++m_currentWave;

        /*
        int const waveBudget      = 3 + static_cast< int >( ( m_currentWave - 1 ) * 1.25f );
        int       remainingBudget = waveBudget;
        while ( remainingBudget > 0 )
        {
            int maxEnemyType = 0;

            if ( m_currentWave >= 3 && remainingBudget >= 2 )
            {
                maxEnemyType = 1;
            }

            if ( m_currentWave >= 5 && remainingBudget >= 3 )
            {
                maxEnemyType = 2;
            }

            if ( m_currentWave >= 7 && remainingBudget >= 4 )
            {
                maxEnemyType = 3;
            }

            if ( m_currentWave >= 10 && remainingBudget >= 5 )
            {
                maxEnemyType = 4;
            }

            int const   enemyType = g_rng.RollRandomIntInRange( 0, maxEnemyType );

            std::string characterId;
            int         enemyCost = 1;

            switch ( enemyType )
            {
                case 0:
                    characterId = "Skeleton";
                    enemyCost   = 1;
                    break;

                case 1:
                    characterId = "Summoner";
                    enemyCost   = 2;
                    break;

                case 2:
                    characterId = "Priest";
                    enemyCost   = 3;
                    break;

                case 3:
                    characterId = "GoatDemon";
                    enemyCost   = 4;
                    break;

                case 4:
                    characterId = "HornedDemon";
                    enemyCost   = 5;
                    break;
            }

            CharacterDefinition const& characterDef = CharacterDefinition::GetDefinitionById( characterId );

            m_game->SpawnEnemy( characterDef );

            remainingBudget -= enemyCost;
        }
        */
        CharacterDefinition const& skeletonDef = CharacterDefinition::GetDefinitionById( "Skeleton" );
        m_game->SpawnEnemy( skeletonDef );

        CharacterDefinition const& hornedDemonDef = CharacterDefinition::GetDefinitionById( "HornedDemon" );
        m_game->SpawnEnemy( hornedDemonDef );

        CharacterDefinition const& goatDemonDef = CharacterDefinition::GetDefinitionById( "GoatDemon" );
        m_game->SpawnEnemy( goatDemonDef );

        CharacterDefinition const& priestDef = CharacterDefinition::GetDefinitionById( "Priest" );
        m_game->SpawnEnemy( priestDef );

        CharacterDefinition const& summonerDef = CharacterDefinition::GetDefinitionById( "Summoner" );
        m_game->SpawnEnemy( summonerDef );
    }
}