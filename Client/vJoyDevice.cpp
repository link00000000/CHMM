#include "VjoyDevice.h"

vJoyDevice::vJoyDevice()
{
	// Zeroize buttons
	ZeroMemory(&buttons, sizeof(buttons));

	// Check if vJoy is enabled
	if (!vJoyEnabled())
	{
		printf("Failed getting vJoy drivers. Make you to install vJoy drivers before running this program.\n");
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("vJoy drivers found\n");

	// Get the state of vJoy device
	do {
		status = GetVJDStatus(deviceId);
		switch (status)
		{
		case VJD_STAT_OWN:
			printf("Device %d is already owned by this feeder\n", deviceId);
			break;
		case VJD_STAT_FREE:
			printf("Device %d is free\n", deviceId);
			break;
		case VJD_STAT_BUSY:
			printf("Device %d is already owned by another feeder. Trying a different device id.\n", deviceId);
			deviceId++;
			break;
		case VJD_STAT_MISS:
			printf("Device not installed or disabled.\n");
			system("pause");
			exit(EXIT_FAILURE);
			break;
		default:
			printf("An unexpected error has occurred.\n");
			system("pause");
			exit(EXIT_FAILURE);
			break;
		}
	} while (status == VJD_STAT_BUSY);

	// Acquire vJoy Device
	if ((status == VJD_STAT_OWN || (status == VJD_STAT_FREE) && (!AcquireVJD(deviceId))))
	{
		printf("Failed to acquire vJoy device %d.\n", deviceId);
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("Acquired vJoy device %d.\n", deviceId);

	ResetVJD(deviceId);
	resetInputs();
}

vJoyDevice::~vJoyDevice(void)
{
	RelinquishVJD(deviceId);
}

// Reset all inputs on device to defaults
void vJoyDevice::resetInputs(void)
{
	// Reset values of buttons to defaults
	for (int buttonId = 1; buttonId <= sizeof(buttons); buttonId++)
	{
		SetBtn(buttons[buttonId - 1], deviceId, buttonId);
	}
	setAxis(HID_USAGE_X, xAxis);
	setAxis(HID_USAGE_Y, yAxis);
	setAxis(HID_USAGE_Z, zAxis);
	setAxis(HID_USAGE_RX, RxAxis);
	setAxis(HID_USAGE_RY, RyAxis);
	setAxis(HID_USAGE_RZ, 0x0);
	setAxis(HID_USAGE_SL0, 0x0);
	setAxis(HID_USAGE_SL1, 0x0);
	setAxis(HID_USAGE_WHL, 0x0);
}

// Set button to on or off
void vJoyDevice::setButton(UINT buttonId, bool value)
{
	SetBtn(value, deviceId, buttonId);
}

// Press button
void vJoyDevice::pressButton(UINT buttonId)
{
	setButton(buttonId, true);
	Sleep(50);
	setButton(buttonId, false);
}
// Set axis to value 0x1-0x8000
// Axis Ids:
//		HID_USAGE_X <X Axis>
//		HID_USAGE_Y <Y Axis>
//		HID_USAGE_Z <Z Axis>
//		HID_USAGE_RX <RX Axis>
//		HID_USAGE_RY <RY Axis>
//		HID_USAGE_RZ <RZ Axis>
//		HID_USAGE_SL0 <Slider 0>
//		HID_USAGE_SL1 <Slider 1>
//		HID_USAGE_WHL <Wheel>
void vJoyDevice::setAxis(UINT axisId, long value)
{
	SetAxis(value, deviceId, axisId);
}

// Relinquish device with id
void vJoyDevice::relinquish(void)
{
	RelinquishVJD(deviceId);
}
