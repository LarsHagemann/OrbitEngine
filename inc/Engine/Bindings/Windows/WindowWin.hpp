#pragma once
#include "Engine/Rendering/WindowBase.hpp"
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Misc/Logger.hpp"
// @brief: this file abstracts a Win32 window instance

namespace orbit
{

	class Window : public WindowBase
	{
	protected:
		// @method: static window proc handler
		// @brief: the callback function for the window handle
		//	this function will internally call 
		//	Engine::window_callback(...)
		// @internal
		static LRESULT CALLBACK static_window_callback(HWND, UINT, WPARAM, LPARAM);
		// @brief: the callback function for the window handle
		// @internal
		LRESULT CALLBACK window_callback(UINT, WPARAM, LPARAM);
		// @internal
		bool _scheduleClose;
		// @internal (for recovering from fullscreen)
		RECT _windowRect;
	public:
		// @constructor: creates a new Win32 Window
		// @param dimensions: dimensions of the window
		// @param title: window title
		Window(Vector2i dimensions, std::wstring_view title);
		// @method: creates a shared pointer to a window
		// @param dimensions: dimensions of the window
		// @param title: window title
		static std::shared_ptr<Window> Create(Vector2i dimensions, std::wstring_view title);
		// @method: closes the window
		virtual void Close() override;
		// @method: toggles the fullscreen mode
		// @param fullscreen: true to enter fullscreen, false to leave fullscreen
		virtual void SetFullscreen(bool fullscreen = true) override;
		// @method: translates and dispatches window messages
		// @internal
		virtual void HandleEvents() override;
	};

}
