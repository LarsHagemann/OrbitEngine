#pragma once
#include <Eigen/Dense>

namespace orbit
{

    using namespace Eigen;

    // @class: Interface for windows. 
    //  Must be implemented by the operating-system dependant backend.
    //  Implements various functionality that must be supported by any window.
    // @template WindowHandle: The OS window handle type that is used
    //
    // @inv: !(m_minimized && m_maximized)            
    //  -- window must not be minimized and maximized at the same time
    // @inv: m_active => m_open
    //  -- a window must not be active but closed
    template<typename WindowHandle>
    class IWindowBase
	{
	protected:
        // @member: OS-specific window handle
        WindowHandle m_handle;
        // @member: stores the dimensions of the window
		Vector2i m_dimensions = { 0, 0 };
        // @member: true if the window is focused at the moment. 
		bool m_active = false;
        // @member: true if the window is minimized.
		bool m_minimized = false;
        // @member: true if the window is maximized.
		bool m_maximized = false;
        // @member: true if the window is open.
		bool m_open = false;
        // @member: true if the window is in fullscreen mode at the moment.
		bool m_fullscreen = false;
        // @member: true if the window is currently resizing
        bool m_resizing;
        // @member: true if a reload of the window is necessary
        bool m_resizeNecessary;
	public:
		virtual ~IWindowBase() {}

        // The window handle type that this window uses
        using window_handle = WindowHandle;
    public:
        // @return: the window's handle
        virtual window_handle GetHandle() const { return m_handle; }
        // @return: The dimensions of the window
		virtual Vector2i GetDimensions() const { return m_dimensions; }
        // @return: true if the window is open, false otherwise
		virtual bool IsOpen() const { return m_open; }
        // @return: true if the window is active (in focus), false otherwise
		virtual bool IsActive() const { return m_active; }
        // @return: true if the window is minimized, false otherwise
		virtual bool IsMinimized() const { return m_minimized; }
        // @return: true if the window is maximized (a maximized window doesn't mean that it has to be in fullscreen mode @see IsFullscreen()), 
        //  false otherwise
		virtual bool IsMaximized() const { return m_maximized; }
        // @return: true if the window is in fullscreen mode, false otherwise
		virtual bool IsFullscreen() const { return m_fullscreen; }

        // @method: Closes this window. This must also invalidate the window handle
		virtual void Close() = 0;
        // @method: Toggles the fullscreen mode of the window
        // @param fullscreen: true will enable fullscreen mode, false will restore the window
        //  to its usual size
		virtual void ToggleFullscreen(bool fullscreen = true) = 0;
        // @method: Handles OS-specific events. Must be called once per frame.
		virtual void HandleEvents() = 0;

        virtual bool IsResizeNeccessary() const { return m_resizeNecessary; }
		
		virtual void Resize(Vector2i size) { m_resizeNecessary = false; }
	};

}