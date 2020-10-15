#pragma once
#include "engine.hpp"

#ifdef ORBIT_WITH_IMGUI
#include "imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace orbit
{

	LRESULT Engine::static_window_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
#ifdef ORBIT_WITH_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;
#endif

		Engine* self = nullptr;
		if (msg == WM_NCCREATE) {
			// Setup the windows long ptr which will be used to
			// get a 'this' pointer
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
			self = static_cast<Engine*>(lpcs->lpCreateParams);
			self->_hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(self)
			);
		}
		else {
			// try to get the 'this' pointer
			self = reinterpret_cast<Engine*>(
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

	LRESULT Engine::window_callback(UINT msg, WPARAM wParam, LPARAM lParam)
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
				_clock.Pause();
			}
			else
			{
				_active = true;
				_clock.Unpause();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			_dimensions.x() = LOWORD(lParam);
			_dimensions.y() = HIWORD(lParam);
			if (_device)
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
					_resizeNeccessary = true;
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (_minimized)
					{
						_active = true;
						_minimized = false;
						_resizeNeccessary = true;
					}

					// Restoring from maximized state?
					else if (_maximized)
					{
						_active = true;
						_maximized = false;
						_resizeNeccessary = true;
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
						_resizeNeccessary = true;
				}
			}
			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			_active = false;
			_resizing = true;
			_clock.Pause();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			_active = true;
			_resizing = false;
			_clock.Unpause();
			_resizeNeccessary = true;
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

		return DefWindowProc(_hWnd, msg, wParam, lParam);
	}

}
