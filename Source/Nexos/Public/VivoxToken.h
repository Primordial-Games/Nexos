// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VivoxCore.h"

struct FVivoxToken
{
    static void GenerateClientLoginToken(const ILoginSession& LoginSession, FString& OutToken);
    static void GenerateClientJoinToken(const IChannelSession& ChannelSession, FString& OutToken);
};
