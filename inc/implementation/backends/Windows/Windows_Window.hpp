#pragma once
#ifdef ORBIT_WINDOWS
#include <Windows.h>
#include <string_view>
#include <memory>

#include "interfaces/rendering/WindowBase.hpp"
#include <Eigen/Dense>

namespace orbit
{

    using namespace Eigen;

    // Implementation of a window for Windows machines.
    // @example:
    //  auto window = orb::WindowsWindow::Create({1080, 720}, L"Title");
    //  while (window->IsOpen()) {
    //      window->HandleEvents();
    //      // Render...
    //  }
    class WindowsWindow : public IWindowBase<HWND>
    {
    private:
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
		bool m_scheduleClose;
		// @internal (for recovering from fullscreen)
		RECT m_windowRect;
	public:
		// @constructor: creates a new Win32 Window
		// @param dimensions: dimensions of the window
		// @param title: window title
		WindowsWindow(Vector2i dimensions, std::wstring_view title);
		// @method: creates a shared pointer to a window
		// @param dimensions: dimensions of the window
		// @param title: window title
		static std::shared_ptr<WindowsWindow> Create(Vector2i dimensions, std::wstring_view title);
    public:
        // @method: Closes this window. This must also invalidate the window handle
		virtual void Close() override;
        // @method: Toggles the fullscreen mode of the window
        // @param fullscreen: true will enable fullscreen mode, false will restore the window
        //  to its usual size
		virtual void ToggleFullscreen(bool fullscreen = true) override;
        // @method: Handles OS-specific events. Must be called once per frame.
		virtual void HandleEvents() override;
    };

}
#endif