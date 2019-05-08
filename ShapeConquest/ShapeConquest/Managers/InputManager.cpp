#include "InputManager.h"
HWND InputManager::m_hwnd;
InputType InputManager::m_gamepadInputType;
std::map<Button, const WORD> InputManager::m_XInputButtonMap;
std::map<Button, const int> InputManager::m_DirectInputButtonMap;
IDirectInput8* InputManager::m_DirectInputDevice;
std::vector<std::unique_ptr<XInputGamePad>> InputManager::m_XInputGamePads;
std::vector<std::unique_ptr<DirectInputGamePad>> InputManager::m_DirectInputGamePads;
std::function<void(const int)> InputManager::m_createPlayerFunction;
std::function<void(const int)> InputManager::m_destroyPlayerFunction;

InputManager::~InputManager()
{

}

void InputManager::RegisterCreatePlayerFunction(const std::function<void(const int)>& createPlayerFunction)
{
	m_createPlayerFunction = createPlayerFunction;
}

void InputManager::RegisterDestroyPlayerFunction(const std::function<void(const int)>& destroyPlayerFunction)
{
	m_destroyPlayerFunction = destroyPlayerFunction;
}

void InputManager::DestroyRegisteredFunctions()
{
	m_createPlayerFunction = nullptr;
	m_destroyPlayerFunction = nullptr;
}


void InputManager::SetWindowHandle(const HWND hwnd)
{
	m_hwnd = hwnd;
}

void InputManager::InitializeXInputGamePads()
{
	//Just some dispose logic if DirectInput has already been initialised
	if (m_DirectInputDevice)
	{
		DestroyRegisteredFunctions();
		m_DirectInputDevice->Release();
		m_DirectInputDevice = nullptr;

		if (!m_DirectInputGamePads.empty())
		{
			m_DirectInputGamePads.clear();
		}
	}

	ConstructXInputButtonMap();
	//std::cout << "Initialising XInput GamePads" << std::endl;

	if (!(m_XInputGamePads.size() == XUSER_MAX_COUNT))
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			m_XInputGamePads.emplace_back(std::make_unique<XInputGamePad>(i));
		}

		std::cout << "XInput GamePads initialised." << std::endl;
	}
	else
	{
		std::cout << "XInput GamePads already initialised." << std::endl;
	}

	m_gamepadInputType = InputType::XInput;

	std::cout << "InputType set to XInput mode." << std::endl;
}


BOOL CALLBACK InputManager::EnumerateGameControllers(LPCDIDEVICEINSTANCE devInst, LPVOID pvRef)
{
	auto* inputHandlerInstance = (InputManager*)pvRef;

	LPDIRECTINPUTDEVICE8 gamepad;

	if (!FAILED(m_DirectInputDevice->CreateDevice(devInst->guidInstance, &gamepad, nullptr)))
	{
		m_DirectInputGamePads.emplace_back(std::make_unique<DirectInputGamePad>(m_hwnd, m_DirectInputGamePads.size(), gamepad));
	}
	else
	{
		auto test = false;
		//Fug we failed boys
	}

	return true;
}

bool InputManager::InitializeDirectInputGamePads()
{
	ConstructDirectInputButtonMap();

	if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInputDevice, nullptr)))
	{
		auto test = false;
		//Fug we failed boys
	}

	if (FAILED(m_DirectInputDevice->EnumDevices(DI8DEVCLASS_GAMECTRL, &EnumerateGameControllers, nullptr, DIEDFL_ATTACHEDONLY))) 
	{
		auto test = false;
		//Fug we failed boys
	}

	m_gamepadInputType = InputType::DirectInput;

	std::cout << "InputType set to DirectInput mode." << std::endl;

	return false;
}

void InputManager::Update()
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		if (CheckXInputGamePadStates())
		{
			for (auto& gamePad : m_XInputGamePads)
			{
				gamePad->Update();
			}
		}
		else
		{
			//std::cout << "No XInput controllers detected..." << std::endl;
		}
		break;
	case InputType::DirectInput:

		//Manage HotSwapping
		for (auto& gamePad : m_DirectInputGamePads)
		{
			//Query button state (start)
			const auto buttonState = gamePad->QueryButtonState(7);

			//Check if button is pressed or held
			if (buttonState == ButtonState::Pressed)
			{
				//If gamepad is not connected to a player, we initialise and connect one to them
				if (!gamePad->GetGamePadConnected())
				{
					m_createPlayerFunction(gamePad->GetGamePadID());

					gamePad->SetGamePadConnected(true);
				}
				else //Else we disconnect them from the player and delete it
				{
					m_destroyPlayerFunction(gamePad->GetGamePadID());

					gamePad->SetGamePadConnected(false);
				}
			}
		}


		for (auto& gamePad : m_DirectInputGamePads)
		{
			gamePad->Update();
		}
	}
}

InputManager::InputManager()
{

}

void InputManager::ConstructXInputButtonMap()
{
	//Construct XInput Button Maps
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::DPadUp, 0x0001));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::DPadDown, 0x0002));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::DPadLeft, 0x0004));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::DPadRight, 0x0008));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::Start, 0x0010));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::Back, 0x0020));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::LeftThumb, 0x0040));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::RightThumb, 0x0080));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::LeftShoulder, 0x0100));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::RightShoulder, 0x0200));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::A, 0x1000));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::B, 0x2000));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::X, 0x4000));
	m_XInputButtonMap.emplace(std::make_pair<Button, const WORD>(Button::Y, 0x8000));
}

void InputManager::ConstructDirectInputButtonMap()
{
	//Construct DirectInput Button Maps
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::DPadUp, -1));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::DPadRight, -2));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::DPadDown, -3));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::DPadLeft, -4));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::Start, 7));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::Back, 6));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::LeftThumb, 8));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::RightThumb, 9));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::LeftShoulder, 4));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::RightShoulder, 5));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::A, 0));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::B, 1));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::X, 2));
	m_DirectInputButtonMap.emplace(std::make_pair<Button, const int>(Button::Y, 3));
}

//Checks if any controllers are connected or not
//Returns true if any number of gamepads are connected, else false
bool InputManager::CheckXInputGamePadStates()
{
	bool gamepadsConnected = false;
	DWORD dwResult;

	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE inputState;
		ZeroMemory(&inputState, sizeof(XINPUT_STATE));

		//std::cout << "Checking Controller " << i << " state..." << std::endl;

		dwResult = XInputGetState(i, &inputState);

		XInputGamePad* gamePad = m_XInputGamePads[i].get();

		if (dwResult == ERROR_SUCCESS)
		{
			gamepadsConnected = true;
			//std::cout << "Controller " << i << " is connected." << std::endl;

			if (!gamePad->GetGamePadConnected())
			{
				//std::cout << "Activating GamePad..." << std::endl;
				gamePad->SetGamePadConnected(true);
			}
		}
		else
		{
			//std::cout << "Controller " << i << " is not connected." << std::endl;

			if (gamePad->GetGamePadConnected())
			{
				//std::cout << "Deactivating GamePad..." << std::endl;
				gamePad->SetGamePadConnected(false);
			}
		}

		gamePad = nullptr;
	}

	return gamepadsConnected;
}

//Returns true if the motor vibration has been set for a controller successfully, else it returns false
//Only supported for XInput
bool InputManager::ActivateMotorVibration(const int playerID, const float leftMotor, const float rightMotor)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		for (auto& gamepad : m_XInputGamePads)
		{
			if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
			{
				gamepad->ActivateMotorVibration(leftMotor, rightMotor);
				return true;
			}
		}

		return false;

	case InputType::DirectInput:
		std::cout << "DirectInput does not support motor vibration." << std::endl;
		return false;

	default:
		return false;
	}
}

//Returns true if motor vibration has been deactivated successfully, else false.
//Only supported for XInput
bool InputManager::DeactivateMotorVibration(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		for (auto& gamepad : m_XInputGamePads)
		{
			if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
			{
				gamepad->DeactivateMotorVibration();
				return true;
			}
		}

		return false;

	case InputType::DirectInput:
		std::cout << "DirectInput does not support motor vibration." << std::endl;
		return false;

	default:
		return false;
	}
}

//Send value between 0.0f, 1.0f
void InputManager::SetTriggerThreshold(const float threshold)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		SetXInputTriggerThreshold(threshold);
		return;

	case InputType::DirectInput:
		SetDirectInputTriggerThreshold(threshold);
		return;

	default:
		return;
	}
}

void InputManager::SetXInputTriggerThreshold(const float threshold)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		gamepad->SetTriggerThreshold(threshold);
	}
}

void InputManager::SetDirectInputTriggerThreshold(const float threshold)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		gamepad->SetTriggerThreshold(threshold);
	}
}

ButtonState InputManager::QueryTrigger(const int playerID, const Trigger trigger)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputTriggerState(playerID, trigger);

	case InputType::DirectInput:
		return GetDirectInputTriggerState(playerID, trigger);

	default:
		return ButtonState::Invalid;
	}
}

ButtonState InputManager::GetXInputTriggerState(const int playerID, const Trigger trigger)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			switch (trigger)
			{
			case Trigger::Left:
				return gamepad->QueryTriggerState(0);

			case Trigger::Right:
				return gamepad->QueryTriggerState(1);

			default:
				return ButtonState::Invalid;
			}
		}
	}

	return ButtonState::Invalid;
}

ButtonState InputManager::GetDirectInputTriggerState(const int playerID, const Trigger trigger)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			switch (trigger)
			{
			case Trigger::Left:
				return gamepad->QueryTriggerState(0);

			case Trigger::Right:
				return gamepad->QueryTriggerState(1);

			default:
				return ButtonState::Invalid;
			}
		}
	}

	return ButtonState::Invalid;
}

ButtonState InputManager::QueryButton(const int playerID, const Button button)
{
	switch (m_gamepadInputType)
	{
		case InputType::XInput: 
			return GetXInputButtonState(playerID, m_XInputButtonMap.at(button));

		case InputType::DirectInput:
			return GetDirectInputButtonState(playerID, m_DirectInputButtonMap.at(button));

		default:
			return ButtonState::Invalid;
	}
}

ButtonState InputManager::GetXInputButtonState(const int playerID, const WORD button)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->QueryButtonState(button);
		}
	}

	return ButtonState::Invalid;
}

ButtonState InputManager::GetDirectInputButtonState(const int playerID, const int button)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->QueryButtonState(button);
		}
	}

	return ButtonState::Invalid;
}

float InputManager::QueryLeftStickX(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputLeftStickXPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputLeftStickXPosition(playerID);
	default:
		return 0.0f;
	}
}

float InputManager::QueryLeftStickY(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputLeftStickYPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputLeftStickYPosition(playerID);
	default:
		return 0.0f;
	}
}

float InputManager::QueryRightStickX(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputRightStickXPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputRightStickXPosition(playerID);
	default:
		return 0.0f;
	}
}

float InputManager::QueryRightStickY(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputRightStickYPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputRightStickYPosition(playerID);
	default:
		return 0.0f;
	}
}

glm::vec2 InputManager::QueryLeftStickPosition(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputLeftStickPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputLeftStickPosition(playerID);
	default:
		return glm::vec2();
	}
}

glm::vec2 InputManager::QueryRightStickPosition(const int playerID)
{
	switch (m_gamepadInputType)
	{
	case InputType::XInput:
		return GetXInputRightStickPosition(playerID);
	case InputType::DirectInput:
		return GetDirectInputRightStickPosition(playerID);
	default:
		return glm::vec2();
	}
}

float InputManager::GetXInputLeftStickXPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->LeftStickXPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetXInputLeftStickYPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->LeftStickYPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetXInputRightStickXPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->RightStickXPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetXInputRightStickYPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->RightStickYPosition();
		}
	}

	return 0.0f;
}

glm::vec2 InputManager::GetXInputLeftStickPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return glm::vec2(gamepad->LeftStickXPosition(), gamepad->LeftStickYPosition());
		}
	}

	return glm::vec2();
}

glm::vec2 InputManager::GetXInputRightStickPosition(const int playerID)
{
	for (auto& gamepad : m_XInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return glm::vec2(gamepad->RightStickXPosition(), gamepad->RightStickYPosition());
		}
	}

	return glm::vec2();
}

float InputManager::GetDirectInputLeftStickXPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->LeftStickXPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetDirectInputLeftStickYPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->LeftStickYPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetDirectInputRightStickXPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->RightStickXPosition();
		}
	}

	return 0.0f;
}

float InputManager::GetDirectInputRightStickYPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return gamepad->RightStickYPosition();
		}
	}

	return 0.0f;
}

glm::vec2 InputManager::GetDirectInputLeftStickPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return glm::vec2(gamepad->LeftStickXPosition(), gamepad->LeftStickYPosition());
		}
	}

	return glm::vec2();
}

glm::vec2 InputManager::GetDirectInputRightStickPosition(const int playerID)
{
	for (auto& gamepad : m_DirectInputGamePads)
	{
		if (playerID == gamepad->GetGamePadID() && gamepad->GetGamePadConnected())
		{
			return glm::vec2(gamepad->RightStickXPosition(), gamepad->RightStickYPosition());
		}
	}

	return glm::vec2();
}
