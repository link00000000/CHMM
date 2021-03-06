// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "vJoyDevice.h"
#include "xboxController.h"

#define NUM_XBOX_INPUT_TYPES 11

UINT dev_id = DEV_ID;

int main()
{
	const int XBOX_INPUT_TYPES[NUM_XBOX_INPUT_TYPES]{ XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_Y, XINPUT_GAMEPAD_X,
		XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_DOWN,
		XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_DPAD_RIGHT, XINPUT_GAMEPAD_START, XINPUT_GAMEPAD_BACK };

	vJoyDevice* virtualGuitars[XUSER_MAX_COUNT - 1];
	xboxController* xboxControllers[XUSER_MAX_COUNT];

	int controllerNum;

	SOCKET sock;
	struct sockaddr_in si_other;
	int slen;
	char buf[BUF_LEN];
	WSADATA wsa;
	int result;

	slen = sizeof(si_other);

	// Initialize winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0)
	{
		printf("Socket initialization failed. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Socket initialized.\n");

	// Create socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Socket created.\n");

	// Prepare sockaddr_in structure
	std::string server_addr = "0.0.0.0";
	printf("Server IP Address: ");
	std::cin >> server_addr;

	ZeroMemory(&si_other, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(server_addr.c_str());

	// Select controller to be shared
	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		xboxControllers[i] = new xboxController(i + 1);
		printf("Controller %d: %s\n", i + 1, xboxControllers[i]->IsConnected() ? "Connected" : "Disconnected");
	}
	while (true)
	{
		printf("Select a controller to share.\n");
		printf("Player Number: ");
		std::cin >> controllerNum;
		if (controllerNum >= 1 && controllerNum <= XUSER_MAX_COUNT)
		{
			if (xboxControllers[controllerNum - 1]->IsConnected())
			{
				printf("Controller %d selected.\n", controllerNum);
				break;
			}
			else
			{
				printf("Controller %d is not connected. Select a different controller.\n", controllerNum);
			}
		}
		else
		{
			printf("Controller %d is not a valid controller.\n", controllerNum);
		}
	}
	xboxController* controller = xboxControllers[controllerNum - 1];

	WORD prevState = controller->GetAllButtons();
	WORD curState = controller->GetAllButtons();
	bool isFirstLoop = true;

	// Initial connection to server
	result = sendto(sock, OPEN_CONNECTION, BUF_LEN, 0, (struct sockaddr*)&si_other, slen);
	if (result == SOCKET_ERROR)
	{
		printf("Send, failed. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	result = recvfrom(sock, buf, BUF_LEN, 0, (sockaddr*)&si_other, &slen);
	if (result == SOCKET_ERROR)
	{
		printf("Recv failed. Error Code: %d\n", WSAGetLastError());
		system("pause");
		exit(EXIT_FAILURE);
	}
	if (buf[0] != CONNECTION_ACCEPTED[0] || buf[0] == CONNECTION_REFUSED[0])
	{
		printf("Connection refused by server.\n");
		system("pause");
		exit(EXIT_SUCCESS);
	}
	printf("Connection to server established.\n");

	// Start communication
	while (true)
	{
		// Zeroize buffer
		ZeroMemory(&buf, BUF_LEN);

		// Poll controller for new inputs
		controller->PollController();
		curState = controller->GetAllButtons();

		// Check for change in inputs from last iteration
		if (prevState != curState && !isFirstLoop)
		{
			// Determine which button state changed
			for (int i = 0; i < NUM_XBOX_INPUT_TYPES; i++)
			{
				if ((prevState & XBOX_INPUT_TYPES[i]) != (curState & XBOX_INPUT_TYPES[i]))
				{
					bool btnState;
					switch (i)
					{
						// Green fret
					case 0:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_A);
						printf("Green %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, FRET_GREEN); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Red fret
					case 1:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_B);
						printf("Red %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, FRET_RED); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Yellow fret
					case 2:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_Y);
						printf("Yellow %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, FRET_YELLOW); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Blue fret
					case 3:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_X);
						printf("Blue %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, FRET_BLUE); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Orange fret
					case 4:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_LEFT_SHOULDER);
						printf("Orange %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, FRET_ORANGE); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Dpad up
					case 5:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_DPAD_UP);
						printf("Dpad-up %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, STRUM_UP); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Dpad down
					case 6:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_DPAD_DOWN);
						printf("Dpad-down %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, STRUM_DOWN); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Dpad left
					case 7:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_DPAD_LEFT);
						printf("Dpad-left %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, DPAD_LEFT); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
					case 8:
						// Dpad right
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_DPAD_RIGHT);
						printf("Dpad-right %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, DPAD_RIGHT); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Start
					case 9:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_START);
						printf("Start %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, START_BUTTON); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
						// Back
					case 10:
						btnState = controller->GetButtonState(XINPUT_GAMEPAD_BACK);
						printf("Back %s\n", btnState ? "Pressed" : "Released");
						strcat(buf, BACK_BUTTON); strcat(buf, btnState ? STATE_DOWN : STATE_UP);
						break;
					}
					result = sendto(sock, buf, BUF_LEN, 0, (sockaddr*)&si_other, slen);
					if (result == SOCKET_ERROR)
					{
						printf("Send failed. Error Code: %d\n", WSAGetLastError());
						system("pause");
						exit(EXIT_FAILURE);
					}
				}
			}
		}

		// Move current state to previous state for next iteration
		ZeroMemory(&prevState, sizeof(prevState));
		memcpy(&prevState, &curState, sizeof(curState));
		ZeroMemory(&curState, sizeof(curState));

		isFirstLoop = false;

		//ZeroMemory(&buf, BUF_LEN);
		//result = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr*)&si_other, &slen);
		//if (result == SOCKET_ERROR)
		//{
		//	printf("Recv failed. Error Code: %d\n", WSAGetLastError());
		//	system("pause");
		//	exit(EXIT_FAILURE);
		//}
		Sleep(1);
	}

	closesocket(sock);
	WSACleanup();

    return 0;
}

