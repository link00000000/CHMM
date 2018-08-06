#pragma once

#include "stdafx.h"
#include "public.h"
#include <malloc.h>
#include <string>
#include <stdlib.h>
#include "vjoyinterface.h"
#include "Math.h"

// Default device ID (Used when ID not specified)
#define DEV_ID		1

class vJoyDevice
{
public:
	vJoyDevice(void);
	~vJoyDevice(void);

	void resetInputs(void);
	void setButton(UINT buttonId, bool value);
	void setAxis(UINT axisId, long value);
	void pressButton(UINT buttonId);

	void relinquish(void);

private:
	UINT deviceId = DEV_ID;
	VjdStat status;

	bool buttons[25];
	long xAxis = 0x0;
	long yAxis = 0x0;
	long zAxis = 0x0;
	long RxAxis = 0x0;
	long RyAxis = 0x0;
	long RzAxis = 0x0;
};

