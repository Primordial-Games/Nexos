#pragma once

#include "Engine/LocalPlayer.h"
#include "SlateBasics.h"
#include "SlateExtras.h"

class UNexosGameUserSettings;
class UNexosPersistentUser;

/** delegate called when changes are applied */
DECLARE_DELEGATE(FOnApplyChanges);

/** delegate called when a menu item is confirmed */
DECLARE_DELEGATE(FOnConfirmMenuItem);

/** delegate called when facebutton_left is pressed */
DECLARE_DELEGATE(FOnControllerFacebuttonLeftPressed);

/** delegate called down on the gamepad is pressed*/
DECLARE_DELEGATE(FOnControllerDownInputPressed);

/** delegate called up on the gamepad is pressed*/
DECLARE_DELEGATE(FOnControllerUpInputPressed);

/** delegate called when facebutton_down is pressed */
DECLARE_DELEGATE(FOnOnControllerFacebuttonDownPressed);


class FNexosFriends : public TSharedFromThis<FNexosFriends>
{
public:
	/** sets owning player controller */
	void Construct(ULocalPlayer* _PlayerOwner, int32 LocalUserNum);

	/** get current Friends values for display */
	void UpdateFriends(int32 NewOwnerIndex);

	/** UI callback for applying settings, plays sound */
	void OnApplySettings();

	/** applies changes in game settings */
	void ApplySettings();

	/** needed because we can recreate the subsystem that stores it */
	void TellInputAboutKeybindings();

	/** increment the counter keeping track of which user we're looking at */
	void IncrementFriendsCounter();

	/** decrement the counter keeping track of which user we're looking at */
	void DecrementFriendsCounter();

	/** view the profile of the selected user */
	void ViewSelectedFriendProfile();

	/** send an invite to the selected user */
	void InviteSelectedFriendToGame();

};

