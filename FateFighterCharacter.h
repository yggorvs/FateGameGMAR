#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FateFighterCharacter.generated.h"

UENUM(BlueprintType)
enum class EBlockState : uint8
{
    None,
    StandingBlock, // Segurar para trás: defende em pé e golpes por cima
    CrouchingBlock // Segurar para trás e baixo: defende rasteiras
};

UENUM(BlueprintType)
enum class EAttackHeight : uint8
{
    Mid,
    Low,      // Rasteira (exige defesa em baixo)
    Overhead, // Golpe por cima (exige defesa em pé)
    Throw     // Agarrão (vence a defesa)
};

UCLASS()
class FATEECLIPSE_API AFateFighterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFateFighterCharacter();

    // --- Atributos de Vida e Mana ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combate|Recursos")
    float MaxHealth = 1000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Recursos")
    float CurrentHealth;

    // Três segmentos de Mana (0.0 a 3.0), começa vazia a cada round
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Recursos")
    float CurrentMana = 0.0f;

    // --- Selos de Comando (Mestre fora da arena) ---
    // Três Selos por partida; passam ao round seguinte
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Selos")
    int32 CommandSeals = 3;

    // Limite de uma ativação de Selo por round
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Selos")
    bool bUsedSealThisRound = false;

    // --- Estado de Combate e Limites de Combo ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Estado")
    EBlockState BlockState = EBlockState::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Estado")
    bool bIsInHitstun = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Combo")
    int32 CurrentComboHits = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate|Combo")
    float CurrentAirComboTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combate|Combo")
    float MaxAirComboTime = 3.5f; // Limite de tempo no ar para conter sequências infinitas

    // --- Funções do Sistema ---
    UFUNCTION(BlueprintCallable, Category = "Combate|Round")
    void ResetForNewRound(int32 SavedSealsAtRoundStart);

    UFUNCTION(BlueprintCallable, Category = "Combate|Mana")
    void AddMana(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Combate|Selos")
    bool TryUseSealHeal(); // Custa 1 Selo: cura 15% da vida máxima

    UFUNCTION(BlueprintCallable, Category = "Combate|Selos")
    bool TryUseSealEscapeCombo(); // Custa 2 Selos: escapa de um combo

    UFUNCTION(BlueprintCallable, Category = "Combate|Especial")
    bool TryActivateEnhancedSpecial(); // Gasta 1 segmento de Mana

    UFUNCTION(BlueprintCallable, Category = "Combate|Supremo")
    bool TryActivateNoblePhantasm(); // Gasta 3 segmentos de Mana

    UFUNCTION(BlueprintCallable, Category = "Combate|Dano")
    bool ReceiveAttack(float BaseDamage, EAttackHeight Height, bool bIsNoblePhantasm, bool& bOutLegendaryFinish);
};