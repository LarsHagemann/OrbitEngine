#ifdef ORBIT_WINDOWS
#include "implementation/backends/Windows/Windows_Window.hpp"
#include "implementation/misc/Logger.hpp"

#undef new

#include <backends/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define WM_ORBIT_FULLSCREEN (WM_USER + 0x0001)

namespace orbit
{
	LRESULT WindowsWindow::static_window_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
		
		WindowsWindow* self = nullptr;
		if (msg == WM_NCCREATE) {
			// Setup the windows long ptr which will be used to
			// get a 'this' pointer
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			self = static_cast<WindowsWindow*>(lpcs->lpCreateParams);
			self->m_handle = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(self)
			);
		}
		else {
			// try to get the 'this' pointer
			self = reinterpret_cast<WindowsWindow*>(
				GetWindowLongPtr(hWnd, GWLP_USERDATA)
				);
		}
		if (self) {
			// set everything up if there is a this pointer
			return self->window_callback(msg, wParam, lParam);
		}
		else
			// Most of the first messages are windows internal and no
			// lParam will be available (self will be nullptr) so we 
			// forward those messages to the default procedur
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(Vector2i dimensions, std::wstring_view title) :
		m_scheduleClose(false)
	{
		ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Initializing window.");

		WNDCLASSEX wndClass;
		ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.lpfnWndProc = static_window_callback;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hInstance = NULL;
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszClassName = NULL;
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpszClassName = L"OrbitWindowClass";

		ORBIT_THROW_IF_FAILED(
			RegisterClassExW(&wndClass),
			"Failed to register the window class"
		);

		m_dimensions = dimensions;

		RECT rc = { 0, 0, static_cast<LONG>(m_dimensions.x()), static_cast<LONG>(m_dimensions.y()) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		m_handle = ::CreateWindowExW(
			0L,
			L"OrbitWindowClass",
			title.data(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			m_dimensions.x(),
			m_dimensions.y(),
			nullptr,
			nullptr,
			nullptr,
			this
		);

		if (!m_handle)
		{
			auto le = GetLastError();
			ORBIT_ERROR("GetLastError: %d; %s\n",
				le,
				std::system_category().message(le).c_str()
			);
			ORBIT_THROW("Failed to create the render window");
		}
		
		m_open = true;
		m_active = true;
		ShowWindow(m_handle, SW_SHOW);
	}

	LRESULT WindowsWindow::window_callback(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// WM_ACTIVATE is sent when the window is activated or deactivated.  
			// We pause the game when the window is deactivated and unpause it 
			// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				m_active = false;
				//_clock.Pause();
			}
			else
			{
				m_active = true;
				//_clock.Unpause();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			m_dimensions.x() = LOWORD(lParam);
			m_dimensions.y() = HIWORD(lParam);
			//if (_device)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					m_active = false;
					m_minimized = true;
					m_maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_active = true;
					m_minimized = false;
					m_maximized = true;
					m_resizeNecessary = true;
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (m_minimized)
					{
						m_active = true;
						m_minimized = false;
						m_resizeNecessary = true;
					}

					// Restoring from maximized state?
					else if (m_maximized)
					{
						m_active = true;
						m_maximized = false;
						m_resizeNecessary = true;
					}
					else if (m_resizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else
						m_resizeNecessary = true;
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			m_active = false;
			m_resizing = true;
			//_clock.Pause();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			m_active = true;
			m_resizing = false;
			//_clock.Unpause();
			m_resizeNecessary = true;
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			m_open = false;
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
		case WM_ORBIT_FULLSCREEN:
			m_resizeNecessary = true;
			if (m_fullscreen)
			{
				// Toggle fullscreen on
				::GetWindowRect(m_handle, &m_windowRect);

				UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
				::SetWindowLongPtrW(m_handle, GWL_STYLE, windowStyle);

				HMONITOR hMonitor = ::MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST);
				MONITORINFOEX monitorInfo = {};
				monitorInfo.cbSize = sizeof(MONITORINFOEX);
				::GetMonitorInfo(hMonitor, &monitorInfo);

				::SetWindowPos(
					m_handle,
					HWND_TOP,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE
				);

				::ShowWindow(m_handle, SW_MAXIMIZE);
			}
			else
			{
				// recover from fullscreen mode
				::SetWindowLong(m_handle, GWL_STYLE, WS_OVERLAPPEDWINDOW);

				::SetWindowPos(
					m_handle,
					HWND_NOTOPMOST,
					m_windowRect.left,
					m_windowRect.top,
					m_windowRect.right - m_windowRect.left,
					m_windowRect.bottom - m_windowRect.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE
				);

				::ShowWindow(m_handle, SW_NORMAL);
			}
			return 0;
		}

		return DefWindowProc(m_handle, msg, wParam, lParam);
	}

	std::shared_ptr<WindowsWindow> WindowsWindow::Create(Vector2i dimensions, std::wstring_view title)
	{
		return std::make_shared<WindowsWindow>(dimensions, title);
	}

	void WindowsWindow::Close()
	{
		m_scheduleClose = true;
	}

	void WindowsWindow::ToggleFullscreen(bool fullscreen)
	{
		if (m_fullscreen == fullscreen) return;

		m_fullscreen = fullscreen;
		// Signal the message handler in the main thread
		// to change into fullscreen
		SendNotifyMessage(m_handle, WM_ORBIT_FULLSCREEN, 0, 0);
	}

	void WindowsWindow::HandleEvents()
	{
		if (m_scheduleClose)
			SendMessage(m_handle, WM_DESTROY, 0, 0);

		MSG message;
		while (PeekMessage(&message, m_handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

}

#endif