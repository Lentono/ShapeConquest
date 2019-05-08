#include <iostream>
#include <Windows.h>
#include <minwinbase.h>
#include <Xinput.h>
#include <dinput.h>
#include "../Utilities/XInputGamePad.h"
#include "../Utilities/DirectInputGamePad.h"
#include <vector>
#include <glm\glm.hpp>
#include <functional>

#pragma comment (lib, "xinput.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

enum class InputType {
	XInput,
	DirectInput
};

enum class Button {
	DPadUp,
	DPadDown,
	DPadLeft,
	DPadRight,
	Start,
	Back,
	LeftThumb,
	RightThumb,
	LeftShoulder,
	RightShoulder,
	A,
	B,
	X,
	Y
};

enum class Trigger {
	Left,
	Right
};

#pragma once
class InputManager
{
public:
	~InputManager();

	static void RegisterCreatePlayerFunction(const std::function<void(const int)>& createPlayerFunction);
	static void RegisterDestroyPlayerFunction(const std::function<void(const int)>& destroyPlayerFunction);

	static void SetWindowHandle(const HWND hwnd);

	static void InitializeXInputGamePads();

	static bool InitializeDirectInputGamePads();

	static void Update();

	static bool ActivateMotorVibration(const int playerID, const float leftMotor, const float rightMotor);
	static bool DeactivateMotorVibration(const int playerID);

	static void SetTriggerThreshold(const float threshold);

	static ButtonState QueryTrigger(const int playerID, const Trigger trigger);
	static ButtonState QueryButton(const int playerID, const Button button);

	static float QueryLeftStickX(const int playerID);
	static float QueryLeftStickY(const int playerID);
	static float QueryRightStickX(const int playerID);
	static float QueryRightStickY(const int playerID);
	static glm::vec2 QueryLeftStickPosition(const int playerID);
	static glm::vec2 QueryRightStickPosition(const int playerID);

private:
	InputManager();

	static void DestroyRegisteredFunctions();

	static void ConstructXInputButtonMap();
	static void ConstructDirectInputButtonMap();

	static BOOL CALLBACK EnumerateGameControllers(LPCDIDEVICEINSTANCE devInst, LPVOID pvRef);

	static bool CheckXInputGamePadStates();

	static void SetXInputTriggerThreshold(const float threshold);
	static void SetDirectInputTriggerThreshold(const float threshold);

	static ButtonState GetXInputTriggerState(const int playerID, const Trigger trigger);
	static ButtonState GetDirectInputTriggerState(const int playerID, const Trigger trigger);

	static ButtonState GetXInputButtonState(const int playerID, const WORD button);
	static ButtonState GetDirectInputButtonState(const int playerID, const int button);

	static float GetXInputLeftStickXPosition(const int playerID);
	static float GetXInputLeftStickYPosition(const int playerID);
	static float GetXInputRightStickXPosition(const int playerID);
	static float GetXInputRightStickYPosition(const int playerID);
	static glm::vec2 GetXInputLeftStickPosition(const int playerID);
	static glm::vec2 GetXInputRightStickPosition(const int playerID);

	static float GetDirectInputLeftStickXPosition(const int playerID);
	static float GetDirectInputLeftStickYPosition(const int playerID);
	static float GetDirectInputRightStickXPosition(const int playerID);
	static float GetDirectInputRightStickYPosition(const int playerID);
	static glm::vec2 GetDirectInputLeftStickPosition(const int playerID);
	static glm::vec2 GetDirectInputRightStickPosition(const int playerID);

private:
	static HWND m_hwnd;

	static InputType m_gamepadInputType;
	static std::map<Button, const WORD> m_XInputButtonMap;
	static std::map<Button, const int> m_DirectInputButtonMap;

	static IDirectInput8* m_DirectInputDevice;

	static std::vector<std::unique_ptr<XInputGamePad>> m_XInputGamePads;
	static std::vector<std::unique_ptr<DirectInputGamePad>> m_DirectInputGamePads;

	static std::function<void(const int)> m_createPlayerFunction;
	static std::function<void(const int)> m_destroyPlayerFunction;
};

