#include "renderer.h"

#include <thread>

#include "imGuiUI.h"
using namespace std;
namespace thr2 {
	DWORD threadId;
	DWORD WINAPI StaticMessageStart(void* Param)
	{
		Overlay* ov = (Overlay*)Param;
		ov->renderer();
		return 0;
	}
	ImGuiUI ui;
	ImGuiUI subtabui;
	ImGuiUI menuui;

}



using namespace thr2;
#include <map>


//SettingState
bool isSkipDialog;
bool isAutoDialog;
bool runSpeed;
bool flySpeed;
int dialogSpeed;
int multiple;
bool isDoubleAttack;
bool isMenu;
float fly;
float movef;
float movef_max;
bool  visible_objectmessage;
bool visible_allobject;
bool isEdit;
bool isShortTeleport;

ImU32 color_yq = ImColor(0, 255, 255);
ImU32 color_red = ImColor(255, 0, 0, 255);
ImU32 color_green = ImColor(0, 255, 127, 255);
ImU32 color_yellow = ImColor(255, 236, 139, 255);
ImU32 color_grey = ImColor(220, 220, 220, 255);
ImU32 color_while = ImColor(247, 247, 247, 255);

ImVec4 ImU32ToImVec4(ImU32 imU32Color) {
	ImVec4 imVec4Color;
	imVec4Color.x = (float)(imU32Color & 0xFF) / 255.0f; // 提取红色分量并归一化
	imVec4Color.y = (float)((imU32Color >> 8) & 0xFF) / 255.0f; // 提取绿色分量并归一化
	imVec4Color.z = (float)((imU32Color >> 16) & 0xFF) / 255.0f; // 提取蓝色分量并归一化
	imVec4Color.w = (float)((imU32Color >> 24) & 0xFF) / 255.0f; // 提取 alpha 分量并归一化
	return imVec4Color;
}



// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT   g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// imgui 回调函数
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool LoadDataFromResource(DWORD resourceID, LPCWSTR type,void** fi,int* size)
{
	// Find the resource
	
	HINSTANCE m_hInstance = GetModuleHandle(nullptr);
	// "ZIP" 是自定义资源类型，可以自己决定
	HRSRC hResource = FindResourceW(m_hInstance, MAKEINTRESOURCE(resourceID), type);

	if (hResource == NULL)
	{
	
		return false;
	}

	
	// Load the resource into memory
	HGLOBAL hResourceData = LoadResource(NULL, hResource);
	if (hResourceData == NULL)
		return false;

	// Lock the resource data
	void* pData = LockResource(hResourceData);
	if (pData == NULL)
		return false;

	// Get the size of the resource
	DWORD dwSize = SizeofResource(NULL, hResource);
	if (dwSize == 0)
		return false;
;
*fi = pData;
*size = dwSize;
	return true;
}
bool Overlay:: LoadTextureFromData(void* pData,int data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
;
	// Create texture
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((stbi_uc*)pData, data_size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);
	return true;
}
bool LoadTextureFromResource(DWORD resourceID, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Find the resource
	
	HINSTANCE m_hInstance = GetModuleHandle(nullptr);
	// "ZIP" 是自定义资源类型，可以自己决定
	HRSRC hResource = FindResourceW(m_hInstance, MAKEINTRESOURCE(resourceID), _T("JPG"));


	if (hResource == NULL)
	{
		hResource = FindResourceW(m_hInstance, MAKEINTRESOURCE(resourceID), _T("PNG"));
		
	}
	if (hResource == NULL)
	{
	
		return false;
	}

	
	// Load the resource into memory
	HGLOBAL hResourceData = LoadResource(NULL, hResource);
	if (hResourceData == NULL)
		return false;

	// Lock the resource data
	void* pData = LockResource(hResourceData);
	if (pData == NULL)
		return false;

	// Get the size of the resource
	DWORD dwSize = SizeofResource(NULL, hResource);
	if (dwSize == 0)
		return false;

	// Create texture
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((stbi_uc*)pData, dwSize, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&subResource, sizeof(subResource));
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);
	return true;
}


bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);
	return true;
}

float GetPixelRatio()
{
	HDC screen = GetDC(NULL);
	float dpiX = static_cast<float>(GetDeviceCaps(screen, LOGPIXELSX));
	ReleaseDC(NULL, screen);
	return dpiX / 96.0f;
}
void Overlay::DrawImage(ImTextureID texture_id, ImVec2 position, ImVec2 size, ImVec2 uv_max)
{

	ImGui::GetForegroundDrawList()->AddImage(texture_id, position, position + size,uv_max);
}

void Overlay::DrawCircularImage(ImTextureID texture_id, ImVec2 position, ImVec2 size)
{
	ImGui::GetForegroundDrawList()->AddImageRounded(texture_id, position, position + size, {0,0}, {1,1},ImColor(255,255,255 ),50.0f);
}
void Overlay::DrawCircle(ImVec2 center, float radius, ImU32 color, int num_segments, float thickness)
{
	const float segment_angle = 2.0f * 3.1415926f / static_cast<float>(num_segments);
	for (int i = 0; i < num_segments; ++i)
	{
		const float a = segment_angle * static_cast<float>(i);
		const float x = center.x + radius * cosf(a);
		const float y = center.y + radius * sinf(a);
		ImGui::GetForegroundDrawList()->PathLineTo(ImVec2(x, y));

	}
	ImGui::GetForegroundDrawList()->PathStroke(color, false, thickness);
}
void Overlay::DrawArrowToTarget(ImVec2 start, ImVec2 target, float arrow_size, ImU32 color)
{
	// 计算箭头的方向向量和长度
	ImVec2 dir = target - start;
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y);


	// 计算箭头头部的两个边向量
	ImVec2 left = ImVec2(-dir.y, dir.x);
	ImVec2 right = ImVec2(-left.x, -left.y);
	ImVec2 centre = ImVec2(dir.x * 0.3f, dir.y * 0.3f);


	// 绘制箭头线段
	//ImGui::GetForegroundDrawList()->AddLine(start, target, color, 2.0f);


	// 绘制箭头头部 两个三角形拼接
	ImGui::GetForegroundDrawList()->AddTriangleFilled(start + left * arrow_size, target, start + centre, color);
	ImGui::GetForegroundDrawList()->AddTriangleFilled(start + centre, target, start + right * arrow_size, color);

}
void Overlay::DrawLine(float x0, float y0, float x1, float y1, ImU32 color, float thickness)
{

	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x0, y0), ImVec2(x1, y1), color, thickness);
}
void Overlay::DrawText_My(float x, float y, const char* Text, ImU32 color, float fontSize)
{

	ImFont* font = ImGui::GetIO().Fonts->Fonts[1];

	ImGui::GetForegroundDrawList()->AddText(font, font->FontSize, ImVec2(x, y), color, Text);
	return;
	if (fontSize == 0.0f)
		ImGui::GetForegroundDrawList()->AddText(font, font->FontSize, ImVec2(x, y), color, Text);
	else
		ImGui::GetForegroundDrawList()->AddText(font, fontSize, ImVec2(x, y), color, Text);

}
void Overlay::DrawNewText(float x, float y, const char* Text, ImU32 color, float fontSize)
{

	ImU32 color2 = ImColor(0, 0, 0, 128);
	//DrawText_My(x -1, y, Text, color2, fontSize);
	//DrawText_My(x + 1, y, Text, color2, fontSize);
	//DrawText_My(x, y + 1, Text, color2, fontSize);
	//DrawText_My(x, y - 1, Text, color2, fontSize);

	DrawText_My(x - 1, y - 1, Text, color2, fontSize);
	DrawText_My(x + 1, y + 1, Text, color2, fontSize);
	DrawText_My(x - 1, y + 1, Text, color2, fontSize);
	DrawText_My(x + 1, y - 1, Text, color2, fontSize);

	DrawText_My(x, y, Text, color, fontSize);
}
void Overlay::DrawRect(float x, float y, float wide, float height, ImU32 color, ...)
{

	DrawLine(x, y, x + wide, y, color);
	DrawLine(x, y, x, y + height, color);
	DrawLine(x + wide, y, x + wide, y + height, color);
	DrawLine(x, y + height, x + wide, y + height, color);

}
void Overlay::RectFilled(ImVec2 start, ImVec2 end, ImU32 color, float rounding, int rounding_corners_flags)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(start, end, color, rounding, rounding_corners_flags);
}
void Overlay::DrawProgressBar(ImVec2 start, float w, float h, float value, float v_max)
{

	ImColor barColor = ImColor(
		min(510 * (v_max - value) / 100, 255),
		min(510 * value / 100, 255),
		25.0f,
		255.0f
	);



	RectFilled(start, start + ImVec2(w, ((h / float(v_max)) * (float)value)), barColor, 0.0f, 0);

}


int Overlay::start()
{

	CreateThread(0, 0, StaticMessageStart, this, 0, &threadId);

	return 0;
}

bool ExportToFile(const std::wstring& exportFilePath, const void* pBuffer, DWORD bufferLength)
{
	if (pBuffer == NULL || bufferLength <= 0)
	{
		return false;
	}
	HANDLE hFile = ::CreateFile(exportFilePath.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == NULL)
	{
		return false;
	}

	DWORD writetem = -1;
	BOOL ret = ::WriteFile(hFile, pBuffer, bufferLength, &writetem, NULL);
	if (writetem != bufferLength)
	{
		::CloseHandle(hFile);
		return false;
	}

	::CloseHandle(hFile);
	return true;
}

/**
* exportPath:文件路径，
* resourceId:资源ID ：Resource.h中
* 导出资源包转成指定文件
*/
bool ExportRes(const std::wstring& exportPath, DWORD resourceId)
{
	HINSTANCE m_hInstance = GetModuleHandle(nullptr);
	// "ZIP" 是自定义资源类型，可以自己决定
	HRSRC hrSrc = FindResourceW(m_hInstance, MAKEINTRESOURCE(resourceId), _T("image"));
	if (hrSrc == NULL)
	{
		return false;
	}

	HGLOBAL hGlobalResource = LoadResource(m_hInstance, hrSrc);
	if (hGlobalResource == NULL)
	{
		return false;
	}

	const void* pResourceData = ::LockResource(hGlobalResource);
	if (!pResourceData)
	{
		return false;
	}

	DWORD resLength = SizeofResource(m_hInstance, hrSrc);
	bool ret = ExportToFile(exportPath, pResourceData, resLength);

	FreeResource(hGlobalResource);
	return ret;
}

int Overlay::renderer()
{
	
	// 屏幕宽度和高度
	screenSize.x = 1024;
	screenSize.y = 1024;
	
	WNDCLASSEXW wc = { sizeof(wc), CS_VREDRAW | CS_HREDRAW, WndProc, 0L, 0L, GetModuleHandle(nullptr), LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1)), nullptr, nullptr, nullptr, L" ", nullptr};
	::RegisterClassExW(&wc);

	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L" ", WS_OVERLAPPEDWINDOW, 0, 0, screenSize.x, screenSize.y, nullptr, nullptr, wc.hInstance, nullptr);


	//// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}


	// Show the window
	::ShowWindow(hwnd, SW_HIDE);
	::UpdateWindow(hwnd);



	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	io.IniFilename = nullptr;//不保存ini

	//c:/windows/fonts/msyhbd.ttc
	//font = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	void* fi = nullptr;
	int fi_size = 0;
	LoadDataFromResource(FONT_M, L"JPG", &fi, &fi_size);
	font = io.Fonts->AddFontFromMemoryTTF(fi, fi_size, 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	

	//font_big = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyhbd.ttc", 22.0f.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	//font_25 = io.Fonts->AddFontFromFileTTF("c:/windows/fonts/msyh.ttc", 25.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	 fi = nullptr;
	 fi_size = 0;
	LoadDataFromResource(FONT_M, L"JPG", &fi, &fi_size);
	font_25 = io.Fonts->AddFontFromMemoryTTF(fi, fi_size, 25.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());



	static const ImWchar icons_ranges[] = { ICON_MIN_FAB, ICON_MAX_16_FAB,0 };

	float iconFontSize = 35.0f * 2.0f / 3.0f;
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	//font_imguiIcon = io.Fonts->AddFontFromFileTTF("d:/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

	 fi = nullptr;
	 fi_size = 0;
	LoadDataFromResource(FONT_ICON,L"JPG" , &fi, &fi_size);
	font_imguiIcon = io.Fonts->AddFontFromMemoryTTF(fi, fi_size, iconFontSize, &icons_config, icons_ranges);

	

	ImFontConfig font_config;
	font_config.PixelSnapH = false;
	font_config.OversampleH = 5;
	font_config.OversampleV = 5;
	font_config.RasterizerMultiply = 1.2f;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0xE000, 0xE226, // icons
		0,
	};

	font_config.GlyphRanges = ranges;
	logo = io.Fonts->AddFontFromMemoryTTF((void*)clarityfont, sizeof(clarityfont), 21.0f, &font_config, ranges);
	tab_title_icon = io.Fonts->AddFontFromMemoryTTF((void*)clarityfont, sizeof(clarityfont), 18.0f, &font_config, ranges);
	//int my_image_width;//图长
	//int my_image_height;//图宽
	//ID3D11ShaderResourceView* my_texture = NULL;
	//bool ret = LoadTextureFromFile("Chest.png", &my_texture, &my_image_width, &my_image_height);

	int w = 0, h = 0;

	string pData = http_get("47.98.177.117", "/img/userimg_man.png");
	int data_size = pData.size();
	LoadTextureFromData((void*)pData.c_str(), data_size, &image_user_img, &w, &h);

	LoadTextureFromResource(IMAGE_LOGO, &image_logo, &w, &h);
	LoadTextureFromResource(IMAGE_BACKGROUND_1, &image_background_1, &w, &h);
	LoadTextureFromResource(IMAGE_BACKGROUND_2, &image_background_2, &w, &h);
	LoadTextureFromResource(IMAGE_BACKGROUND_3, &image_background_3, &w, &h);
	LoadTextureFromResource(IMAGE_TEXT, &image_text, &w, &h);
	LoadTextureFromResource(IMAGE_PAY, &image_pay, &w, &h);
	LoadTextureFromResource(IMAGE_WRITER, &image_writer, &w, &h);


	for (int i = 1; i <= 21; i++)
	{
		pData = http_get("47.98.177.117", TOTEXT("/img/c_img/%d.png", i));
		data_size = pData.size();
		ID3D11ShaderResourceView* tmp = 0;
		LoadTextureFromData((void*)pData.c_str(), data_size, &tmp, &w, &h);
		img_c_img.push_back(tmp);
	}
		


	ImGui::StyleColorsDark();
	ImGui::GetStyle().AntiAliasedLines = true;
	ImGui::GetStyle().AntiAliasedLinesUseTex = true;
	ImGui::GetStyle().AntiAliasedFill = true;
	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive];



	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// 开始执行绘制循环事件

	//初始化局部变量
	bool done = false;
	static ImVec4  clear_color = ImVec4{ ImColor(0, 0, 0,0) };
	const int FPS = 240; // 设置目标帧率为帧/秒
	const std::chrono::milliseconds FRAME_TIME((int)(1000.0 / FPS)); // 计算每帧需要的时间


	//绘制总循环
	while (!done)
	{
		auto start_time = std::chrono::high_resolution_clock::now(); // 记录开始时间



		//窗口反截图
		//SetWindowDisplayAffinity(hwnd, 1);

		//窗口置顶
		//SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置优先级别最高


		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		//////////////////////////////////定义区


		//ImGui::GetForegroundDrawList()->AddLine(ImVec2( 200+50,200 ), ImVec2( 200+50,400+200 ), color_yellow, 2.0f);
		//DrawNewText(500, 500, u8"元素方碑哪怕是擅长战斗与杀戮的我，也从未拥有过如此强大的力量，谢谢你的祝福。", color_while);

		//DrawLine(300+1, 300+1, 550+1, 550+1, ImColor(0, 0, 0, 50), 5.0f);
		//DrawLine(300 -1, 300-1, 550 - 1, 550-1, ImColor(0, 0, 0, 50), 5.0f);
		//DrawLine(300-1, 300+1, 550 -1, 550+1, ImColor(0, 0, 0, 50), 5.0f);
		////DrawLine(300+1, 300 - 1, 550+1, 550 - 1, ImColor(0, 0, 0, 50), 5.0f);
		//DrawLine(300, 300, 550, 550, color_while, 5.0f);
		//login();
		Menu();




		//////////////////////////////////定义区结束
		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		g_pSwapChain->Present(1, 0); // Present with vsync


		auto end_time = std::chrono::high_resolution_clock::now(); // 记录结束时间
		auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); // 计算代码执行时间
		if (delta_time < FRAME_TIME) { // 如果代码执行时间小于每帧需要的时间，则等待剩余时间
			std::this_thread::sleep_for(FRAME_TIME - delta_time);
		}


	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;


	return 0;
}
int Overlay::login()
{

	ImGui::Begin("#login");

	ImGui::Image(image_background_1, ImVec2(960, 720));

	ImGui::End();

	return 0;
}
int Overlay::Menu()
{


	ImGuiStyle style = ImGui::GetStyle();
	style.WindowBorderSize = 0.0f;
	style.WindowRounding = 10.0f;



	ImGui::SetNextWindowSize({ 960, 720 });
	ImGui::Begin(u8"碳足迹计算器", 0, ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoDecoration);
	//ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar

	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;


	auto draw = ImGui::GetWindowDrawList();
	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	ui.draw_list = draw;
	menuui.draw_list = draw;
	subtabui.draw_list = draw;

	draw->ChannelsSplit(10);
	//{
	//	static std::map <ImDrawList*, ImVec2> anim_move;

	//  
	//	auto it_anim = anim_move.find(draw);
	//	if (it_anim == anim_move.end())
	//	{
	//		anim_move.insert({ draw, pos });
	//		it_anim = anim_move.find(draw);
	//	}
	//	

	//	it_anim->second = ImLerp(it_anim->second, pos, ImGui::GetIO().DeltaTime / ui.anim_move_t);
	//	pos = it_anim->second;
	//	ImGui::SetWindowPos(pos);

	//}
	draw->ChannelsSetCurrent(0);
	draw->AddImage(image_background_1, pos, pos + size);

	for (int i = 0; i < 100; i++)
	{
		draw->AddRectFilled(pos + ImVec2(0, 180.0f * i / 100.0f), pos + ImVec2(size.x, 180.0f * (i + 1) / 100.0f), ImColor(0, 0, 0, int(240 / 100.0f * (100 - i))), 0.0f);
	}
	for (int i = 0; i < 100; i++)
	{
		draw->AddRectFilled(pos + ImVec2(0, size.y - 400.0f * i / 100.0f), pos + ImVec2(size.x, size.y - 400.0f * (i + 1) / 100.0f), ImColor(0, 0, 0, int(200 / 100.0f * (100 - i))), 0.0f);
	}
	draw->ChannelsSetCurrent(1);

	{

		ImVec2 point = pos;
		//{
		//	const char* label = u8"原";
		//	ImRect frame_label;
		//	frame_label.Min = point ;
		//	frame_label.Max = frame_label.Min + size;
		//	const ImVec2 label_size = ui.CalcTextSize_ex(font_ys, label, NULL, true);

		//	
		//	//标签位置(上下居中)
		//	ImRect label_bb(frame_label.Min + ImVec2(frame_label.GetWidth() * 0.3f - label_size.x * 0.5f, frame_label.GetHeight() * 0.25f - label_size.y * 0.5f), { 0,0 });
		//	label_bb.Max = label_bb.Min + label_size;
		//	draw->AddText(font_ys, font_ys->FontSize, label_bb.Min, ImColor(255, 255, 255, 20), label);
		//}

		//draw->AddRectFilled(point, ImVec2(point.x + size.x+100, point.y + 65), ImColor(27, 32, 36), style.WindowRounding, ImDrawFlags_RoundCornersLeft);

		//draw->AddRectFilled(point, point+size, ImColor(255, 255,255,50), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
		//draw->AddLine(ImVec2(point.x + 210, point.y + 2), ImVec2(point.x + 210, point.y + size.y - 2), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
		//draw->AddLine(ImVec2(point.x + 90, point.y + 2), ImVec2(point.x + 90, point.y + size.y - 2), ImColor(45, 190, 55, 20));
		//draw->AddLine(ImVec2(point.x, point.y + 65), ImVec2(point.x + size.x, point.y + 65), ImColor(45, 190, 55, 20));
		//draw->AddLine(ImVec2(point.x + 63, point.y + 47), ImVec2(point.x + 195, point.y + 47), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
		//draw->AddText(logo, 42.0f, ImVec2(point.x + 14, point.y + 12), ImColor(147, 190, 66), "\u0041");



		//draw->AddText(tab_title_icon, 18.0f, ImVec2(point.x + 65, point.y + 14), ImColor(147, 190, 66), "B");


		{

			ImVec2 point = pos + ImVec2(90, 0);

			//draw->AddRectFilled(pos, pos + size, ImColor(255, 255, 255, 10));
			ui.beginChild("tab#666", point, ImVec2(size.x, 65));

			/*ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(20.0f, 9.0f));
			ui.iconSelectable(font_imguiIcon, u8"\uf2bd", 35.0f, ImVec2(47, 47));*/

			
			//DrawCircularImage(image_user_img, point + ImVec2(20.0f, 9.0f),ImVec2(45, 45));
			draw->AddImageRounded(image_user_img, point + ImVec2(20.0f, 9.0f), point + ImVec2(20.0f, 9.0f) + ImVec2(45, 45), {0,0}, {1,1}, ImColor(255, 255, 255), 50.0f);
			
			//登陆
			ImVec2 fontsize = ui.CalcTextSize_ex(font, u8" hgy", NULL, true);
			draw->AddText(font, font->FontSize, point + ImVec2(20.0f, 9.0f) + ImVec2(47, 23.5f - fontsize.y / 2.0f), color_grey, u8" hgy4");

			//退出
			ImGui::SetCursorPos(ImVec2(size.x - 90.0f - 47 - 8, 5.0f));
			if (ui.iconSelectable(font_imguiIcon, u8"\u00d7", 30.0f, ImVec2(47, 47)))
			{
				
				exit(0);
			}

			//缩小界面
			ImGui::SetCursorPos(ImVec2(size.x - 90.0f - 47 - 8 - 47 - 8, 5.0f));
			if (ui.iconSelectable(font_imguiIcon, u8"\u2013", 30.0f, ImVec2(47, 47)))
			{
				printf("22");
				ImGui::SetWindowSize(ImVec2(0, 0));
				ImGui::SetWindowPos(ImVec2(0, 0));
				ImGui::SetWindowCollapsed(true, ImGuiCond_Always);
			}

			//下拉框
			ImGui::SetCursorPos(ImVec2(size.x - 90.0f - 47 - 8 - 47 - 8 - 47 - 8, 5.0f));
			if (ui.iconSelectable(font_imguiIcon, u8"\uf0ca", 30.0f, ImVec2(47, 47)))
			{
				printf("sdfsdf");
				
			}

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 25.0f));
			ui.Button(u8"m1", u8"cale1");
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(120.0f, -31.0f));
			ui.Button(u8"m2", u8"cale2");
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(240.0f, -31.0f));
			ui.Button(u8"m3", u8"cale3");
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(360.0f, -31.0f));
			ui.Button(u8"m4", u8"cale4");


			ui.endChild();


		}




		//控件背景
		{
			//draw->AddRectFilled(frame_label.Min, frame_label.Max, ImColor(80, 200, 160, 25));
			/*ImRect label_bb(frame_label.Min + ImVec2(0.0f, frame_label.GetHeight() / 2.0f), { 0,0 });
			label_bb.Max = label_bb.Min + ImVec2(800, 0);
			draw->AddLine(label_bb.Min, label_bb.Max, ImColor(255, 255, 255, 255));*/
		}
		//draw->AddRect(point + ImVec2(1, 1), point + size - ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, 0.03f), style.WindowRounding);
	}




	static int  tab = 0;

	{

		ImVec2 point = pos + ImVec2(0, 130);
		ImVec2 tabSize = ImVec2(90, size.y - 130);
		draw->ChannelsSetCurrent(8);

		bool hov = ImGui::IsMouseHoveringRect(pos, pos + ImVec2(90, size.y));
		if (hov)
		{
			for (int i = 0; i < 100; i++)
			{
				draw->AddRectFilled(pos + ImVec2(0, size.y * i / 100.0f), pos + ImVec2(90, size.y * (i + 1) / 100.0f) + ImVec2(tabSize.x, 0), ImColor(22, 29, 26, int(255 / 100.0f * (100 - i))), 0.0f);
			}



		}
		else
		{
			//draw->AddRectFilled(pos, pos + ImVec2(90, size.y), ImColor(0, 0, 0, 150), 0.0f);
			for (int i = 0; i < 100; i++)
			{
				draw->AddRectFilled(pos + ImVec2(0, size.y * i / 100.0f), pos + ImVec2(90, size.y * (i + 1) / 100.0f), ImColor(0, 0, 0, int(200 / 100.0f * (100 - i))), 0.0f);
			}
		}

		DrawImage(image_logo, pos + ImVec2(22.5, 10), ImVec2(45, 45));
		if (hov)
		{

			ImVec2 fontsize = ui.CalcTextSize_ex(font, u8"碳足迹垃圾队", NULL, true);
			draw->AddText(font, 18.0f, pos + ImVec2(22.5, 10) + ImVec2(45 + 15, 22.5 - fontsize.y * 0.5f), ImColor(1.0f, 1.0f, 1.0f), u8"碳足迹垃圾队");

		}
		draw->ChannelsSetCurrent(9);
		vector<string> icon_arr = {
			u8"\uf015",
			u8"\uf1ec",
			u8"\uf27b",
			u8"\u2699",
			u8"\uf06a"

		};
		vector<string> iconLabel_arr = {
			u8"	首页",
			u8"	计算器",
			u8"	ai提问",
			u8"	设置",
			u8"	关于"
		};
		//draw->AddRectFilled(pos, pos + size, ImColor(255, 255, 255, 10));
		ui.beginChild("tab#0", point + ImVec2(20, 0), tabSize);

		for (int i = 0; i < icon_arr.size(); i++)
		{
			if (hov)
			{
				ImGuiWindow* curwindow = ImGui::GetCurrentWindow();
				ImVec2 pos = curwindow->DC.CursorPos;
				ImVec2 fontsize = ui.CalcTextSize_ex(font, iconLabel_arr[i].c_str(), NULL, true);
				draw->AddText(font, 18.0f, pos + ImVec2(47, 23 - fontsize.y * 0.5f), ImColor(255, 255, 255), iconLabel_arr[i].c_str());

			}

			if (ui.iconSelectable(font_imguiIcon, icon_arr[i].c_str(), 25.0f, ImVec2(47, 47), tab == i))
			{
				tab = i;
			}
		}
		ImGui::SetCursorPos(ImVec2(0.0f, tabSize.y - 47 - 16));
		if (ui.iconSelectable(font_imguiIcon, u8"\uf4c0", 25.0f, ImVec2(47, 47), tab == 666))
		{
			tab = 666;
		}


		ui.endChild();


		static int pretab = tab;
		if (pretab != tab)
		{
			subtabui.anim_move_iconSelectable.clear();
			pretab = tab;

		}


	}
	draw->ChannelsSetCurrent(0);
	static int  subtab_cur = 0;
	{
		ImVec2 point = pos + ImVec2(90, 65);
		ImVec2 tabsize = size - (point - pos);

		//draw->AddRectFilled(point, point + tabsize, ImColor(255, 255, 255, 10));

		switch (tab)
		{
		case 0://首页
		{

			subtabui.beginChild("subtab#0", point, tabsize);
			draw->AddRectFilled(point + ImVec2(60, 80), point + ImVec2(tabsize.x - 60, 80 + 150), ImColor(0, 0, 0, 80), 5.0f);
			draw->AddImage(image_logo, point + ImVec2(60, 80) + ImVec2(120, 75 - 45), point + ImVec2(60, 80) + ImVec2(120, 75 - 45) + ImVec2(90, 90));
			//draw->AddRectFilledMultiColor(point + ImVec2(60, 30), point + ImVec2(tabsize.x - 60, 30 + 150), ImColor(255, 255, 255, 120), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 0), ImColor(255, 255, 255, 120));

			draw->AddImage(image_text, point + ImVec2(60, 80) + ImVec2(120, 75 - 45) + ImVec2(90, 45 - 31), point + ImVec2(60, 80) + ImVec2(120, 75 - 45) + ImVec2(90, 45 - 31) + ImVec2(472, 63));

			ImGui::SetCursorPos({ 130,tabsize.y - 280.0f + 45.0f + 30.0f });
			if (subtabui.Button(0, font_imguiIcon, 25.0f, u8"AI提问 \uf1d8", u8"AI提问", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(130, 45)))
			{
				tab = 2;
			}

			ImGui::SetCursorPos({ 130,tabsize.y - 280.0f });
			if (subtabui.Button(0, font_imguiIcon, 25.0f, u8"官方网站 \uf0ac", u8"官方网站", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(130, 45)))
			{
				// 当按钮被点击时，使用系统默认的浏览器打开网页
				const char* url = "http://47.98.177.117/home_l.html";
				
				std::string command = "start " + std::string(url);
				system(command.c_str());
			}

			ImGui::SetCursorPos({ 130 ,tabsize.y - 280.0f - 45.0f - 30.0f });
			if (subtabui.Button(0, font_imguiIcon, 25.0f, u8"开始计算 \uf0a9", u8"开始计算", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(130, 45)))
			{
				tab = 1;
			}


			static int index = 0;
			switch (index)
			{
			case 0:
			{
				draw->AddImage(image_background_1, point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f), point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f) + ImVec2(390, 260));
				break;
			}
			case 1:
			{
				draw->AddImage(image_background_2, point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f), point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f) + ImVec2(390, 260));
				break;
			}
			case 2:
			{
				draw->AddImage(image_background_2, point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f), point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f) + ImVec2(390, 260));
				break;
			}
			case 3:
			{
				draw->AddImage(image_background_2, point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f), point + ImVec2(130 + 130 + 85, tabsize.y - 280.0f - 45.0f - 60.0f) + ImVec2(390, 260));
				break;
			}
			}

			for (int i = 0; i < 4; i++)
			{
				ImGui::SetCursorPos(ImVec2(130 + 130 + 85 + 90 + (45.0f + 8.0f) * i, tabsize.y - 280.0f - 45.0f - 60.0f + 260));
				if (subtabui.iconSelectable(font_imguiIcon, u8"\uf192", TOTEXT(u8"\uf192%d", i).c_str(), 30.0f, ImVec2(45.0f, 45.0f), index == i))
					index = i;


			}
			ImGui::SetCursorPos(ImVec2(130 + 130 + 85 + 390, tabsize.y - 280.0f - 45.0f - 60.0f + 130 - 23));
			if (subtabui.Button(font_imguiIcon, 30.0f, u8"\u232a", u8"\u232a"))
				index++;

			ImGui::SetCursorPos(ImVec2(130 + 130 + 85 - 47, tabsize.y - 280.0f - 45.0f - 60.0f + 130 - 23));
			if (subtabui.Button(font_imguiIcon, 30.0f, u8"\u2329", u8"\u2329"))
				index--;
			index = index > 3 ? 0 : index < 0 ? 3 : index;



			//subtab_cur = subtab;

			subtabui.endChild();




			break;
		}
		case 1://计算器

		{
			//subtabui.beginChild("subtab#1", point, tabsize);

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 255));
			static int subtab=0;
			static float v = 0.0f;
			static float value_max[21] = 
			{
				20.0f,
				5000.0f,
				5000.0f,
				1000.0f,
				1000.0f,
				5000.0f,
				5000.0f,
				5000.0f,
				5000.0f,
				5000.0f,
				500.0f,
				500.0f,
				500.0f,
				500.0f,
				100.0f,
				100.0f,
				24.0f,
				100.0f,
				24.0f,
				100.0f,
				100.0f

			};
			static float min = 0.0f;
			//ImGui::SliderFloat("sdf", &v, 0, 10);

			draw->AddRectFilled(point + ImVec2(10, 50), point + ImVec2(10, 50) + ImVec2(tabsize.x - 20, tabsize.y - 100), ImColor(0, 0, 0, 100),5.0f);

			//draw->AddRectFilled(point + ImVec2(10, 50 + 200 + 20+ 200 + 50+15), point + ImVec2(10, 50 + 200 + 20 + 200 + 50 + 15) + ImVec2(tabsize.x - 20,60), ImColor(255, 255, 255, 80),5.0f);
			//draw->AddLine(point + ImVec2(10, 50-5), point + ImVec2(10 + tabsize.x - 20, 50-5), ImColor(255, 255, 255,180));
			//draw->AddLine(point + ImVec2(10-5, 50 - 5), point + ImVec2(10 - 5, 50 - 5+ tabsize.y - 100), ImColor(255, 255, 255, 180));
			//draw->AddLine(point + ImVec2(10, 50 - 5), point + ImVec2(10 + tabsize.x - 20, 50 - 5), ImColor(255, 255, 255, 180));
			//draw->AddLine(point + ImVec2(10, 50 - 5), point + ImVec2(10 + tabsize.x - 20, 50 - 5), ImColor(255, 255, 255, 180));
			//draw->AddRectFilled(point + ImVec2(10, 50), point + ImVec2(10, 50) + ImVec2(400, 200), ImColor(0, 0, 0, 80));

			draw->AddLine(point + ImVec2(10, 50) + ImVec2((tabsize.x - 10) * 0.5f, 10), point + ImVec2(10, 50) + ImVec2((tabsize.x - 10) * 0.5f, 200), ImColor(200, 200, 200, 200),1.5f);
			
			draw->AddLine(point + ImVec2(10, 50 + 200 + 20) + ImVec2(0, -12), point + ImVec2(10, 50 + 200 + 20) + ImVec2(tabsize.x - 20, -15), ImColor(200, 200, 200, 255));

			static float value[21] = { 0.0f };
			static float value_ca[21] = 
			{ 
				1.0f,
				35.0f,
				1.02f * 12.0f,
				1.7978 * 12,
				0.4512 * 12,
				0.139,
				0.0109,
				0.032 * 365,
				0.125 * 1.02 * 365,
				0.24 * 365,
				3 / 1.5 / 3600 * 15 * 1.02 * 365,
				0.0001 * 52,
				(0.0229 + 0.001) * 52,
				0.047 * 4 * 365,
				0.6 * 365,
				0.1f,
				0.007 * 365,
				3.3,
				0.0043 * 365,
				0.015 * 12,
				 6.4 * 2
			
			};
			static string value_st[21] =
			{
				u8"家庭成员(人/家)",
				u8"居住地(平方米)",
				u8"用电度数(度/月)",
				u8"天然气(立方/月)",
				u8"每月用水(吨/月)",
				u8"飞机(千米/年)",
				u8"火车(千米/年)",
				u8"公交(千米/天)",
				u8"地铁(站/天)",
				u8"轿车(千米/天)",
				u8"电梯(层/天)",
				u8"塑料袋(个/天)",
				u8"一次性筷子(双/周)",
				u8"新衣服(件/半年)",
				u8"主食(碗/天)",
				u8"肉食(碗/天)",
				u8"使用电脑(小时/天)",
				u8"电子邮件(封/天)",
				u8"搜索(小时/天)",
				u8"书和杂志(册/月)",
				u8"纸张(张/月)"
			};
			float result = 0.0f;
			subtabui.beginChild("subtab#3332", point + ImVec2(10, 50), ImVec2(420, 200));
			
			for (int i = 0; i < 5; i++)
			{


				subtabui.imageSelectable(img_c_img[i], font, value_st[i].c_str(), 20.0f, ImVec2(420, 60.0f), &value[i], &min, &value_max[i]);
				result += value[i] * value_ca[i];
				/*static float vv = 0.0f;
				subtabui.SliderFloat(TOTEXT("SUBTABEL2%d", i).c_str(), &vv,0.0f, 10.0f);*/
			}
			subtabui.endChild();

			subtabui.beginChild("subtab#31121332", point + ImVec2(10 + 400 +50, 50), ImVec2(400, 200));
			for (int i = 5; i < 5+6; i++)
			{

				subtabui.imageSelectable(img_c_img[i], font, value_st[i].c_str(), 20.0f, ImVec2(400, 60.0f), &value[i], &min, &value_max[i]);
				result += value[i] * value_ca[i];
				/*static float vv = 0.0f;
				subtabui.SliderFloat(TOTEXT("SUBTABEL2%d", i).c_str(), &vv,0.0f, 10.0f);*/
			}
			subtabui.endChild();

			subtabui.beginChild("subtab#311ds21332", point + ImVec2(10, 50+200+20), ImVec2(tabsize.x - 20, 200 + 50));
			for (int i = 5 + 6; i < img_c_img.size(); i++)
			{


				subtabui.imageSelectable(img_c_img[i], font, value_st[i].c_str(), 20.0f, ImVec2(tabsize.x - 20, 60.0f), &value[i], &min, &value_max[i]);
				result += value[i] * value_ca[i];
				/*static float vv = 0.0f;
				subtabui.SliderFloat(TOTEXT("SUBTABEL2%d", i).c_str(), &vv,0.0f, 10.0f);*/
			}

			
			subtabui.endChild();
			subtabui.beginChild("subtab#111111155", point + ImVec2(10+80, 50 + 200 + 20 + 200 + 50 + 20), ImVec2(800, 200));

			//ImGui::SetCursorPos(  ImVec2(10, 50 + 200 + 20 + 200 + 50 + 15));
			subtabui.Text(TOTEXT(u8"您一年排放的CO2: %.2f (吨)", (result / 1000.0f)).c_str(), "co2");
			ImGui::SameLine(0.0f, -10.0f);
			subtabui.Text(TOTEXT(u8"13亿人会因此导致全球气温升高:%.2f(度/年)", (0.015f * (result * 1300000000.0f / 2000000000000.0f))).c_str(), "tmp");
			ImGui::SameLine(0.0f, -10.0f);
			subtabui.Text(TOTEXT(u8"需要种树:%.2f(棵)", ((result - 2700.0f * 0.52) / 185.0f) < 0 ? 0.0f : ((result - 2700.0f * 0.52) / 185.0f)).c_str(), "tree");
		
			
		
			ImGui::PopStyleColor(1);

			subtabui.endChild();
			subtab_cur = subtab;
			break;
		}
		case 2://ai提问

		{
			subtabui.beginChild("subtab#1", point, tabsize);
			static char text[1024 * 16] =
				u8"";

			draw->AddRectFilled(point + ImVec2(60, 40), point + ImVec2(tabsize.x - 60, 40 + 550), ImColor(255, 255, 255, 80), 5.0f);
			draw->AddLine(point + ImVec2(60, 40 + 450), point + ImVec2(tabsize.x - 60, 40 + 450), ImColor(255, 255, 255, 80));

			ImGui::SetCursorPos(ImVec2(60 + 60, 40 + 450 + 22));
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 200.0f);
			ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(450, 28 * 2));
			ImGui::SetCursorPos(ImVec2(60 + 60 + 450 + 30, 40 + 450 + 30));
			do
			{
				if (!subtabui.Button(0, font_imguiIcon, 21.0f, u8"发送 \uf1d8", u8"发送", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(100, 40)))
					break;
				if (!strlen(text))
					break;
				if (chatLock)
					break;
				chatContent_question.push_back(text);
				chatContent_answer.push_back(u8"...");
				chatLock = true;

				strcpy_s(text, 1024 * 16, u8"");
			} while (0);


			ImGui::PopTextWrapPos();
			// 在编辑模式下显示可编辑的输入框
			//ImGui::InputText("##edit", text, sizeof(text), ImGuiInputTextFlags_EnterReturnsTrue);
			subtabui.endChild();


			/*ImGui::ItemSize(total_bb, style.FramePadding.y);
			ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_Inputable);*/
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f / 255.0f, 1.0f / 255.0f, 1.0f / 255.0f, 255));
			subtabui.beginChild("chatcon", point + ImVec2(60 + 60 + 10, 40 + 10), ImVec2(tabsize.x - 60 - 60, 40 + 400 - 10));
			float wra = ImGui::GetWindowWidth() * 0.8f;
			//draw->AddRectFilled(point + ImVec2(60+60, 40), point + ImVec2(tabsize.x - 60-60, 40 + 450), ImColor(0, 0, 0, 200), 5.0f);




			for (int i = 0; i < chatContent_question.size(); i++)
			{
				subtabui.Text(chatContent_question[i].c_str(), TOTEXT("#question%d", i).c_str(), wra, 1);
				subtabui.Text(chatContent_answer[i].c_str(), TOTEXT("#answer%d", i).c_str(), wra);
			}




			subtabui.endChild();
			ImGui::PopStyleColor(1);


			break;
		}
		case 3:

		{

			draw->AddRectFilled(point + ImVec2(60, 10), point + ImVec2(tabsize.x - 60, 40 + 550), ImColor(0, 0, 0, 80), 5.0f);
			subtabui.beginChild("subtab#3", point + ImVec2(60, 10), ImVec2(tabsize.x - 60, 40 + 550)- ImVec2(60, 10));

			static bool aa;
			subtabui.Checkbox(u8"开机自启动", &aa);
			subtabui.Checkbox(u8"自动登陆", &aa);
			subtabui.Checkbox(u8"保存日志", &aa);
			subtabui.Checkbox(u8"精确计算", &aa);

			static float v;
			subtabui.SliderFloat(u8"动画速度", &v, 0.1f, 1.0f);

			//for (int i = 0; i < 5; i++)
			//{
			//	static bool ttt;
			//	subtabui.Checkbox(TOTEXT(u8"待添加%d", i).c_str(), &ttt);
			//}
			subtabui.endChild();
			//draw->AddRectFilled(point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f-10.0f, 30.0f-10.0f), point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f) + ImVec2(73.6 * 2+10.0f, 73.6 * 2+10.0f+40.0f), ImColor(0, 0, 0, 100), 5.0f);


			break;
		}
		case 4:

		{

			draw->AddRectFilled(point + ImVec2(200, 10), point + ImVec2(tabsize.x - 200, 40 + 550), ImColor(255, 255, 255, 80), 5.0f);
			//draw->AddRectFilled(point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f-10.0f, 30.0f-10.0f), point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f) + ImVec2(73.6 * 2+10.0f, 73.6 * 2+10.0f+40.0f), ImColor(0, 0, 0, 100), 5.0f);


			string writer_name = u8"@ data";
			string writer_note = u8"这个人很懒。。。";
			string writer_qq = u8"QQ: 930837329";
			string writer_email = u8"Email: 930837329@qq.com";
			string writer_from = u8"From: 碳足迹第八组";
			string writer_thanks = u8"感谢: https://github.com/ocornut/imgui";
			draw->AddImage(image_writer, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f), point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f) + ImVec2(73.6 * 2, 73.6 * 2));

			//昵称
			draw->AddText(font_25, 28, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f + 30.0f, 30.0f + 73.6 * 2 + 10.0f), ImColor(1, 1, 1), writer_name.c_str(), 0, 370.0f);

			//简介
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f + 73.6 * 2 + 10.0f + 30.0f), ImColor(122, 122, 122), writer_note.c_str(), 0, 150.0f);

			//组别
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f + 73.6 * 2 + 10.0f + 60.0f), ImColor(1, 1, 1), writer_from.c_str(), 0, 300.0f);

			//qq
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f + 73.6 * 2 + 10.0f + 90.0f), ImColor(1, 1, 1), writer_qq.c_str(), 0, 300.0f);

			//邮箱			
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f, 30.0f + 73.6 * 2 + 10.0f + 120.0f), ImColor(1, 1, 1), writer_email.c_str(), 0, 300.0f);


			ImGui::SetCursorPos(ImVec2(90, 65) + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f + 10.0f, 30.0f + 73.6 * 2 + 10.0f + 180.0f));
			if (subtabui.Button(0, font_imguiIcon, 22.0f, u8"资助作者  \uf4c0", u8"资助作者 \uf4c0", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(140.0f, 40)))
			{
				tab = 666;
			}

			ImGui::SetCursorPos(ImVec2(90, 65) + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f + 10.0f, 30.0f + 73.6 * 2 + 10.0f + 180.0f + 40.0f + 15.0f));
			if (subtabui.Button(0, font_imguiIcon, 22.0f, u8"了解框架  \uf1d8", u8"了解框架 \uf1d8", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(140.0f, 40)))
			{
	
				// 当按钮被点击时，使用系统默认的浏览器打开网页
				const char* url = "		https://github.com/ocornut/imgui";
				std::string command = "start " + std::string(url);
				system(command.c_str());
			}

			ImGui::SetCursorPos(ImVec2(90, 65) + ImVec2(tabsize.x / 2.0f - 73.6 * 2 / 2.0f + 10.0f, 30.0f + 73.6 * 2 + 10.0f + 180.0f + 40.0f + 15.0f + 40.0f + 15.0f));
			if (subtabui.Button(0, font_imguiIcon, 22.0f, u8"免责声明  \uf0ac", u8"免责声明 \uf0ac", ImColor(235, 235, 235), ImColor(235, 235, 235), ImVec2(140.0f, 40)))
			{
				tab = 666;
			}
			break;
		}
		case 666:
		{
			draw->AddRectFilled(point + ImVec2(120, 10), point + ImVec2(tabsize.x - 120, 40 + 550), ImColor(255, 255, 255, 80), 5.0f);

			string tip1 = u8"软件完全免费使用，采用imgui+dx11框架设计，具有简洁且高度自定义的界面，流畅精细的动画……总之很棒就完事啦 =w=……";
			string tip2 = u8"二维码是自愿捐赠！禁止发布、传播任何违法、违规内容，禁止付费倒卖，违法必究，使用本软件，视您接受并同意《免责声明》!";
			draw->AddImage(image_pay, point + ImVec2(tabsize.x / 2.0f - 111.8 * 2.5 / 2.0f, 30.0f), point + ImVec2(tabsize.x / 2.0f - 111.8 * 2.5 / 2.0f, 30.0f) + ImVec2(111.8 * 2.5, 152.4 * 2.5));
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 111.8 * 2.5 / 2.0f - 30.0f, 30.0f + 152.4 * 2.5 + 15.0f), ImColor(1, 1, 1), tip1.c_str(), 0, 370.0f);
			draw->AddText(font_25, 22, point + ImVec2(tabsize.x / 2.0f - 111.8 * 2.5 / 2.0f - 30.0f, 30.0f + 152.4 * 2.5 + 15.0f + 80.0f), ImColor(255, 41, 41), tip2.c_str(), 0, 370.0f);

			break;
		}

		}

		static int  pre = subtab_cur;

		if (pre != subtab_cur)
		{
			pre = subtab_cur;
			menuui.anim_move_iconSelectable.clear();
		}



	}



	{

		ImVec2 point = pos + ImVec2(90 + 163 + 20, 65);
		ImVec2 menuSize = ImVec2(480, 400);

		//draw->AddRectFilled(point, point + menuSize, ImColor(80, 255, 160, 50));
		switch (tabToInt(tab, subtab_cur))
		{

		case tab0_0:
		{


			//menuui.beginChild("menu#0", point, menuSize);

			//menuui.Text(u8"这是一段测试字符串");
			//if (menuui.Button(u8"恢复设置", u8"Reset")) {};

			//if (menuui.Button(u8"退出程序", "Exit")) { Sleep(200); exit(0); };

			//menuui.Checkbox("ObjectMessage", &visible_objectmessage);

			//menuui.Checkbox("Visible_AllObject", &visible_allobject);

			//menuui.endChild();
			break;
		}
		case tab0_1:
		{
			menuui.beginChild("menu#1", point, menuSize);

			menuui.Checkbox("RunSpeed", &runSpeed);

			if (runSpeed)
			{

				menuui.setNextSubWidget();
				menuui.SliderFloat(u8" ", &movef, 0.0f, 1.0f);


			}

			menuui.Checkbox("FlySpeed", &flySpeed);
			if (flySpeed)
			{
				menuui.setNextSubWidget();
				menuui.SliderFloat(u8"  ", &fly, 0.1f, 50.0f);

			}



			menuui.Checkbox("DoubleAttack", &isDoubleAttack);
			if (isDoubleAttack)
			{
				menuui.setNextSubWidget();
				menuui.SliderInt("     ", &multiple, 2, 20);

			}


			menuui.Checkbox("SkipDialog", &isSkipDialog);

			if (!isSkipDialog)
			{
				menuui.setNextSubWidget();
				menuui.Checkbox("AutoDialog", &isAutoDialog);
				if (isAutoDialog)
				{
					menuui.setNextSubWidget();
					menuui.SliderInt("dialogSpeed", &dialogSpeed, 100, 1000);
				}

			}

			menuui.Checkbox("ShortTeleport", &isShortTeleport);


			static int combo;
			const char* combo_items[5] = { "Value", "Value 1", "Value 2" ,"Value 3" ,"Value 4" };
			menuui.Combo("Combobox", &combo, combo_items, IM_ARRAYSIZE(combo_items));

			menuui.Text(TOTEXT(u8"当前FPS: % .3f ms / frame(% .1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate).c_str(), "fps");

			menuui.endChild();
			break;
		}
		default:
			break;
		}





	}

	/*draw->AddRectFilled(pos + ImVec2(0, size.y - 20), pos + size, ImColor(15, 14, 21), style.WindowRounding, ImDrawFlags_RoundCornersBottom);
	draw->AddText(pos + ImVec2(5, size.y - 18), ImGui::GetColorU32(ImGuiCol_Text), "unknowncheats.me - V1.0");
	draw->AddText(pos + ImVec2(size.x - ImGui::CalcTextSize("Free Cheat").x - 5, size.y - 18), ImGui::GetColorU32(ImGuiCol_Text), "Free Cheat");*/
	ImGui::End();


	draw->ChannelsMerge();
	return 0;

}


int tabToInt(int tab1, int tab2) {
	return (tab1 << 16) | tab2;
}
int Overlay::getThreadID()
{

	return threadId;
}
