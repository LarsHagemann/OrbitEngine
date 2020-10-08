#pragma once
#include "engine_initialization.hpp"
#include "resource_manager.hpp"
#include "helper.hpp"
#include "object.hpp"
#include "scene.hpp"

namespace orbit
{

	struct EngineState
	{
		// @member: the id of the currently bound pipeline state object
		std::string _currentPipelineState;
		// @member: the current index into the backbuffer array
		unsigned _currentBackbuffer;
		// @member: the next fence value to be used
		uint64_t _nextFenceValue;
		// @member: current fence values of the different frames
		std::vector<uint64_t> _fences;
		// @member: feature level for direct X
		D3D_FEATURE_LEVEL _featureLevel;
		// @member: driver type of the direct X device
		D3D_DRIVER_TYPE _driverType;
		// @member: number of backbuffers for the swap chain
		unsigned _numBackbuffers;
		// @member: size of a single RTV descriptor
		unsigned _RTVDescriptorSize;
		// @member: is the window currently active (focused and not-minimized)
		bool _active;
		// @member: is the window currently open
		bool _open;
		// @member: stores the screen dimensions
		Vector2i _dimensions;
		// @member: used for fullscreen toggling
		RECT _hWndRect;
		// @member: clock for the elapsed frame time
		//	(time elapsed between two frames)
		Clock _clock;
		// @member: clear color for the window
		Vector4f _clearColor;
		// @member: is v-sync currently enabled?
		bool _vSync;
		// @member: determines whether tearing is supported by the 
		//	display and the GPU
		bool _tearingSupported;
		// @member: is the window currently in fullscreen mode?
		bool _fullscreen;
		// @member: is the window currently minimized?
		bool _minimized;
		// @member: is the window currently maximized?
		bool _maximized;
		// @member: is the window currently being resized?
		bool _resizing;
	};

	// @brief: this class is the heart piece of the Orbit Game Engine
	class Engine : public ResourceManager
	{
	protected:
		// @member: the DirectX 12 rendering device
		Ptr<ID3D12Device6> _device;
#ifdef _DEBUG
		Ptr<ID3D12Debug> _debug;
#endif
		// @member: backbuffers for the swap chain
		std::vector<Ptr<ID3D12Resource1>> _backbuffers;
		// @member: command allocators for the command lists
		std::vector<Ptr<ID3D12CommandAllocator>> _commandAllocators;
		// @member: the DirectX 12 command list
		Ptr<ID3D12CommandQueue> _cmdQueue;
		// @member: TMP; the graphics command list to be used
		Ptr<ID3D12GraphicsCommandList> _cmdList;
		// @member: descriptor heap for Render Target Views
		Ptr<ID3D12DescriptorHeap> _RTVDescriptorHeap;
		// @member: the DXGI swap chain
		Ptr<IDXGISwapChain4> _swapChain;
		// @member: the syncronization fence
		Ptr<ID3D12Fence1> _fence;
		// @member: the window handle
		HWND _hWnd;
		// @member: direct input device
		Ptr<IDirectInput8W> _directInput;
		// @member: helper struct for state management of the engine
		EngineState _state;
		// @member: scene to be rendered
		std::shared_ptr<Scene> _scene;
	protected:
		// @internal
		void UpdateRenderTargetViews();
		// @method: called whenever the window is being resized
		// @brief: resizes internal buffers to the new window size
		void OnResize();
		// @method: updates and draws the current scene
		void UpdateAndDraw();
		// @method: clears the screen to the specified clear color
		void Clear();
		// @method: presents the new back buffer
		void Display();
		// @method: static window proc handler
		// @brief: the callback function for the window handle
		//	this function will internally call 
		//	Engine::window_callback(...)
		// @internal
		static LRESULT CALLBACK static_window_callback(HWND, UINT, WPARAM, LPARAM);
		// @brief: the callback function for the window handle
		// @internal
		LRESULT CALLBACK window_callback(UINT, WPARAM, LPARAM);
	public:
		// @brief: constructor. It's recommended to use Engine::Create(...) instead
		Engine(EngineDesc* desc);
		// @brief: engine desctructor
		~Engine();
		// @method: returns a shared pointer to an initialized engine object
		// @param desc: descriptor for the engine. See engine_initialization.hpp for more
		// @return: initialized engine object
		static std::shared_ptr<Engine> Create(EngineDesc* desc);
		// @method: returns the DX12 Device
		Ptr<ID3D12Device> GetDevice() const override { return _device; }
		// @method: returns the WIN32 window handle
		// @return: the window handle of the window created by the engine
		HWND GetHandle() const { return _hWnd; }
		// @method: returns the DX input device
		// @return: DX Input device
		Ptr<IDirectInput8W> GetInputDevice() const { return _directInput; }
		// @method: binds a pipeline state to the pipeline
		// @param id: id of tht pipeline state to be bound
		void BindPipelineState(const std::string& id);
		// @method: sets the scene to be rendered
		// @param scene: the scene to be rendered during the next frame
		void SetScene(std::shared_ptr<Scene> scene);
		// @method: signals the command queue with the current fence value
		// @return: the fence value for the current Signal
		uint64_t Signal();
		// @method: stalls the CPU until a certain fence value has been reached
		// @param value: fence value to wait for
		void WaitForFenceValue(uint64_t value);
		// @method: flushes the command queue. Use this function wisely it
		//	will stall the CPU in the worst case
		void Flush();
		// @method: runs the game engine's update loop
		void Run();
		// @method: toggles the window's fullscreen mode
		void SetFullscreen(bool fullscreen);
	};

}
