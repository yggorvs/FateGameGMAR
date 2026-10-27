#include "FateFighterCharacter.h"

AFateFighterCharacter::AFateFighterCharacter()
{
    CurrentHealth = MaxHealth;
    CurrentMana = 0.0f;
    CommandSeals = 3;
}

void AFateFighterCharacter::ResetForNewRound(int32 SavedSealsAtRoundStart)
{
    // Vida e mana reiniciam entre rounds; Selos mantêm o valor guardado
    CurrentHealth = MaxHealth;
    CurrentMana = 0.0f;
    CommandSeals = SavedSealsAtRoundStart;
    bUsedSealThisRound = false;
    CurrentComboHits = 0;
    CurrentAirComboTime = 0.0f;
    bIsInHitstun = false;
}

void AFateFighterCharacter::AddMana(float Amount)
{
    // Obtida ao acertar ataques ou defender (máximo de 3 segmentos)
    CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, 3.0f);
}

bool AFateFighterCharacter::TryUseSealHeal()
{
    // Limite de 1 ativação por round; curar 15% da vida máxima custa 1 Selo
    if (bUsedSealThisRound || CommandSeals < 1 || CurrentHealth <= 0.0f)
        return false;

    CommandSeals -= 1;
    bUsedSealThisRound = true;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + (MaxHealth * 0.15f));
    return true;
}

bool AFateFighterCharacter::TryUseSealEscapeCombo()
{
    // Limite de 1 ativação por round; escapar de um combo custa 2 Selos
    if (bUsedSealThisRound || CommandSeals < 2 || !bIsInHitstun)
        return false;

    CommandSeals -= 2;
    bUsedSealThisRound = true;
    bIsInHitstun = false;
    CurrentComboHits = 0;
    CurrentAirComboTime = 0.0f;
    return true;
}

bool AFateFighterCharacter::TryActivateEnhancedSpecial()
{
    // 1 segmento fortalece um especial
    if (CurrentMana >= 1.0f)
    {
        CurrentMana -= 1.0f;
        return true;
    }
    return false;
}

bool AFateFighterCharacter::TryActivateNoblePhantasm()
{
    // 3 segmentos ativam o Nobre Fantasma (preparação defensável; cena começa após confirmar o acerto)
    if (CurrentMana >= 3.0f)
    {
        CurrentMana = 0.0f;
        return true;
    }
    return false;
}

bool AFateFighterCharacter::ReceiveAttack(float BaseDamage, EAttackHeight Height, bool bIsNoblePhantasm, bool& bOutLegendaryFinish)
{
    bOutLegendaryFinish = false;
    bool bBlocked = false;

    // Agarrões vencem a defesa; golpes por cima exigem defesa em pé; rasteiras exigem defesa em baixo
    if (Height != EAttackHeight::Throw)
    {
        if (Height == EAttackHeight::Overhead && BlockState == EBlockState::StandingBlock)
            bBlocked = true;
        else if (Height == EAttackHeight::Low && BlockState == EBlockState::CrouchingBlock)
            bBlocked = true;
        else if (Height == EAttackHeight::Mid && BlockState != EBlockState::None)
            bBlocked = true;
    }

    if (bBlocked)
    {
        // Ganha mana ao defender; o Nobre Fantasma não ativa a cena cinematográfica se for defendido
        AddMana(0.15f);
        return false;
    }

    // Dano decrescente para conter sequências infinitas
    CurrentComboHits++;
    float ScalingFactor = FMath::Max(0.2f, 1.0f - (CurrentComboHits - 1) * 0.1f);
    float FinalDamage = BaseDamage * ScalingFactor;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - FinalDamage);
    bIsInHitstun = true;

    // Vencer a partida com um Nobre Fantasma pode ativar uma Finalização de Lenda
    if (CurrentHealth <= 0.0f && bIsNoblePhantasm)
    {
        bOutLegendaryFinish = true;
    }

    return true;
}