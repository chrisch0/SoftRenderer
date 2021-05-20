#include "renderer.h"
#include <windowsx.h>
#include <cstring>

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return Renderer::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

Renderer* Renderer::m_app = nullptr;

Renderer::Renderer()
{
	assert(m_app == nullptr);
	m_app = this;
}

Renderer::Renderer(int width, int height) :
	m_clientWidth(width), m_clientHeight(height)
{
	assert(m_app == nullptr);
	m_app = this;
}

Renderer::~Renderer()
{

}

Renderer* Renderer::GetApp()
{
	return m_app;
}

bool Renderer::Initialize(VertexShader vs, PixelShader ps)
{
	if (!InitMainWindow())
		return false;
	m_memoryDC = CreateFrameBuffer();

	m_pipelineState.vertexShader = vs;
	m_pipelineState.pixelShader = ps;

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
			if (!m_appPaused)
			{
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
	unsigned char *buffer;

	window_dc = GetDC(m_hMainWnd);
	memory_dc = CreateCompatibleDC(window_dc);
	ReleaseDC(m_hMainWnd, window_dc);

	std::memset(&bitmap_header, 0, sizeof(BITMAPINFOHEADER));
	bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_header.biWidth = m_clientWidth;
	bitmap_header.biHeight = -m_clientHeight;
	bitmap_header.biPlanes = 1;
	bitmap_header.biBitCount = 32;
	dib_bitmap = CreateDIBSection(memory_dc, (BITMAPINFO*)&bitmap_header,
		DIB_RGB_COLORS, (void**)&buffer, NULL, 0);
	assert(dib_bitmap != NULL);
	old_bitmap = (HBITMAP)SelectObject(memory_dc, dib_bitmap);
	DeleteObject(old_bitmap);

	m_frameBuffer = new FrameBuffer((int)m_clientWidth, (int)m_clientHeight);
	m_frameBuffer->SetBuffer(buffer);

	return memory_dc;
}

void Renderer::InitScene()
{
	Vertex v;
	v.position = vec3f(-1.f, -1.f, 0.f);
	v.normal = vec3f(0.f, 0.f, -1.f);
	v.uv = vec2f(0.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = vec3f(1.f, -1.f, 0.f);
	v.uv = vec2f(1.f, 1.f);
	m_vertexBuffer.push_back(v);
	v.position = vec3f(-1.f, 1.f, 0.f);
	v.uv = vec2f(0.f, 0.f);
	m_vertexBuffer.push_back(v);
	v.position = vec3f(1.f, 1.f, 0.f);
	v.uv = vec2f(1.f, 0.f);
	m_vertexBuffer.push_back(v);
	m_indexBuffer.push_back(0);
	m_indexBuffer.push_back(1);
	m_indexBuffer.push_back(2);
	m_indexBuffer.push_back(1);
	m_indexBuffer.push_back(3);
	m_indexBuffer.push_back(2);
}

void Renderer::Update(const Timer& timer)
{

}

void Renderer::Render(const Timer& timer)
{
	int num_faces = m_indexBuffer.size() / 3;
	for (int i = 0; i < num_faces; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			VSInput* vs_input = m_vertexBuffer.data() + m_indexBuffer[i * 3 + j];
			inVertexAttri[j] = m_pipelineState.vertexShader(vs_input, nullptr);
		}

		// TODO: triangle clipping
		for (int j = 0; j < 3; ++j)
		{
			outVertexAttri[j] = inVertexAttri[j];
		}

		// rasterize triangle

		// perspective division
		static vec3f ndc_coords[3];
		for (int j = 0; j < 3; ++j)
		{
			outVertexAttri[j].sv_position = outVertexAttri[j].sv_position / outVertexAttri[j].sv_position.w;
			//ndc_coords[j] = outVertexAttri[j].sv_position;
		}

		// TODO: build bounding box

		//for (int x = 0; x < )

	}
}

LRESULT Renderer::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
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
	case WM_MOUSEWHEEL:
		float offset = GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
		OnMouseScroll(offset);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Renderer::OnKeyDown(WPARAM key)
{

}

void Renderer::OnKeyUp(WPARAM key)
{

}

void Renderer::OnMouseDown(button_t button, int x, int y)
{

}

void Renderer::OnMouseUp(button_t button, int x, int y)
{

}

void Renderer::OnMouseScroll(float scroll)
{

}

void Renderer::OnMouseMove(WPARAM btnState, int x, int y)
{

}