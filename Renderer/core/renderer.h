#pragma once
#include <assert.h>
#include <windows.h>
#include <string>
#include <functional>
#include <vector>
#include <array>
#include "pixel_buffer.h"
#include "graphics.h"
#include "utils/timer.h"
#include "camera.h"
#include "scene/scene.h"

struct IO
{
	float4 DeltaMousePos;
	float2 CurMousePos;
	float2 ClickMousePos;
	float DeltaScroll;
};

class Renderer
{
public:
	Renderer();
	Renderer(int width, int height);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	static Renderer* GetApp();
	bool Initialize(Scene* scene);
	void InitScene();
	void MainLoop();
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Timer m_timer;
	bool m_appPaused;
	static Renderer* m_app;
	HWND m_hMainWnd = nullptr;
	HDC m_memoryDC;
	HBITMAP m_curBitmap;
	std::wstring m_mainWndCaption = L"Soft Renderer";
	double m_clientWidth = 800;
	double m_clientHeight = 600;

	bool InitMainWindow();
	HDC CreateFrameBuffer();

	void Update();
	void Render();

	typedef enum { BUTTON_L, BUTTON_R, BUTTON_MID } button_t;
	void CalculateFrameStats();
	void OnResize();
	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);
	void OnMouseDown(button_t button, int x, int y);
	void OnMouseUp(button_t button, int x, int y);
	void OnMouseScroll(float scroll);
	void OnMouseMove(WPARAM btnState, int x, int y);
	
	Viewport m_viewport;
	FrameBuffer* m_frameBuffer;
	IO m_io;
	Camera m_camera;
	Scene* m_scene;
	GraphicsContext m_context;
};