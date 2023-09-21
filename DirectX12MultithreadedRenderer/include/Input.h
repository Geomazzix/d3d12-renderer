#pragma once
#include <string>
#include "EventSystem/EventArgs.h"
#include "EventSystem/Keycode.h"
#include <unordered_map>

namespace mtd3d
{
	/// <summary>
	/// Serves as a monostate class, containing all the input states of the engine.
	/// </summary>
	class Input
	{
	public:
		Input();
		~Input();

		bool IsPressed(KeyCode key);
		float GetMouseX() const;
		float GetMouseY() const;
		float GetMouseDeltaX() const;
		float GetMouseDeltaY() const;

		void OnKeyboardStateChanged(KeyEventArgs& e);
		void OnMouseMoveChanged(MouseMotionEventArgs& e);
		void OnMouseButtonChanged(MouseButtonEventArgs& e);
		void OnMouseScrollWheelChanged(MouseWheelEventArgs& e);

	private:
		float m_MouseX;
		float m_MouseY;
		float m_MouseDeltaX;
		float m_MouseDeltaY;

		std::unordered_map<KeyCode, bool> m_ButtonMap;
	};
}