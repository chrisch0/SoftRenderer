#include "renderer.h"
#include <windowsx.h>
#include <cstring>
#include <limits>
#include <iostream>
#include <thread>
#include "omp.h"
#include "shader_functions.h"

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Renderer::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

Renderer* Renderer::m_app = nullptr;

Renderer::Renderer()
	: m_camera(float3(1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), 1.0)
{
	assert(m_app == nullptr);
	
	m_app = this;
}

Renderer::Renderer(int width, int height) :
	m_clientWidth(width), m_clientHeight(height), m_camera(float3(1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), (float)width / (float)height)
{
	assert(m_app == nullptr);
	m_app = this;
}

Renderer::~Renderer()
{
	m_scene->Release();
}

Renderer* Renderer::GetApp()
{
	return m_app;
}

bool Renderer::Initialize(Scene* scene)
{
	if (!InitMainWindow())
		return false;

	m_scene = scene;

	omp_set_num_threads(std::thread::hardware_concurrency());

	return true;
}

void Renderer::MainLoop()
{
	MSG msg = { 0 };

	m_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();
			CalculateFrameStats();
			if (!m_appPaused)
			{
				Update();
				Render();
				HDC window_dc = GetDC(m_hMainWnd);
				BitBlt(window_dc, 0, 0, m_clientWidth, m_clientHeight, m_memoryDC, 0, 0, SRCCOPY);
				ReleaseDC(m_hMainWnd, window_dc);
			}
			else
			{
				Sleep(100);
			}
		}
	}
}

bool Renderer::InitMainWindow()
{
	ATOM class_atom;
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Register Class Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_clientWidth, m_clientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"MainWnd", m_mainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, GetModuleHandle(NULL), 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"Create Window Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

HDC Renderer::CreateFrameBuffer()
{
	BITMAPINFOHEADER bitmap_header;
	HDC window_dc;
	HDC memory_dc;
	HBITMAP dib_bitmap;
	HBITMAP old_bitmap;
	unsigned char *buffer = nullptr;

	DeleteObject(m_curBitmap);
	DeleteDC(m_memoryDC);

	window_dc = GetDC(m_hMainWnd);
	memory_dc = CreateCompatibleDC(window_dc);
	ReleaseDC(m_hMainWnd, window_dc);

	std::memset(&bitmap_header, 0, sizeof(BITMAPINFOHEADER));
	bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_header.biWidth = m_clientWidth;
	bitmap_header.biHeight = -m_clientHeight;
	bitmap_header.biPlanes = 1;
	bitmap_header.biBitCount = 32;
	//bitmap_header.biCompression = BI_RGB;
	dib_bitmap = CreateDIBSection(memory_dc, (BITMAPINFO*)&bitmap_header,
		DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
	assert(dib_bitmap != NULL);
	old_bitmap = (HBITMAP)SelectObject(memory_dc, dib_bitmap);
	DeleteObject(old_bitmap);
	m_curBitmap = dib_bitmap;

	if (m_frameBuffer == nullptr)
		m_frameBuffer = new FrameBuffer((int)m_clientWidth, (int)m_clientHeight);
	else
	{
		m_frameBuffer->SetWidth(m_clientWidth);
		m_frameBuffer->SetHeight(m_clientHeight);
	}
	m_frameBuffer->SetBuffer(buffer);

	return memory_dc;
}

void Renderer::InitScene()
{
	m_scene->InitScene(m_frameBuffer, m_camera);
}

void Renderer::Update()
{
	float4 delta_pos = float4(m_io.DeltaMousePos.x / m_clientWidth, m_io.DeltaMousePos.y / m_clientHeight, m_io.DeltaMousePos.z / m_clientWidth, m_io.DeltaMousePos.w / m_clientHeight);
	m_camera.Update(delta_pos * 0.5f, m_io.DeltaScroll);
	m_scene->Update(m_timer, m_io, m_camera);
}

void Renderer::Render()
{
	m_context.SetViewport(&m_viewport);
	m_scene->Draw(m_context);

	m_io.DeltaScroll = 0.0f;
	m_io.DeltaMousePos = float4(0.f, 0.f, 0.f, 0.f);
}

LRESULT Renderer::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			m_clientHeight = HIWORD(lParam);
			m_clientWidth = LOWORD(lParam);
			OnResize();
		}
		return 0;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPaused = true;

			m_timer.Stop();
		}
		else
		{
			m_appPaused = false;
			m_timer.Start();
		}
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		OnKeyUp(wParam);
		return 0;
	case WM_LBUTTONDOWN:
		OnMouseDown(BUTTON_L, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
		OnMouseUp(BUTTON_L, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONDOWN:
		OnMouseDown(BUTTON_R, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		OnMouseUp(BUTTON_R, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		float offset = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		OnMouseScroll(offset);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Renderer::OnResize()
{
	DeleteDC(m_memoryDC);
	m_memoryDC = CreateFrameBuffer();
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = (float)m_clientWidth;
	m_viewport.Height = (float)m_clientHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_camera.SetAspect((float)m_clientWidth / (float)m_clientHeight);
}

void Renderer::OnKeyDown(WPARAM key)
{

}

void Renderer::OnKeyUp(WPARAM key)
{

}

void Renderer::OnMouseDown(button_t button, int x, int y)
{
	m_io.ClickMousePos.x = x;
	m_io.ClickMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void Renderer::OnMouseUp(button_t button, int x, int y)
{
	ReleaseCapture();
}

void Renderer::OnMouseScroll(float scroll)
{
	m_io.DeltaScroll += scroll;
}

void Renderer::OnMouseMove(WPARAM btnState, int x, int y)
{
	//m_deltaMousePos = float4(0.0f);
	if ((btnState & MK_LBUTTON) != 0)
	{
		m_io.DeltaMousePos.x += (x - m_io.CurMousePos.x);
		m_io.DeltaMousePos.y += (y - m_io.CurMousePos.y);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		m_io.DeltaMousePos.z += (x - m_io.CurMousePos.x);
		m_io.DeltaMousePos.w += (y - m_io.CurMousePos.y);
	}
			
	m_io.CurMousePos.x = x;
	m_io.CurMousePos.y = y;
}

void Renderer::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.f;

	frameCnt++;

	if ((m_timer.TotalTime() - timeElapsed) >= 1.f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = m_mainWndCaption +
			L"	 fps: " + fpsStr +
			L"	mspf: " + mspfStr;

		SetWindowTextW(m_hMainWnd, windowText.c_str());

		frameCnt = 0;
		timeElapsed += 1.f;
	}
}