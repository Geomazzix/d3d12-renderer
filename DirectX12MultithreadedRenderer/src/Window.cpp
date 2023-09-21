#include "Window.h"
#include <cassert>

#include "EventSystem/EventArgs.h"
#include "EventSystem/EventMessenger.h"
#include "Utility/Time.h"

namespace mtd3d
{
	Window::Window() :
		m_WinAPICloseWindow(false),
		m_Title(L""),
		m_WindowWidth(0),
		m_WindowHeight(0),
		m_WindowInstance(nullptr),
		m_WindowClassName(L"D3D12ApplicationWindow"),
		m_Minimized(false),
		m_Maximized(false),
		m_Resizing(false)
	{
	}

	void Window::Initialize(HINSTANCE appModule, std::weak_ptr<Input> input, const std::wstring& title, int windowWidth, int windowHeight)
	{
		m_Title = title;
		m_WindowWidth = windowWidth;
		m_WindowHeight = windowHeight;

		WNDCLASSEXW windowClass =
		{
			sizeof(WNDCLASSEXW),
			CS_HREDRAW | CS_VREDRAW,
			&Window::WindowProc,
			0,
			0,
			appModule,
			LoadIcon(appModule, MAKEINTRESOURCE(101)),		//101 = the default window icon
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			m_WindowClassName.c_str(),
			LoadIcon(appModule, MAKEINTRESOURCE(101))		//101 = the default window icon
		};

		static ATOM atom = RegisterClassExW(&windowClass);
		assert(atom > 0);

		RECT windowCanvas = { 0, 0, windowWidth, windowHeight };
		AdjustWindowRect(&windowCanvas, WS_OVERLAPPEDWINDOW, false);
		int width = windowCanvas.right - windowCanvas.left;
		int height = windowCanvas.bottom - windowCanvas.top;
		int windowX = std::max<int>(0, (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2);
		int windowY = std::max<int>(0, (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2);

		m_WindowInstance = CreateWindowExW(
			NULL,
			m_WindowClassName.c_str(),
			m_Title.c_str(),
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			NULL,
			NULL,
			appModule,
			this
		);

		assert(m_WindowInstance && "Failed to create render window!");
	
		m_Input = input;
		Show();
		UpdateWindow(m_WindowInstance);
	}

	void Window::PollEvents()
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				Destroy();
				return;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	HWND Window::GetWindowHandle() const
	{
		return m_WindowInstance;
	}

	unsigned int Window::GetWindowWidth() const
	{
		return m_WindowWidth;
	}

	unsigned int Window::GetWindowHeight() const
	{
		return m_WindowHeight;
	}

	bool Window::DoesWinAPIWindowClose() const
	{
		return m_WinAPICloseWindow;
	}

	void Window::Show()
	{
		ShowWindow(m_WindowInstance, SW_SHOW);
		SetFocus(m_WindowInstance);
	}

	void Window::Hide()
	{
		ShowWindow(m_WindowInstance, SW_HIDE);
	}

	void Window::UpdateWindowText()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		// Compute averages over one second period.
		if ((Time::TotalTime() - timeElapsed) >= 1.0f)
		{
			float fps = (float)frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			std::wstring windowText = m_Title +
				L"    fps: " + fpsStr +
				L"   mspf: " + mspfStr;

			SetWindowText(m_WindowInstance, windowText.c_str());

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}
	}

	void Window::Destroy()
	{
		DestroyWindow(m_WindowInstance);
	}

	LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Window* windowInstance = NULL;

		switch (message)
		{
		case WM_CREATE:
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			windowInstance = (Window*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)windowInstance);
			break;
		}
		default:
			windowInstance = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			break;
		}

		return windowInstance != nullptr 
			? windowInstance->LocalWindowProc(hwnd, message, wParam, lParam) 
			: DefWindowProc(hwnd, message, wParam, lParam);
	}

	LRESULT CALLBACK Window::LocalWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{

		case WM_MOUSEHWHEEL: //Redefined mousewheel points to the wrong windows version, ms bug due to new windows update?
		case 0x020A:
		{
			float wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if(abs(wheelDelta) >= WHEEL_DELTA)
			{
				MouseWheelEventArgs e(wheelDelta);
				m_Input.lock()->OnMouseScrollWheelChanged(e);
			}
			return 0;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// All painting occurs here, between BeginPaint and EndPaint.
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			EndPaint(hwnd, &ps);

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Left, MouseButtonEventArgs::ButtonState::Pressed);
			m_Input.lock()->OnMouseButtonChanged(e);
			SetCapture(m_WindowInstance);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Middle, MouseButtonEventArgs::ButtonState::Pressed);
			m_Input.lock()->OnMouseButtonChanged(e);
			SetCapture(m_WindowInstance);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Right, MouseButtonEventArgs::ButtonState::Pressed);
			m_Input.lock()->OnMouseButtonChanged(e);
			SetCapture(m_WindowInstance);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Left, MouseButtonEventArgs::ButtonState::Released);
			m_Input.lock()->OnMouseButtonChanged(e);
			ReleaseCapture();
			return 0;
		}
		case WM_MBUTTONUP:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Middle, MouseButtonEventArgs::ButtonState::Released);
			m_Input.lock()->OnMouseButtonChanged(e);
			ReleaseCapture();
			return 0;
		}
		case WM_RBUTTONUP:
		{
			MouseButtonEventArgs e(MouseButtonEventArgs::MouseButton::Right, MouseButtonEventArgs::ButtonState::Released);
			m_Input.lock()->OnMouseButtonChanged(e);
			ReleaseCapture();
			return 0;
		}
		case WM_MOUSEMOVE:
		{	
			MouseMotionEventArgs e(LOWORD(lParam), HIWORD(lParam));
			m_Input.lock()->OnMouseMoveChanged(e);
			return 0;
		}
		case WM_KEYDOWN:
		{
			MSG charMsg;
			// Get the Unicode character (UTF-16)
			unsigned int c = 0;
			// For printable characters, the next message will be WM_CHAR.
			// This message contains the character code we need to send the KeyPressed event.
			// Inspired by the SDL 1.2 implementation.
			if (PeekMessage(&charMsg, m_WindowInstance, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
			{
				GetMessage(&charMsg, m_WindowInstance, 0, 0);
				c = static_cast<unsigned int>(charMsg.wParam);
			}
			KeyCode key = static_cast<KeyCode>(wParam);
			if (key == KeyCode::Escape) m_WinAPICloseWindow = true; //Escape = close application.

			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
			unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
			KeyEventArgs e(key, c, KeyEventArgs::KeyState::Pressed, shift, control, alt);
			m_Input.lock()->OnKeyboardStateChanged(e);
			return 0;
		}
		case WM_KEYUP:
		{
			KeyCode key = (KeyCode)wParam;

			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
			unsigned int c = 0;
			unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

			// Determine which key was released by converting the key code and the scan code
			// to a printable character (if possible).
			// Inspired by the SDL 1.2 implementation.
			unsigned char keyboardState[256];
			if (!GetKeyboardState(keyboardState))
				printf("Unrecognized key!\n");
			wchar_t translatedCharacters[4];
			if (int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
			{
				c = translatedCharacters[0];
			}

			KeyEventArgs e(key, c, KeyEventArgs::KeyState::Released, shift, control, alt);
			m_Input.lock()->OnKeyboardStateChanged(e);
			return 0;
		}
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				EventMessenger::GetInstance().EvokeMessenger("OnAppPause");
			}
			else
			{
				EventMessenger::GetInstance().EvokeMessenger("OnAppResume");
			}
			return 0;
		}

		case WM_SIZE:
		{
			m_WindowWidth = LOWORD(lParam);
			m_WindowHeight = HIWORD(lParam);

			ResizeEventArgs resizeArgs(m_WindowWidth, m_WindowHeight);

			if (wParam == SIZE_MINIMIZED)
			{
				EventMessenger::GetInstance().EvokeMessenger("OnAppPause");
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				EventMessenger::GetInstance().EvokeMessenger("OnAppResume");
				m_Minimized = false;
				m_Maximized = true;
				EventMessenger::GetInstance().EvokeMessenger("OnWindowResize", resizeArgs);
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_Minimized) // Restoring from minimized state?
				{
					EventMessenger::GetInstance().EvokeMessenger("OnAppResume");
					m_Minimized = false;
					EventMessenger::GetInstance().EvokeMessenger("OnWindowResize", resizeArgs);
				}
				else if (m_Maximized) // Restoring from maximized state?
				{
					EventMessenger::GetInstance().EvokeMessenger("OnAppResume");
					m_Maximized = false;
					EventMessenger::GetInstance().EvokeMessenger("OnWindowResize", resizeArgs);
				}
				else if (!m_Resizing)
				{
					EventMessenger::GetInstance().EvokeMessenger("OnWindowResize", resizeArgs);
				}
			}
			return 0;
		}
			
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
		{
			EventMessenger::GetInstance().EvokeMessenger("OnAppPause");
			m_Resizing = true;
			return 0;
		}

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
		{
			EventMessenger::GetInstance().EvokeMessenger("OnAppResume");
			m_Resizing = false;
			ResizeEventArgs resizeArgs(m_WindowWidth, m_WindowHeight);
			EventMessenger::GetInstance().EvokeMessenger("OnWindowResize", resizeArgs);
			return 0;
		}

		// Don't beep when we alt-enter.
		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		//Catch the window destruction flag.
		case WM_DESTROY:
			m_WinAPICloseWindow = true;
			PostQuitMessage(0);
			return 0;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
}