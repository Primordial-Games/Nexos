// Fill out your copyright notice in the Description page of Project Settings.


#include "VivoxTracer.h"
#include "Nexos.h"

DEFINE_LOG_CATEGORY(LogNexosTracer);

void Tracer::_LogVerbose(const FString Name, const FString Message)
{
    SET_WARN_COLOR(COLOR_CYAN);
    UE_LOG(LogNexosTracer, Verbose, TEXT("%s() : %s"), *Name, *Message);
    CLEAR_WARN_COLOR();
}

void Tracer::_LogVeryVerbose(const FString Name, const FString Message)
{
    SET_WARN_COLOR(COLOR_BLUE);
    UE_LOG(LogNexosTracer, VeryVerbose, TEXT("%s() : %s"), *Name, *Message);
    CLEAR_WARN_COLOR();
}

void Tracer::_MethodPrologue(const FString Name, const FString Message)
{
    _LogVerbose(Name, TEXT("Entered."));
    if (!Message.IsEmpty())
    {
        _LogVeryVerbose(Name, Message);
    }
}