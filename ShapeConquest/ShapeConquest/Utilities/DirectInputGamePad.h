#include "InputEnums.h"
#include <dinput.h>
#include <iostream>
#include <map>

#pragma once
class DirectInputGamePad
{
public:
	DirectInputGamePad(const HWND hwnd, unsigned int gamePadID, LPDIRECTINPUTDEVICE8 deviceHandle);
	~DirectInputGamePad();

	bool GetGamePadConnected() const;
	void SetGamePadConnected(bool connected);

	unsigned int GetGamePadID() const;

	void SetTriggerThreshold(const float threshold);

	void Update();

	float LeftStickXPosition() const;
	float LeftStickYPosition() const;
	float RightStickXPosition() const;
	float RightStickYPosition() const;

	ButtonState QueryTriggerState(const int trigger) const;

	ButtonState QueryButtonState(const int button) const;

private:
	bool LeftTriggerOutSideThreshold();
	bool RightTriggerOutSideThreshold();

	void UpdatePreviousButtonState();
	void UpdateCurrentButtonState();

	bool ButtonPressed(const int button) const;

	bool LeftStickOutSideDeadZone() const;
	bool RightStickOutSideDeadZone() const;

	void ResetGamePadValues();

private:
	bool m_gamePadConnected;

	unsigned int m_gamePadID;
	LPDIRECTINPUTDEVICE8 m_deviceHandle;

	float m_triggerThreshold;

	std::map<const int, bool> m_currentTriggerState;
	std::map<const int, bool> m_previousTriggerState;

	std::map<int, bool> m_currentButtonState;
	std::map<int, bool> m_previousButtonState;

	DIJOYSTATE m_gamePadState;
};

