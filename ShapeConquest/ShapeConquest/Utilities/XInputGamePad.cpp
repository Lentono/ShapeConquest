#include "XInputGamePad.h"

XInputGamePad::XInputGamePad(unsigned int gamePadID) : m_gamePadConnected(false), m_gamePadID(gamePadID), m_gamePadState()
{
	m_triggerThreshold = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	ResetGamePadValues();
}

XInputGamePad::~XInputGamePad()
{
}

bool XInputGamePad::GetGamePadConnected() const
{
	return m_gamePadConnected;
}

void XInputGamePad::SetGamePadConnected(bool connected)
{
	m_gamePadConnected = connected;

	if (m_gamePadConnected)
	{
		ResetGamePadValues();
	}
}

unsigned XInputGamePad::GetGamePadID() const
{
	return m_gamePadID;
}

void XInputGamePad::SetTriggerThreshold(const float threshold)
{
	m_triggerThreshold = threshold * 255.0f;
}

void XInputGamePad::Update()
{
	if (m_gamePadConnected)
	{
		UpdatePreviousButtonState();
		XInputGetState(m_gamePadID, &m_gamePadState);
		UpdateCurrentButtonState();
	}
}

//Returns true if the left stick is outside the deadzone
bool XInputGamePad::LeftStickOutSideDeadZone() const
{
	const float lX = m_gamePadState.Gamepad.sThumbLX;
	const float lY = m_gamePadState.Gamepad.sThumbLY;

	if (lX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || lX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}

	if (lY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || lY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		return true;
	}

	return false;
}

//Returns true if the right stick is outside the deadzone
bool XInputGamePad::RightStickOutSideDeadZone() const
{
	const float rX = m_gamePadState.Gamepad.sThumbRX;
	const float rY = m_gamePadState.Gamepad.sThumbRY;

	if (rX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || rX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return true;
	}

	if (rY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || rY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		return true;
	}

	return false;
}

float XInputGamePad::LeftStickXPosition() const
{
	if (LeftStickOutSideDeadZone())
	{
		return m_gamePadState.Gamepad.sThumbLX / 32768.0f;
	}

	return 0.0f;
}

float XInputGamePad::LeftStickYPosition() const
{
	if (LeftStickOutSideDeadZone())
	{
		return m_gamePadState.Gamepad.sThumbLY / 32768.0f;
	}

	return 0.0f;
}

float XInputGamePad::RightStickXPosition() const
{
	if (RightStickOutSideDeadZone())
	{
		return m_gamePadState.Gamepad.sThumbRX / 32768.0f;
	}

	return 0.0f;
}

float XInputGamePad::RightStickYPosition() const
{
	if (RightStickOutSideDeadZone())
	{
		return m_gamePadState.Gamepad.sThumbRY / 32768.0f;
	}

	return 0.0f;
}

float XInputGamePad::LeftTrigger() const
{
	return m_gamePadState.Gamepad.bLeftTrigger;
}

float XInputGamePad::RightTrigger() const
{
	return m_gamePadState.Gamepad.bRightTrigger;
}

//Value between 0.0f and 1.0f only
void XInputGamePad::ActivateMotorVibration(const float leftMotor, const float rightMotor) const
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = leftMotor * 65535.0f;
	vibration.wRightMotorSpeed = rightMotor * 65535.0f;
	XInputSetState(m_gamePadID, &vibration);
}

void XInputGamePad::DeactivateMotorVibration() const
{
	ActivateMotorVibration(0.0f, 0.0f);
}

ButtonState XInputGamePad::QueryTriggerState(const int trigger) const
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

ButtonState XInputGamePad::QueryButtonState(const WORD button) const
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

bool XInputGamePad::LeftTriggerOutSideThreshold() const
{
	return m_gamePadState.Gamepad.bLeftTrigger > m_triggerThreshold;
}

bool XInputGamePad::RightTriggerOutSideThreshold() const
{
	return m_gamePadState.Gamepad.bRightTrigger > m_triggerThreshold;
}

void XInputGamePad::UpdatePreviousButtonState()
{
	for (auto previousState = m_previousButtonState.begin(); previousState != m_previousButtonState.end(); ++previousState)
	{
		previousState->second = ButtonPressed(previousState->first);
	}

	m_previousTriggerState.at(0) = LeftTriggerOutSideThreshold();
	m_previousTriggerState.at(1) = RightTriggerOutSideThreshold();
}

void XInputGamePad::UpdateCurrentButtonState()
{
	for(auto currentState = m_currentButtonState.begin(); currentState != m_currentButtonState.end(); ++currentState)
	{
		currentState->second = ButtonPressed(currentState->first);
	}

	m_currentTriggerState.at(0) = LeftTriggerOutSideThreshold();
	m_currentTriggerState.at(1) = RightTriggerOutSideThreshold();
}

bool XInputGamePad::ButtonPressed(const WORD button) const
{
	return (m_gamePadState.Gamepad.wButtons & button) != 0;
}

void XInputGamePad::ResetGamePadValues()
{
	ActivateMotorVibration(0.0f, 0.0f);

	m_currentButtonState.clear();
	m_previousButtonState.clear();

	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_UP, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_DOWN, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_LEFT, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_RIGHT, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_START, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_BACK, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_LEFT_THUMB, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_RIGHT_THUMB, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_LEFT_SHOULDER, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_RIGHT_SHOULDER, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_A, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_B, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_X, false));
	m_currentButtonState.insert(std::make_pair(XINPUT_GAMEPAD_Y, false));

	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_UP, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_DOWN, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_LEFT, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_DPAD_RIGHT, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_START, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_BACK, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_LEFT_THUMB, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_RIGHT_THUMB, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_LEFT_SHOULDER, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_RIGHT_SHOULDER, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_A, true));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_B, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_X, false));
	m_previousButtonState.insert(std::make_pair(XINPUT_GAMEPAD_Y, false));

	m_currentTriggerState.clear();
	m_previousTriggerState.clear();

	m_currentTriggerState.insert(std::make_pair(0, false));
	m_currentTriggerState.insert(std::make_pair(1, false));

	m_previousTriggerState.insert(std::make_pair(0, false));
	m_previousTriggerState.insert(std::make_pair(1, false));
}
