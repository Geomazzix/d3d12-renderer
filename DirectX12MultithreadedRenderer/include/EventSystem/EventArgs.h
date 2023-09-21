#pragma once
#include "EventSystem/KeyCode.h"

namespace mtd3d
{
	struct EventArgs
	{
	public:
		EventArgs() = default;
		virtual ~EventArgs() = default;
	};
	struct KeyEventArgs : public EventArgs
	{
	public:
		enum class KeyState
		{
			Released = 0,
			Pressed = 1
		};

		typedef EventArgs base;
		KeyEventArgs(mtd3d::KeyCode key, unsigned int c, KeyState state, bool control, bool shift, bool alt)
			: Key(key)
			, Char(c)
			, State(state)
			, Control(control)
			, Shift(shift)
			, Alt(alt)
		{}

		mtd3d::KeyCode   Key;    // The Key Code that was pressed or released.
		unsigned int    Char;   // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
		KeyState        State;  // Was the key pressed or released?
		bool            Control;// Is the Control modifier pressed
		bool            Shift;  // Is the Shift modifier pressed
		bool            Alt;    // Is the Alt modifier pressed
	};

	class MouseMotionEventArgs : public EventArgs
	{
	public:
		typedef EventArgs base;
		MouseMotionEventArgs(int x, int y) :
			X(x),
			Y(y)
		{}

		int X;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int Y;              // The Y-position of the cursor relative to the upper-left corner of the client area.
		int RelX{};           // How far the mouse moved since the last event.
		int RelY{};           // How far the mouse moved since the last event.

	};

	class MouseButtonEventArgs : public EventArgs
	{
	public:
		enum class MouseButton
		{
			None = 0,
			Left = 1,
			Right = 2,
			Middle = 3
		};
		enum class ButtonState
		{
			Released = 0,
			Pressed = 1
		};

		typedef EventArgs base;
		MouseButtonEventArgs(MouseButton buttonID, ButtonState state)
			: Button(buttonID)
			, State(state)
		{}

		MouseButton Button; // The mouse button that was pressed or released.
		ButtonState State;  // Was the button pressed or released?
	};

	class MouseWheelEventArgs : public EventArgs
	{
	public:
		typedef EventArgs base;
		MouseWheelEventArgs(float wheelDelta)
			: WheelDelta(wheelDelta)
		{}

		float WheelDelta;   // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.

	};

	class ResizeEventArgs : public EventArgs
	{
	public:
		typedef EventArgs base;
		ResizeEventArgs(int width, int height)
			: Width(width)
			, Height(height)
		{}

		// The new width of the window
		int Width;
		// The new height of the window.
		int Height;

	};

	class UserEventArgs : public EventArgs
	{
	public:
		typedef EventArgs base;
		UserEventArgs(int code, void* data1, void* data2)
			: Code(code)
			, Data1(data1)
			, Data2(data2)
		{}

		int     Code;
		void* Data1;
		void* Data2;
	};
}