#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <XInput.h>
#include "stdafx.h"

#pragma comment(lib, "XInput.lib")

// XBOX Controller Class Definition
class xboxController
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
	WORD buttons;
	XINPUT_STATE GetControllerState();
public:
	xboxController(int playerNumber);
	void PollController(void);
	bool IsConnected(void);
	void Vibrate(int leftVal = 0, int rightVal = 0);
	bool GetButtonState(int button);
	WORD GetAllButtons(void);
};

#endif