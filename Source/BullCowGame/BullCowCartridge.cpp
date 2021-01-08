// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
// #include "HiddenWordList.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"


void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArray(Words, *WordListPath);
    Isograms = GetValidWords(Words);
    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        ProcessGuess(PlayerInput);
    }
}

void UBullCowCartridge::SetupGame() 
{
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num()-1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    // Welcoming the player
    PrintLine(TEXT("Welcome to the BullCow game!"));
    PrintLine(TEXT("You have %i lives."), Lives);
    PrintLine(TEXT("Guess the %i letter word and \npress enter to continue..."), HiddenWord.Len());
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("Press enter to play again..."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess) 
{
    if (Guess == HiddenWord)
    {
        ClearScreen();
        PrintLine(TEXT("You have won! The word was indeed %s!"), *HiddenWord);
        EndGame();
        return;
    }

    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("Word should be %i letters long. Try again."), HiddenWord.Len()); 
        PrintLine(TEXT("You have %i lives left."), Lives);
        return;
    }

    if (!IsIsogram(Guess))
    {
        PrintLine(TEXT("No repeating letters, try again!"));
        return;
    }

    --Lives;
    PrintLine(TEXT("You have a lost a life, %i lives left!"), Lives);

    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("You have no lives left!"));
        PrintLine(TEXT("The hidden word was %s."), *HiddenWord);
        EndGame();
        return;
    }

    FBullCowCount Score = GetBullCows(Guess);
    PrintLine(TEXT("You have %i Bulls and %i Cows."), Score.Bulls, Score.Cows);
    PrintLine(TEXT("Guess again..."));
}

bool UBullCowCartridge::IsIsogram(const FString& Guess) const
{
    for (int32 Index = 0; Index < Guess.Len(); Index++)
    {
        for (int32 Comparison = Index + 1; Comparison < Guess.Len(); Comparison++)
        {
            if (Guess[Index] == Guess[Comparison])
            {
                return false;
            }
        }
        
    }

    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
    TArray<FString> ValidWords;
    for (FString CurrentWord : WordList)
    {
        if (IsIsogram(CurrentWord) && (CurrentWord.Len() >= 4 && CurrentWord.Len() < 8))
        {
            ValidWords.Emplace(*CurrentWord);
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }
        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}