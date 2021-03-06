#include <memory>
using namespace std;

#define NOMINMAX

#include <wrl.h>
#include <atlbase.h>
#include <atlwin.h>

#define ASSERT ATLASSERT
#define VERIFY ATLVERIFY
#define TRACE ATLTRACE

#include <d2d1.h>
#pragma comment(lib, "d2d1")
using namespace D2D1;
using namespace Microsoft::WRL;

#ifdef DEBUG
#define HR(expression) ASSERT(S_OK == (expression))
#else
struct ComException
{
	HRESULT const hr;
	ComException(HRESULT const value) : hr(value) {}
};

inline void HR(HRESULT hr)
{
	if (S_OK != hr) throw ComException(hr);
}
#endif

#include "Scene.h"
#include "CircleRenderer.h"
#include "Engine.h"
#include "SquareRenderer.h"

template <typename T>
class DesktopWindow : public  CWindowImpl<T, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW | WS_VISIBLE>> {
public:
	ComPtr<ID2D1Factory> _factory;
	ComPtr<ID2D1HwndRenderTarget> _target;

	DECLARE_WND_CLASS_EX(L"window", CS_HREDRAW | CS_VREDRAW, -1);

	BEGIN_MSG_MAP(SampleWindow)
		MESSAGE_HANDLER(WM_PAINT, PaintHandler)
		MESSAGE_HANDLER(WM_DESTROY, DestroyHandler)
		MESSAGE_HANDLER(WM_SIZE, SizeHandler)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, DisplayChangeHandler)
	END_MSG_MAP()

	LRESULT DisplayChangeHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		Invalidate();
		return 0;
	}

	LRESULT SizeHandler(UINT, WPARAM, LPARAM lparam, BOOL&)
	{
		if (_target)
		{
			if (S_OK != _target->Resize(SizeU(LOWORD(lparam), HIWORD(lparam))))
			{
				_target.Reset();
			}
		}

		return 0;
	}

	LRESULT PaintHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		PAINTSTRUCT ps;
		VERIFY(BeginPaint(&ps));

		Render();

		EndPaint(&ps);
		return 0;
	}

	LRESULT DestroyHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		PostQuitMessage(0);
		return 0;
	}

	void Invalidate()
	{
		VERIFY(InvalidateRect(nullptr, false));
	}

	void Create()
	{
		D2D1_FACTORY_OPTIONS fo = {};

#ifdef DEBUG
		fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, _factory.GetAddressOf()));

		CreateDeviceIndependentResources();

		VERIFY(__super::Create(nullptr, 0, L"C SCI 151"));
	}

	void CreateDeviceIndependentResources()
	{
	}

	virtual void CreateDeviceResources() = 0;

	void Render()
	{
		// create render target if needed
		if (!_target)
		{
			RECT rect;
			VERIFY(GetClientRect(&rect));

			auto size = SizeU(rect.right, rect.bottom);

			HR(_factory->CreateHwndRenderTarget(RenderTargetProperties(),
				HwndRenderTargetProperties(m_hWnd, size),
				_target.GetAddressOf()));

			CreateDeviceResources();
		}

		// draw the scene
		if (!(D2D1_WINDOW_STATE_OCCLUDED & _target->CheckWindowState()))
		{
			_target->BeginDraw();

			Draw();

			if (D2DERR_RECREATE_TARGET == _target->EndDraw())
			{
				_target.Reset();
			}
		}
	}

	virtual void Update(double deltaTime) = 0;
	virtual void Draw() = 0;

	int processFrame(double deltaTime)
	{
		Update(deltaTime);

		PAINTSTRUCT ps;
		VERIFY(BeginPaint(&ps));

		Render();

		EndPaint(&ps);
		return 0;
	}

	int Run()
	{
		const int FRAMES_PER_SECOND = 60;
		bool finished = false;
		MSG message;
		while (!finished) {
			DWORD currentTick = GetTickCount();
			DWORD endTick = currentTick + 1000 / FRAMES_PER_SECOND;

			while (currentTick < endTick) {
				if (PeekMessage(&message, m_hWnd, 0, 0, PM_REMOVE)) {
					if (WM_QUIT == message.message) {
						finished = true;
						break;
					}
					else {
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
					currentTick = GetTickCount();
				}
				else {
					break;
				}
			}

			processFrame(1000 / FRAMES_PER_SECOND);
		}

		return 0;
	}
};

class SampleWindow : public DesktopWindow<SampleWindow> {
public:
	SampleWindow()
		: _scene{}, _engine{}
	{
		// initialize device resources
		auto brushDeviceResource = make_shared<BrushDeviceResource>();
		_engine.AddDeviceResource(static_pointer_cast<IDeviceResource, BrushDeviceResource>(brushDeviceResource));

		// initialize renderers

		// TODO: add more renderers here

		// initialize three different color circle 
		auto yellowCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 0.f, 1.f }, 50.f);
		auto whiteCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 1.f, 1.f }, 40.f);
		auto greenCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 0.f, 1.f, 0.f, 1.f }, 60.f);
		//initialize the squares
		auto yellowSquareRenderer = make_shared<SquareRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 0.f, 1.f }, 100.f);
		auto whiteSquareRenderer = make_shared<SquareRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 1.f, 1.f }, 100.f);
		auto greenSquareRenderer = make_shared<SquareRenderer>(brushDeviceResource, Color4F{ 0.f, 1.f, 0.f, 1.f }, 100.f);
		// Initialize three differnt size circlce 
		auto smallyellowCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 0.f, 1.f }, 45.f);
		auto smallwhiteCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 1.f, 1.f }, 40.f);
		auto smallgreenCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 0.f, 1.f, 0.f, 1.f }, 35.f);
		auto smallyellowSquareRenderer = make_shared<SquareRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 0.f, 1.f }, 50.f);
		
		
		

		// setup scene


		//1
		auto gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(yellowCircleRenderer), MPoint2F{ 200.f, 400.f });
		_scene.insertGameObject(gameObject);
		//2
		 gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(whiteCircleRenderer), MPoint2F{ 150.f, 300.f });
		_scene.insertGameObject(gameObject);
		//3
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(greenCircleRenderer), MPoint2F{ 150.f, 300.f });
		_scene.insertGameObject(gameObject);

		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(smallgreenCircleRenderer), MPoint2F{ 600.f, 450.f });
		_scene.insertGameObject(gameObject);
		//4
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, SquareRenderer>(whiteSquareRenderer), MPoint2F{ 400.f, 450.f });
		_scene.insertGameObject(gameObject);

		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(smallyellowCircleRenderer), MPoint2F{ 200.f, 150.f });
		_scene.insertGameObject(gameObject);
		//5
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(smallwhiteCircleRenderer), MPoint2F{ 300.f, 300.f });
		_scene.insertGameObject(gameObject);
		//6
	
		//7
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, SquareRenderer>(yellowSquareRenderer), MPoint2F{ 400.f, 150.f });
		_scene.insertGameObject(gameObject);
		//8
		
		//9
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, SquareRenderer>(greenSquareRenderer), MPoint2F{ 400.f, 300.f });
		_scene.insertGameObject(gameObject);
		//10
		gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(smallgreenCircleRenderer), MPoint2F{ 600.f, 450.f });
		_scene.insertGameObject(gameObject);
		
		

		// TODO: add more objects to the scene

	}

	virtual void CreateDeviceResources() override
	{
		_engine.ResetDeviceResource(*_target.Get());
	}

	virtual void Draw() override
	{
		_target->Clear(D2D_COLOR_F{ 0.26f, 0.56f, 0.87f, 1.0f });

		auto size = _target->GetSize();

		_scene.Draw(*_target.Get());
	}

	virtual void Update(double deltaTime) override
	{
		_scene.Update(deltaTime);
	}

private:
	Scene _scene;
	Engine _engine;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	SampleWindow window;
	
	window.Create();

	return window.Run();
}