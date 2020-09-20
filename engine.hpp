#pragma once
#include "resource_manager.hpp"
#include "helper.hpp"
#include "object.hpp"

namespace orbit
{

	// @brief: this class is the heart piece of the Orbit Game Engine

	class Engine : public ResourceManager
	{
	protected:
		// @member: the DirectX 12 rendering device
		Ptr<ID3D12Device> _device;
#ifdef _DEBUG
		Ptr<ID3D12Debug> _debug;
#endif
		// @member: the DirectX 12 command list
		Ptr<ID3D12CommandQueue> _cmdList;
		// @member: the DXGI swap chain
		Ptr<IDXGISwapChain> _swapChain;
		// @member: the window handle
		HWND _hWnd;
		// @member: clear color for the window
		Vector4f _clearColor;
		// @member: direct input device
		Ptr<IDirectInput8W> _directInput;
		// @member: is the window currently active (focused and not-minimized)
		bool _active;
		// @member: is the window currently open
		bool _open;
		// @member: stores the screen dimensions
		Vector2i _dimensions;
		// @member: time elapsed since the last frame
		float _dt;
		// @member: feature level for direct X
		D3D_FEATURE_LEVEL _featureLevel;
		// @member: driver type of the direct X device
		D3D_DRIVER_TYPE _driverType;
	protected:
		// @method: called whenever the window is being resized
		// @brief: resizes internal buffers to the new window size
		void OnResize();
		// @method: updates and draws the current scene
		void UpdateAndDraw();
		// @method: event handling for the scene
		void HandleEvents();
		// @method: clears the screen to the specified clear color
		void Clear();
		// @method: presents the new back buffer
		void Display();
	public:
		Ptr<ID3D12Device> GetDevice() const { return _device; }
	};

}
