#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
#include <string>
#include "Input.h"
#include <memory>

namespace mtd3d
{
	/// <summary>
	/// Represents a WinAPI window.
	/// Input can be retrieved using GetKeyState method
	/// </summary>
	class Window
	{
	public:
		Window();
		~Window() = default;

		void Initialize(HINSTANCE appModule, std::weak_ptr<Input> input, const std::wstring& title, int windowWidth, int windowHeight);
		void Destroy();

		void PollEvents();

		HWND GetWindowHandle() const;
		unsigned int GetWindowWidth() const;
		unsigned int GetWindowHeight() const;
		bool DoesWinAPIWindowClose() const;

		void Show();
		void Hide();

		void UpdateWindowText();

	protected:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK LocalWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		std::weak_ptr<Input> m_Input;

		bool m_Minimized;
		bool m_Maximized;
		bool m_Resizing;
		bool m_WinAPICloseWindow;
		unsigned int m_WindowWidth;
		unsigned int m_WindowHeight;
		HWND m_WindowInstance;
		std::wstring m_Title;
		const std::wstring m_WindowClassName;
	};
}