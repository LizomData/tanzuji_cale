#pragma once

#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <intrin.h>
#include<TlHelp32.h>
#include <string>

#include <chrono>
#include <ctime>
#include <thread>
#include<psapi.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <codecvt>

#include "resource.h"
#include "resource1.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"

#include "imgui\imconfig.h"
#include "imgui\imgui.h"
#include "imgui\imgui_impl_dx11.h"
#include "imgui\imgui_impl_win32.h"
#include "imgui\imgui_internal.h"
#include "imgui\imstb_rectpack.h"
#include "imgui\imstb_textedit.h"
#include "imgui\imstb_truetype.h"


#include "image\stb_image.h"

#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")

#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "http/httplib.h"

using namespace httplib;

#include "renderer.h"
using namespace std;
using namespace rapidjson;

template <typename... Args>
string TOTEXT(const char* Format, Args... Text)
{
	char text[100];
	sprintf_s(text, 100, Format, Text...);
	return string(text);
}


wstring toWstring(string str);
void printf_w(string str);

string aiChat(string request_str);
string login(string username, string password);
string http_get(string path, string path2, httplib::Headers headers = {});
