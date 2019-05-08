#include "DirectInputGamePad.h"

DirectInputGamePad::DirectInputGamePad(const HWND hwnd, unsigned int gamePadID, LPDIRECTINPUTDEVICE8 deviceHandle) : m_gamePadConnected(false), m_gamePadID(gamePadID), m_deviceHandle(deviceHandle), m_triggerThreshold(3800.0f)
{
	if (FAILED(m_deviceHandle->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE)))
	{
		std::cout << "Failed to Set DeviceHandle CooperativeLevel" << std::endl;
	}

	if (FAILED(m_deviceHandle->SetDataFormat(&c_dfDIJoystick)))
	{
		std::cout << "Failed to Set DeviceHandle DataFormat" << std::endl;
	}

	ResetGamePadValues();
}

DirectInputGamePad::~DirectInputGamePad()
{
	if (m_deviceHandle)
	{
		m_deviceHandle->Unacquire();
		m_deviceHandle->Release();
		m_deviceHandle = nullptr;
	}
}

bool DirectInputGamePad::GetGamePadConnected() const
{
	return m_gamePadConnected;
}

void DirectInputGamePad::SetGamePadConnected(bool connected)
{
	m_gamePadConnected = connected;
}

unsigned DirectInputGamePad::GetGamePadID() const 
{
	return m_gamePadID;
}

void DirectInputGamePad::SetTriggerThreshold(const float threshold) 
{
	m_triggerThreshold = threshold;
}

void DirectInputGamePad::Update() 
{
	//Update previous buttons
	UpdatePreviousButtonState();

	if (FAILED(m_deviceHandle->Poll()))
	{
		std::cout << "Lost Device" << std::endl;

		std::cout << "Attempting to Re-Acquire Lost Device..." << std::endl;

		auto result = m_deviceHandle->Acquire();

		while (result == DIERR_INPUTLOST)
		{
			std::cout << "Attempting to Re-Acquire Lost Device..." << std::endl;

			result = m_deviceHandle->Acquire();
		}

		std::cout << "Lost Device Re-Acquired" << std::endl;

		if (result == DIERR_INVALIDPARAM)
		{
			std::cout << "Invalid Device Parameters" << std::endl;
		}

		if (result == DIERR_NOTINITIALIZED)
		{
			std::cout << "Device Not Correctly Initialised" << std::endl;
		}

		if (result == DIERR_OTHERAPPHASPRIO)
		{
			std::cout << "Another application has control of this Device" << std::endl;
		}

		if (!FAILED(result))
		{
			result = m_deviceHandle->GetDeviceState(sizeof(DIJOYSTATE), &m_gamePadState);

			if (FAILED(result))
			{
				std::cout << "Failed to get DeviceHandle DeviceState" << std::endl;
			}
		}
	}
	else
	{
		if (FAILED(m_deviceHandle->GetDeviceState(sizeof(DIJOYSTATE), &m_gamePadState)))
		{
			std::cout << "Failed to get DeviceHandle DeviceState" << std::endl;
		}
	}

	UpdateCurrentButtonState();
}

void DirectInputGamePad::UpdatePreviousButtonState() 
{
	for (auto previousState = m_previousButtonState.begin(); previousState != m_previousButtonState.end(); ++previousState)
	{
		switch (previousState->first)
		{
		case -1:
			previousState->second = m_gamePadState.rgdwPOV[0] == 0;
			continue;
		case -2:
			previousState->second = m_gamePadState.rgdwPOV[0] == 9000;
			continue;
		case -3:
			previousState->second = m_gamePadState.rgdwPOV[0] == 18000;
			continue;
		case -4:
			previousState->second = m_gamePadState.rgdwPOV[0] == 27000;
			continue;
		default:
			previousState->second = ButtonPressed(previousState->first);
			continue;
		}
	}

	m_previousTriggerState.at(0) = LeftTriggerOutSideThreshold();
	m_previousTriggerState.at(1) = RightTriggerOutSideThreshold();
}

void DirectInputGamePad::UpdateCurrentButtonState() 
{
	for (auto currentState = m_currentButtonState.begin(); currentState != m_currentButtonState.end(); ++currentState)
	{
		switch (currentState->first)
		{
		case -1:
			currentState->second = m_gamePadState.rgdwPOV[0] == 0;
			continue;
		case -2:
			currentState->second = m_gamePadState.rgdwPOV[0] == 9000;
			continue;
		case -3:
			currentState->second = m_gamePadState.rgdwPOV[0] == 18000;
			continue;
		case -4:
			currentState->second = m_gamePadState.rgdwPOV[0] == 27000;
			continue;
		default:
			currentState->second = ButtonPressed(currentState->first);
			continue;
		}
	}

	m_currentTriggerState.at(0) = LeftTriggerOutSideThreshold();
	m_currentTriggerState.at(1) = RightTriggerOutSideThreshold();
}

bool DirectInputGamePad::ButtonPressed(const int button) const 
{
	return (m_gamePadState.rgbButtons[button] & 0x80) != 0;
}

//Returns true if the left stick is outside the deadzone
bool DirectInputGamePad::LeftStickOutSideDeadZone() const
{
	const float lX = m_gamePadState.lX - 32768.0f;
	const float lY = m_gamePadState.lY - 32768.0f;

	if (lX > 7849.0f || lX < -7849.0f)
	{
		return true;
	}

	if (lY > 7849.0f || lY < -7849.0f)
	{
		return true;
	}

	return false;
}

//Returns true if the right stick is outside the deadzone
bool DirectInputGamePad::RightStickOutSideDeadZone() const
{
	const float rX = m_gamePadState.lRx - 32768.0f;
	const float rY = m_gamePadState.lRy - 32768.0f;

	if (rX > 8689.0f || rX < -8689.0f)
	{
		return true;
	}

	if (rY > 8689.0f || rY < -8689.0f)
	{
		return true;
	}

	return false;
}

float DirectInputGamePad::LeftStickXPosition() const 
{
	if (LeftStickOutSideDeadZone())
	{
		return (m_gamePadState.lX - 32768.0f) / 32768.0f;
	}

	return 0.0f;
}

float DirectInputGamePad::LeftStickYPosition() const 
{
	if (LeftStickOutSideDeadZone())
	{
		return -((m_gamePadState.lY - 32768.0f) / 32768.0f);
	}

	return 0.0f;
}

float DirectInputGamePad::RightStickXPosition() const 
{
	if (RightStickOutSideDeadZone())
	{
		return (m_gamePadState.lRx - 32768.0f) / 32768.0f;
	}

	return 0.0f;
}

float DirectInputGamePad::RightStickYPosition() const 
{
	if (RightStickOutSideDeadZone())
	{
		return -((m_gamePadState.lRy - 32768.0f) / 32768.0f);
	}

	return 0.0f;
}

ButtonState DirectInputGamePad::QueryTriggerState(const int trigger) const
{
	if (m_previousButtonState.find(trigger)->second)
	{
		if (m_currentButtonState.find(trigger)->second)
		{
			return ButtonState::Held;
		}

		return ButtonState::Released;
	}

	if (m_currentButtonState.find(trigger)->second)
	{
		return ButtonState::Pressed;
	}

	return ButtonState::Released;
}

ButtonState DirectInputGamePad::QueryButtonState(const int button) const 
{
	if (m_previousButtonState.find(button)->second)
	{
		if (m_currentButtonState.find(button)->second)
		{
			return ButtonState::Held;
		}

		return ButtonState::Released;
	}

	if (m_currentButtonState.find(button)->second)
	{
		return ButtonState::Pressed;
	}

	return ButtonState::Released;
}

bool DirectInputGamePad::LeftTriggerOutSideThreshold()
{
	return m_gamePadState.lZ < 32768.0f - m_triggerThreshold;
}

bool DirectInputGamePad::RightTriggerOutSideThreshold()
{
	return m_gamePadState.lZ > 32768.0f + m_triggerThreshold;
}

void DirectInputGamePad::ResetGamePadValues()
{
	m_currentButtonState.clear();
	m_previousButtonState.clear();

	m_currentButtonState.insert(std::make_pair(-1, false));
	m_currentButtonState.insert(std::make_pair(-2, false));
	m_currentButtonState.insert(std::make_pair(-3, false));
	m_currentButtonState.insert(std::make_pair(-4, false));
	m_currentButtonState.insert(std::make_pair(7, false));
	m_currentButtonState.insert(std::make_pair(6, false));
	m_currentButtonState.insert(std::make_pair(8, false));
	m_currentButtonState.insert(std::make_pair(9, false));
	m_currentButtonState.insert(std::make_pair(4, false));
	m_currentButtonState.insert(std::make_pair(5, false));
	m_currentButtonState.insert(std::make_pair(0, false));
	m_currentButtonState.insert(std::make_pair(1, false));
	m_currentButtonState.insert(std::make_pair(2, false));
	m_currentButtonState.insert(std::make_pair(3, false));

	m_previousButtonState.insert(std::make_pair(-1, false));
	m_previousButtonState.insert(std::make_pair(-2, false));
	m_previousButtonState.insert(std::make_pair(-3, false));
	m_previousButtonState.insert(std::make_pair(-4, false));
	m_previousButtonState.insert(std::make_pair(7, false));
	m_previousButtonState.insert(std::make_pair(6, false));
	m_previousButtonState.insert(std::make_pair(8, false));
	m_previousButtonState.insert(std::make_pair(9, false));
	m_previousButtonState.insert(std::make_pair(4, false));
	m_previousButtonState.insert(std::make_pair(5, false));
	m_previousButtonState.insert(std::make_pair(0, true));
	m_previousButtonState.insert(std::make_pair(1, false));
	m_previousButtonState.insert(std::make_pair(2, false));
	m_previousButtonState.insert(std::make_pair(3, false));

	m_currentTriggerState.clear();
	m_previousTriggerState.clear();

	m_currentTriggerState.insert(std::make_pair(0, false));
	m_currentTriggerState.insert(std::make_pair(1, false));

	m_previousTriggerState.insert(std::make_pair(0, false));
	m_previousTriggerState.insert(std::make_pair(1, false));
}