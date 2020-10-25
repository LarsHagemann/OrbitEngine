#pragma once
#include "Engine/Bindings/Platform.hpp"
#include "Eigen/Dense"

namespace orbit
{

	using namespace Eigen;

	class WindowBase
	{
	protected:
		friend class Engine;
		window_handle _handle;
		Vector2i _dimensions;
		bool _active;
		bool _minimized;
		bool _maximized;
		bool _resizing;
		bool _resizeNecessary;
		bool _open;
		bool _fullscreen;
	public:
		WindowBase() :
			_open(false),
			_active(false),
			_minimized(false),
			_maximized(false),
			_resizeNecessary(false),
			_resizing(false),
			_dimensions({ 0,0 }),
			_handle(nullptr),
			_fullscreen(false)
		{}

		virtual ~WindowBase() {}
		virtual window_handle GetHandle() const { return _handle; }
		virtual Vector2i GetDimensions() const { return _dimensions; }
		virtual bool IsOpen() const { return _open; }
		virtual bool IsActive() const { return _active; }
		virtual bool IsMinimized() const { return _minimized; }
		virtual bool IsMaximized() const { return _maximized; }
		virtual bool IsFullscreen() const { return _fullscreen; }

		virtual void Close() = 0;
		virtual void SetFullscreen(bool fullscreen = true) = 0;
		virtual void HandleEvents() = 0;

	};

}
