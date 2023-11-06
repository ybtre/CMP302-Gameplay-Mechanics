// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/** Macro to add on screen debug message
 * @param DS stands for DebugtoScreen;
 * @param text stands for the string text message you want to output to the screen;
 **/
#define DS(text) if( GEngine ) { GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT(text)); }

/**
 * 
 */
class PROJECTB_API Macros
{
public:
	Macros();
	~Macros();
};
