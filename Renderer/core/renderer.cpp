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

}

Renderer* Renderer::GetApp()
{
	return m_app;
}

bool Renderer::Initialize(VertexShader vs, PixelShader ps)
{
	if (!InitMainWindow())
		return false;
	//m_memoryDC = CreateFrameBuffer();

	m_pipelineState.VS = vs;
	m_pipelineState.PS = ps;
	InitPipelineState();

	omp_set_num_threads(std::thread::hardware_concurrency());

	return true;
}

void Renderer::InitPipelineState()
{
	RasterizerDesc& rs_desc = m_pipelineState.RasterizerState;
	rs_desc.CullMode = Cull_Mode_Back;
	rs_desc.FrontCounterClockWise = true;

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
				Update(m_timer);
				Render(m_timer);
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
	// full screen quad
	/*
	{
		Vertex v;
		v.position = float3(-1.f, -1.f, 0.f);
		v.normal = float3(0.f, 0.f, -1.f);
		v.uv = float2(0.f, 1.f);
		v.color = float4(1.f, 0.f, 0.f, 1.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(1.f, -1.f, 0.f);
		v.uv = float2(1.f, 1.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(-1.f, 1.f, 0.f);
		v.uv = float2(0.f, 0.f);
		m_vertexBuffer.push_back(v);

		v.position = float3(1.f, 1.f, 0.f);
		v.uv = float2(1.f, 0.f);
		v.color = float4(0.f, 0.f, 0.f, 0.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(1.f, -1.f, 0.f);
		v.uv = float2(1.f, 1.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(-1.f, 1.f, 0.f);
		v.uv = float2(0.f, 0.f);
		m_vertexBuffer.push_back(v);

		m_indexBuffer.push_back(0);
		m_indexBuffer.push_back(1);
		m_indexBuffer.push_back(2);
		m_indexBuffer.push_back(4);
		m_indexBuffer.push_back(3);
		m_indexBuffer.push_back(5);
	}
	*/

	// cube

	{
		Vertex v;
		v.position = float3( -0.5f, -0.5f, -0.5f );
		v.normal = float3( 0.0f, 0.0f, -1.0f );
		v.uv = float2( 0.0f, 1.0f );
		v.color = float4( 1.0f, 0.0f, 0.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, +0.5f, -0.5f );
		v.uv = float2( 0.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, -0.5f );
		v.uv = float2( 1.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, -0.5f, -0.5f );
		v.uv = float2( 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);

		v.position = float3( -0.5f, -0.5f, +0.5f );
		v.normal = float3( 0.0f, 0.0f, 1.0f );
		v.uv = float2( 1.0f, 1.0f );
		v.color = float4( 0.0f, 1.0f, 0.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, -0.5f, +0.5f );
		v.uv = float2( 0.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, +0.5f );
		v.uv = float2( 0.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, +0.5f, +0.5f );
		v.uv = float2( 1.0f, 0.0f );
		m_vertexBuffer.push_back(v);

		v.position = float3( -0.5f, +0.5f, -0.5f );
		v.normal = float3( 0.0f, 1.0f, 0.0f );
		v.uv = float2( 0.0f, 1.0f );
		v.color = float4( 0.0f, 0.0f, 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, +0.5f, +0.5f );
		v.uv = float2( 0.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, +0.5f );
		v.uv = float2( 1.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, -0.5f );
		v.uv = float2( 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);

		v.position = float3( -0.5f, -0.5f, -0.5f );
		v.normal = float3( 0.0f, -1.0f, 0.0f );
		v.uv = float2( 1.0f, 1.0f );
		v.color = float4( 1.0f, 1.0f, 0.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, -0.5f, -0.5f );
		v.uv = float2( 0.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, -0.5f, +0.5f );
		v.uv = float2( 0.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, -0.5f, +0.5f );
		v.uv = float2( 1.0f, 0.0f );
		m_vertexBuffer.push_back(v);

		v.position = float3( -0.5f, -0.5f, +0.5f );
		v.normal = float3( -1.0f, 0.0f, 0.0f );
		v.uv = float2( 0.0f, 1.0f );
		v.color = float4( 0.0f, 1.0f, 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, +0.5f, +0.5f );
		v.uv = float2( 0.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, +0.5f, -0.5f );
		v.uv = float2( 1.0f, 0.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( -0.5f, -0.5f, -0.5f );
		v.uv = float2( 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);


		v.position = float3( +0.5f, -0.5f, -0.5f );
		v.normal = float3( 1.0f, 0.0f, 0.0f );
		v.uv = float2( 0.0f, 1.0f );
		v.color = float4( 1.0f, 1.0f, 1.0f, 1.0f );
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, -0.5f );
		v.uv = float2(0.0f, 0.0);
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, +0.5f, +0.5f );
		v.uv = float2(1.0f, 0.0);
		m_vertexBuffer.push_back(v);
		v.position = float3( +0.5f, -0.5f, +0.5f );
		v.uv = float2(1.0f, 1.0);
		m_vertexBuffer.push_back(v);

		m_indexBuffer = {
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		m_camera.SetPosition(float3(1.f, 0.f, 0.f));
	}

	// triangle
	/*{
		Vertex v;
		v.position = float3(-0.5f, 0.f, 0.f);
		v.color = float4(1.f, 1.f, 1.f, 1.f);
		v.uv = float2(1.f, 0.f);
		v.normal = float3(0.f, 0.f, 1.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(0.f, 0.5f, 0.f);
		v.uv = float2(0.f, 0.f);
		m_vertexBuffer.push_back(v);
		v.position = float3(0.5f, 0.f, 0.f);
		v.uv = float2(0.f, 1.f);
		m_vertexBuffer.push_back(v);
		m_indexBuffer = { 0, 1, 2 };
		m_camera.SetPosition(float3(0.3, -0.25f, -0.3));
		m_camera.SetTarget(float3(0.f, 0.25f, 0.f));
	}*/
}

void Renderer::Update(const Timer& timer)
{
	m_passCB.Resolution = float4(m_frameBuffer->GetWidth(), m_frameBuffer->GetHeight(), 1.f / m_frameBuffer->GetWidth(), 1.f / m_frameBuffer->GetHeight());
	m_passCB.Mouse = float4(m_currentMousePos.x, m_currentMousePos.y, m_lastLMouseClick.x, m_lastLMouseClick.y);
	m_passCB.Time = float2(timer.TotalTime(), timer.DeltaTime());
	/*{
		static float theta = 0.0f;
		theta += PI * timer.DeltaTime();
		float3 pos = m_camera.GetPosition();
		pos.x = 0.25f * std::sin(theta);
		pos.z = -0.25f * std::cos(theta);
		m_camera.SetPosition(pos);
		m_camera.UpdateViewMatrix();
	}*/
	float4 delta_pos = float4(m_deltaMousePos.x / m_clientWidth, m_deltaMousePos.y / m_clientHeight, m_deltaMousePos.z / m_clientWidth, m_deltaMousePos.w / m_clientHeight);
	m_camera.Update(delta_pos * 0.5f, m_deltaScroll);
	m_passCB.ViewMat = m_camera.GetViewMatrix();
	m_passCB.ProjMat = m_camera.GetProjectionMatrix();
	//std::cout << m_deltaMousePos;
}

void Renderer::Render(const Timer& timer)
{
	m_frameBuffer->Clear(Color(0.2, 0.2, 0.2, 1.0));

	int num_faces = m_indexBuffer.size() / 3;
	for (int i = 0; i < num_faces; ++i)
	{
		numInVertexAttri = 0;
		numOutVertexAttri = 0;
		// vertex shader stage
		for (int j = 0; j < 3; ++j)
		{
			VSInput* vs_input = &m_vertexBuffer[m_indexBuffer[i * 3 + j]];
			inVertexAttri[j] = m_pipelineState.VS(vs_input, &m_passCB);
			numInVertexAttri++;
		}

		// triangle clipping
		{
			for (int clipping_plane = 0; clipping_plane < Clipping_Plane_Count; ++clipping_plane)
			{
				numOutVertexAttri = 0;
				for (int vertex_idx = 0; vertex_idx < numInVertexAttri; ++vertex_idx)
				{
					auto& last_vertex = inVertexAttri[(vertex_idx - 1 + numInVertexAttri) % numInVertexAttri];
					auto& current_vertex = inVertexAttri[vertex_idx];
					bool lv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position);
					bool cv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, current_vertex.sv_position);
					if (cv_inside)
					{
						// if need clip, add intersection point to out array
						if (!lv_inside)
						{
							float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
							VSOut& intersection_point = outVertexAttri[numOutVertexAttri];
							intersection_point.LerpAssgin(last_vertex, current_vertex, t);
							numOutVertexAttri++;
						}
						outVertexAttri[numOutVertexAttri] = current_vertex;
						numOutVertexAttri++;
					}
					else if (lv_inside)
					{
						float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
						VSOut& intersection_point = outVertexAttri[numOutVertexAttri];
						intersection_point.LerpAssgin(last_vertex, current_vertex, t);
						numOutVertexAttri++;
					}
				}
				std::swap(inVertexAttri, outVertexAttri);
				numInVertexAttri = numOutVertexAttri;
			}
		}

		// triangle assembly
		{
			for (int v_idx = 0; v_idx < numInVertexAttri - 2; ++v_idx)
			{
				int idx0 = 0;
				int idx1 = v_idx + 1;
				int idx2 = v_idx + 2;

				outVertexAttri[0] = inVertexAttri[idx0];
				outVertexAttri[1] = inVertexAttri[idx1];
				outVertexAttri[2] = inVertexAttri[idx2];

				// rasterize triangle

				// perspective division
				static float3 ndc_coords[3];
				static float recip_w[3];
				for (int j = 0; j < 3; ++j)
				{
					recip_w[j] = 1.f / outVertexAttri[j].sv_position.w;
					outVertexAttri[j].sv_position = outVertexAttri[j].sv_position / outVertexAttri[j].sv_position.w;
					ndc_coords[j] = float3(outVertexAttri[j].sv_position);
				}

				// face culling
				if (m_pipelineState.RasterizerState.CullMode != Cull_Mode_None)
				{

					auto v0 = ndc_coords[0];
					auto v1 = ndc_coords[1];
					auto v2 = ndc_coords[2];
					float r = v0 * Cross(v1 - v0, v2 - v0);
					// front ccw
					bool is_back_face = !(r >= 0 ^ m_pipelineState.RasterizerState.FrontCounterClockWise);
					bool is_culling = !(m_pipelineState.RasterizerState.CullMode == Cull_Mode_Back ^ is_back_face);
					if (is_culling)
						continue;
				}

				// viewport mapping
				static float2 screen_coords[3];
				static float screen_depth[3];
				for (int j = 0; j < 3; ++j)
				{
					float3 ndc_coord = float3(outVertexAttri[j].sv_position);
					float x = (ndc_coord.x + 1.f) * 0.5f * (float)m_frameBuffer->GetWidth() + m_viewport.TopLeftX;
					float y = (1.f - ndc_coord.y) * 0.5f * (float)m_frameBuffer->GetHeight() + m_viewport.TopLeftY;
					float z = m_viewport.MinDepth + ndc_coord.z * (m_viewport.MaxDepth - m_viewport.MinDepth);
					outVertexAttri[j].sv_position = float4(x, y, z, 1.0f);
					screen_coords[j] = float2(x, y);
					screen_depth[j] = z;
				}

				// TODO: build bounding box
				// TODO: add wire frame rasterizer mode
		//#pragma omp parallel for schedule(dynamic)
				for (int x = 0; x < m_frameBuffer->GetWidth(); ++x)
				{
					for (int y = 0; y < m_frameBuffer->GetHeight(); ++y)
					{
						float2 point = float2((float)x + 0.5f, (float)y + 0.5f);
						float3 weights;
						{
							float2 a = screen_coords[0];
							float2 b = screen_coords[1];
							float2 c = screen_coords[2];
							float2 bp = point - b;
							float2 bc = c - b;
							float2 ba = a - b;
							float2 cp = point - c;
							float2 ca = a - c;
							float alpha = (-bp.x * bc.y + bp.y * bc.x) / (-ba.x * bc.y + ba.y * bc.x);
							float beta = (-cp.x * ca.y + cp.y * ca.x) / (bc.x * ca.y - bc.y * ca.x);
							weights = float3(alpha, beta, 1 - alpha - beta);
						}
						// if pixel inside triangle
						if (weights.x > -std::numeric_limits<float>::epsilon() &&
							weights.y > -std::numeric_limits<float>::epsilon() &&
							weights.z > -std::numeric_limits<float>::epsilon())
						{
							// interpolate depth
							float depth = screen_depth[0] * weights.x + screen_depth[1] * weights.y + screen_depth[2] * weights.z;

							// TODO: add early depth test
							// if (pipelineState.depthStencilState... && depth < depthBuffer[x][y])
							// interpolate vertex attributes
							PSInput pixel_attri;
							{
								float* a0 = (float*)&(outVertexAttri[0]);
								float* a1 = (float*)&(outVertexAttri[1]);
								float* a2 = (float*)&(outVertexAttri[2]);
								float* r = (float*)&pixel_attri;
								float weight0 = recip_w[0] * weights.x;
								float weight1 = recip_w[1] * weights.y;
								float weight2 = recip_w[2] * weights.z;
								float norm = 1.f / (weight0 + weight1 + weight2);
								// perspective correct interpolation
								for (int j = 0; j < sizeof(PSInput) / sizeof(float); ++j)
								{
									float attri = norm * (a0[j] * weight0 + a1[j] * weight1 + a2[j] * weight2);
									r[j] = attri;
								}
							}
							// TODO: multiple render targets
							// pixel shader stage
							Color pixel_color = m_pipelineState.PS(&pixel_attri, &m_passCB);
							m_frameBuffer->SetColorBGR(x, y, pixel_color);
						}
					}
				}
			}
			}
		}
		

	m_deltaScroll = 0.0f;
	m_deltaMousePos = float4(0.f, 0.f, 0.f, 0.f);
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
	/*if (m_frameBuffer && m_frameBuffer->GetBuffer() != nullptr)
		delete m_frameBuffer->GetBuffer();*/
	//if (m_frameBuffer != nullptr)
		//delete m_frameBuffer;
	m_memoryDC = CreateFrameBuffer();
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = (float)m_clientWidth;
	m_viewport.Height = (float)m_clientHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

void Renderer::OnKeyDown(WPARAM key)
{

}

void Renderer::OnKeyUp(WPARAM key)
{

}

void Renderer::OnMouseDown(button_t button, int x, int y)
{
	m_lastLMouseClick.x = x;
	m_lastLMouseClick.y = y;


	SetCapture(m_hMainWnd);
}

void Renderer::OnMouseUp(button_t button, int x, int y)
{
	ReleaseCapture();
}

void Renderer::OnMouseScroll(float scroll)
{
	m_deltaScroll += scroll;
}

void Renderer::OnMouseMove(WPARAM btnState, int x, int y)
{
	//m_deltaMousePos = float4(0.0f);
	if ((btnState & MK_LBUTTON) != 0)
	{
		m_deltaMousePos.x += (x - m_currentMousePos.x);
		m_deltaMousePos.y += (y - m_currentMousePos.y);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		m_deltaMousePos.z += (x - m_currentMousePos.x);
		m_deltaMousePos.w += (y - m_currentMousePos.y);
	}
			
	m_currentMousePos.x = x;
	m_currentMousePos.y = y;
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