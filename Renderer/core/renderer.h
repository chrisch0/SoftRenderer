#pragma once
#include <assert.h>
#include <windows.h>
#include <string>
#include <functional>
#include <vector>
#include "frame_buffer.h"
#include "graphics.h"
#include "utils/timer.h"
#include "camera.h"

class Renderer
{
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	static Renderer* GetApp();
	bool Initialize(VertexShader vs, PixelShader ps);
	void InitScene();
	void MainLoop();
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Timer m_timer;
	bool m_appPaused;
	static Renderer* m_app;
	HWND m_hMainWnd = nullptr;
	HDC m_memoryDC;
	std::wstring m_mainWndCaption = L"Soft Renderer";
	double m_clientWidth = 800;
	double m_clientHeight = 600;

	bool InitMainWindow();
	HDC CreateFrameBuffer();

	void Update(const Timer& timer);
	void Render(const Timer& timer);

	typedef enum { BUTTON_L, BUTTON_R, BUTTON_MID } button_t;
	void CalculateFrameStats();
	void OnResize();
	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);
	void OnMouseDown(button_t button, int x, int y);
	void OnMouseUp(button_t button, int x, int y);
	void OnMouseScroll(float scroll);
	void OnMouseMove(WPARAM btnState, int x, int y);
	
	PipelineState m_pipelineState;

	VSOut inVertexAttri[10];
	PSInput outVertexAttri[10];
	ConstantBuffer m_passCB;

	std::vector<Vertex> m_vertexBuffer;
	std::vector<uint32_t> m_indexBuffer;
	FrameBuffer* m_frameBuffer;

	float2 m_lastLMouseClick;
	float2 m_currentMousePos;
	float4 m_deltaMousePos;

	Camera m_camera;
};