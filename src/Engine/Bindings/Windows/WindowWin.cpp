#include "Engine/Bindings/Windows/WindowWin.hpp"

#ifdef ORBIT_WITH_IMGUI
#include "backends/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace orbit
{
	LRESULT Window::static_window_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
#ifdef ORBIT_WITH_IMGUI
#ifdef ORBIT_WIN
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
#endif
#endif
		Window* self = nullptr;
		if (msg == WM_NCCREATE) {
			// Setup the windows long ptr which will be used to
			// get a 'this' pointer
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			self = static_cast<Window*>(lpcs->lpCreateParams);
			self->_handle = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(self)
			);
		}
		else {
			// try to get the 'this' pointer
			self = reinterpret_cast<Window*>(
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

	Window::Window(Vector2i dimensions, std::wstring_view title) :
		_scheduleClose(false)
	{
		ORBIT_INFO_LEVEL(FormatString("Initializing window."), 5);

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

		_dimensions = dimensions;

		RECT rc = { 0, 0, static_cast<LONG>(_dimensions.x()), static_cast<LONG>(_dimensions.y()) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		_handle = ::CreateWindowExW(
			0L,
			L"OrbitWindowClass",
			title.data(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			_dimensions.x(),
			_dimensions.y(),
			nullptr,
			nullptr,
			nullptr,
			this
		);

		if (!_handle)
		{
			auto le = GetLastError();
			ORBIT_ERR(FormatString(
				"GetLastError: %d; %s\n",
				le,
				std::system_category().message(le).c_str()
			)
			);
			ORBIT_THROW("Failed to create the render window");
		}
		
		_open = true;
		_active = true;
		ShowWindow(_handle, SW_SHOW);
	}

	LRESULT Window::window_callback(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// WM_ACTIVATE is sent when the window is activated or deactivated.  
			// We pause the game when the window is deactivated and unpause it 
			// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				_active = false;
				//_clock.Pause();
			}
			else
			{
				_active = true;
				//_clock.Unpause();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			_dimensions.x() = LOWORD(lParam);
			_dimensions.y() = HIWORD(lParam);
			//if (_device)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					_active = false;
					_minimized = true;
					_maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					_active = true;
					_minimized = false;
					_maximized = true;
					_resizeNecessary = true;
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (_minimized)
					{
						_active = true;
						_minimized = false;
						_resizeNecessary = true;
					}

					// Restoring from maximized state?
					else if (_maximized)
					{
						_active = true;
						_maximized = false;
						_resizeNecessary = true;
					}
					else if (_resizing)
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
						_resizeNecessary = true;
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			_active = false;
			_resizing = true;
			//_clock.Pause();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			_active = true;
			_resizing = false;
			//_clock.Unpause();
			_resizeNecessary = true;
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			_open = false;
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
		}

		return DefWindowProc(_handle, msg, wParam, lParam);
	}

	std::shared_ptr<Window> Window::Create(Vector2i dimensions, std::wstring_view title)
	{
		return std::make_shared<Window>(dimensions, title);
	}

	void Window::Close()
	{
		_scheduleClose = true;
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		if (_fullscreen == fullscreen) return;

		_fullscreen = fullscreen;

		
	}

	void Window::HandleEvents()
	{
		if (_scheduleClose)
			SendMessage(_handle, WM_DESTROY, 0, 0);

		MSG _message;
		while (PeekMessage(&_message, _handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&_message);
			DispatchMessage(&_message);
		}
	}

}