#include <Windows.h>
#include <iostream>
#include <map>
#include "InputEnums.h"
#include <Xinput.h>
#pragma comment (lib, "xinput.lib")

#pragma once
class XInputGamePad
{
public:
	XInputGamePad(unsigned int gamePadID);
	~XInputGamePad();

	bool GetGamePadConnected() const;
	void SetGamePadConnected(bool connected);

	unsigned int GetGamePadID() const;

	void SetTriggerThreshold(const float threshold);

	void Update();

	bool LeftStickOutSideDeadZone() const;
	bool RightStickOutSideDeadZone() const;

	float LeftStickXPosition() const;
	float LeftStickYPosition() const;
	float RightStickXPosition() const;
	float RightStickYPosition() const;

	void ActivateMotorVibration(const float leftMotor, const float rightMotor) const;
	void DeactivateMotorVibration() const;

	float LeftTrigger() const;
	float RightTrigger() const;

	ButtonState QueryTriggerState(const int trigger) const;

	ButtonState QueryButtonState(const WORD button) const;

private:
	bool LeftTriggerOutSideThreshold() const;
	bool RightTriggerOutSideThreshold() const;

	void UpdatePreviousButtonState();
	void UpdateCurrentButtonState();

	bool ButtonPressed(const WORD button) const;

	void ResetGamePadValues();

private:
	bool m_gamePadConnected;
	unsigned int m_gamePadID;

	byte m_triggerThreshold;

	std::map<const int, bool> m_currentTriggerState;
	std::map<const int, bool> m_previousTriggerState;

	std::map<WORD, bool> m_currentButtonState;
	std::map<WORD, bool> m_previousButtonState;

	XINPUT_STATE m_gamePadState;
};

