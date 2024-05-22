#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "main.h"
Overlay overlay;


string timeTo(time_t time)
{
	tm* p;
	p = localtime(&time);
	return TOTEXT("%04d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}
void post_chat(const Request& req, Response& res)
{
	printf("\n");
	time_t curTime;
	time(&curTime);
	printf_w(timeTo(curTime).c_str());
	printf_w(req.remote_addr.c_str());
	printf_w(req.path.c_str());
	printf_w(req.body.c_str());
	printf("********************************");
	string repj = R"(
	{
	 "code": 0,
	 "mes": "",
	 "data": ""
	})";

	Document res_json;
	res_json.Parse(repj.c_str());

	Document body_json;
	bool isJson = body_json.Parse(req.body.c_str()).HasParseError();

	do
	{
		if (isJson || !body_json.HasMember("chat"))
		{
			res_json["code"] = Value(1);
			res_json["mes"] = Value(u8"格式错误", res_json.GetAllocator());

			break;
		}

		string chat = aiChat(body_json["chat"].GetString());

		res_json["mes"] = Value(u8"成功", res_json.GetAllocator());
		res_json["data"] = Value(chat.c_str(), res_json.GetAllocator());
	} while (0);



	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	res_json.Accept(writer);


	Sleep(1000);
	res.set_content(buffer.GetString(), "text/plain");
}void get_test(const Request& req, Response& res)
{
	printf("\n");
	time_t curTime;
	time(&curTime);
	printf_w(timeTo(curTime).c_str());
	printf_w(req.remote_addr.c_str());
	printf_w(req.path.c_str());
	printf_w(req.body.c_str());
	printf("********************************");
	string repj = R"(
	{
	 "code": 0,
	 "mes": "",
	 "data": ""
	})";

	Document res_json;
	res_json.Parse(repj.c_str());




	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	res_json.Accept(writer);



	res.set_content(buffer.GetString(), "text/plain");
}

void startSvr()
{
	Server svr;

	// 处理 GET 请求
	svr.Post("/ser/chat", post_chat);

	svr.Get("/", get_test);

	// 启动服务器，监听所有可用的 IP 地址，并指定端口 8080
	if (svr.listen("0.0.0.0", 8080))
	{
		std::cout << "Server listening on port 8080..." << std::endl;
	}
	else
	{
		std::cerr << "Failed to start server." << std::endl;

	}
	while (1) { Sleep(10000000); }
}

string http_get(string path, string path2, httplib::Headers headers )
{
	string res_data;


	httplib::Client cli(path);



	auto res = cli.Get(path2, headers);

	do
	{
		if (!res)
		{
			auto err = res.error();
			std::cout << "Error during request: " << err << std::endl;
			break;
		}

		if (!(res->status == 200)) {
			std::cout << "Request failed, status code: " << res->status << std::endl;
			break;
		}

		//std::cout << "Response: " << res->body << std::endl;


		//// 保存图片数据到文件
		//std::ofstream output_file("1.png", std::ios::binary);
		//if (output_file.is_open()) {
		//	output_file.write(res->body.c_str(), res->body.size());
		//	output_file.close();

		//}
		//else {
		//	std::cerr << "创建文件失败" << std::endl;

		//}

		res_data = res->body;
	} while (0);


	return  res_data;
}


int main()
{
	HWND l_hwnd = GetConsoleWindow();
	ShowWindow(l_hwnd, SW_HIDE);
	overlay.start();

	//int w = 0, h = 0;

	//Sleep(5000);
	//string pData = NULL;
	//int data_size = pData.size();
	//for (int i = 1; i <= 21; i++)
	//{
	//	pData = http_get("47.98.177.117", TOTEXT("/img/c_img/%d.png", i));
	//	data_size = pData.size();
	//	ID3D11ShaderResourceView* tmp = 0;
	//	overlay. LoadTextureFromData((void*)pData.c_str(), data_size, &tmp, &w, &h);
	//	overlay.img_c_img.push_back(tmp);
	//}

	int chatContont_int = 0;
	while (1)
	{
		if (overlay.chatContent_question.size() != chatContont_int)
		{

			chatContont_int = overlay.chatContent_question.size();
			overlay.chatContent_answer[chatContont_int - 1] = aiChat(overlay.chatContent_question[chatContont_int - 1]);
			Sleep(500);
			overlay.chatLock = false;
		}

		Sleep(500);

	}
	//startSvr();
	//printf_w(aiChat(u8"碳足迹"));

	//login("hgy", "123");

	httplib::SSLClient cli("docs.qq.com");

	std::string payload = "exportType=0&switches=%7B%22embedFonts%22%3Afalse%7D&exportSource=client&docId=300000000%24BffEnzCanNLb&version=2";

	httplib::Headers headers = {
		{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36 Edg/117.0.2045.41"},
		{"Accept", "application/json, text/plain, */*"},
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"sec-ch-ua", "\"Microsoft Edge\";v=\"117\", \"Not;A=Brand\";v=\"8\", \"Chromium\";v=\"117\""},
		{"traceparent", "00-a4991ede134952844bfb83de5dcf7e9e-e37d510491d48466-01"},
		{"sec-ch-ua-mobile", "?0"},
		{"sec-ch-ua-platform", "\"Windows\""},
		{"origin", "https://docs.qq.com"},
		{"sec-fetch-site", "same-origin"},
		{"sec-fetch-mode", "cors"},
		{"sec-fetch-dest", "empty"},
		{"referer", "https://docs.qq.com/sheet/DQmZmRW56Q2FuTkxi?tab=BB08J2"},
		{"accept-language", "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6"},
		{"Cookie", "pac_uid=0_18fede86a225c; iip=0; RK=iQdpojLtHx; ptcz=2077aa5ecec6402b864b8d87bcd834efad86a58a2bc13734157d1f1215407194; pgv_pvid=1764472080; eas_sid=l1m740u1I9Q1Z5H0i1v4i8v4N0; _qimei_uuid42=17c0a12172810023a05e45687124fdc26d8b0cf6df; _qimei_fingerprint=4bd4ef13a91101752946f37969a899a1; _qimei_q36=; _qimei_h38=8f956f99a05e45687124fdc20200000d317c0a; fingerprint=040d48dc52ab4b42807f06bbbd7486b474; low_login_enable=1; uid=144115210433732257; uid_key=EOP1mMQHGixLWHpkaTJuR1B1RHQvR2dSNWNUZExQTi9FN2owWUNQN2hQSVNpNm1Cam4wPSKBAmV5SmhiR2NpT2lKQlEwTkJURWNpTENKMGVYQWlPaUpLVjFRaWZRLmV5SlVhVzU1U1VRaU9pSXhORFF4TVRVeU1UQTBNek0zTXpJeU5UY2lMQ0pXWlhJaU9pSXhJaXdpUkc5dFlXbHVJam9pYzJGaGMxOTBiMk1pTENKU1ppSTZJbnBNWkhsSVF5SXNJbVY0Y0NJNk1UY3hNemMxTkRZek1pd2lhV0YwSWpveE56RXhNVFl5TmpNeUxDSnBjM01pT2lKVVpXNWpaVzUwSUVSdlkzTWlmUS5RamQ3alNjSXY2Sk5xbTdOWTVkWEQ1TU4wdHpJTlROVTJYOGdpakUtSThZKIikl7EGMAE%3D; utype=qq; DOC_QQ_APPID=101458937; DOC_QQ_OPENID=F281D795B770B9A25E46F8AE3441DFCB; env_id=gray-no3; gray_user=true; DOC_SID=d1fd626ed8a54b4caf9f0ee6d991f2494adce46e54ad4a9b8ca8258263bc0612; SID=d1fd626ed8a54b4caf9f0ee6d991f2494adce46e54ad4a9b8ca8258263bc0612; loginTime=1711162633441; traceid=375f0c5276; TOK=375f0c52763dc776; hashkey=375f0c52; optimal_cdn_domain=docs2.gtimg.com"}
	};

	auto res = cli.Post("/v1/export/export_office", headers, payload, "application/x-www-form-urlencoded");

	if (res) {
		if (res->status == 200) {
			std::cout << "Response: " << res->body << std::endl;
		}
		else {
			std::cout << "Request failed, status code: " << res->status << std::endl;
		}
	}
	else {
		auto err = res.error();
		std::cout << "Error during request: " << err << std::endl;
	}

	//隐藏cmd窗口
	//HWND l_hwnd = GetConsoleWindow();
	//ShowWindow(l_hwnd, SW_HIDE);

	overlay.start();
	while (1) { Sleep(10000000); }
	return 0;

}

wstring toWstring(string str)
{
	// 将字符串转换为宽字符串
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	wstring out = converter.from_bytes(str);
	return out;
}
void printf_w(string str)
{

	std::locale::global(std::locale(""));
	std::wcout.imbue(std::locale(""));
	std::wcout << L"" << toWstring(str) << std::endl;
}




string aiChat(string request_str)
{


	//创建 httplib 客户端对象
	httplib::SSLClient   cli("api.binjie.fun");

	Headers headers = {
		{ R"(sec-ch-ua)", R"("Microsoft Edge";v="117", "Not;A=Brand";v="8", "Chromium";v="117")" },
		{ R"(accept)", R"(application/json, text/plain, */*)" },
		{ R"(content-type)", R"(application/json)" },
		{ R"(sec-ch-ua-mobilee)", R"(?0)" },
		{ R"(user-agent)", R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/037.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/507.36 Edg/110.0.2045.41)" },
		{ R"(sec-ch-ua-platform)", R"("Windows")" },
		{ R"(origin)", R"(https://chat18.aichatos.xyz)" },
		{ R"(sec-fetch-site)", R"(cross-site)" },
		{ R"(sec-fetch-mode)", R"(cors)" },
		{ R"(sec-fetch-dest)", R"(empty)" },
		{ R"(referer)", R"(https://chat18.aichatos.xyz/)" },
		{ R"(accept-encoding)", R"(gzip, deflate, br)" },
		{ R"(accept-language)", R"(zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6)" },
	};



	// 构造要发送的数据
	std::string data = R"(
	{
	  "prompt": "",
	  "userId": "#/chat/1712539622697",
	  "network": true,
	  "system": "",
	  "withoutContext": false,
	 "stream": false
	}
	)";




	//char input[100];
	////
	//gets_s(input, 100);
	//printf("%s\n", input);




	Document json;
	json.Parse((data.c_str()));
	json["prompt"] = Value(request_str.c_str(), json.GetAllocator());


	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	json.Accept(writer);

	data = buffer.GetString();



	// 发起 POST 请求
	auto res = cli.Post("/api/generateStream?refer__1360=WqAh7K4GgDCDkDlE%2BG7DyWxRQtGClRWReD/", headers, data, "application/json");

	// 检查是否请求成功
	if (res && res->status == 200) {
		//std::cout << "请求成功！" << std::endl;

		//printf_w(res->body);
		return res->body;

	}
	else
	{
		//std::cerr << "请求失败：" << (res ? res->status : 0) << std::endl;
		return u8"出现问题，请稍后再试。";

	}
	return u8"nothing";
}
string login(string username, string password)
{


	//创建 httplib 客户端对象
	httplib::Client   cli("10.34.111.187", 8083);



	// 构造要发送的数据
	std::string data = R"(
	{
	 "code": "",
	  "username": "",
	 "password": ""
	}
	)";

	Document json;
	json.Parse(data.c_str());
	json["username"].SetString(username.c_str(), username.size());
	json["password"].SetString(password.c_str(), password.size());

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	json.Accept(writer);
	data = buffer.GetString();

	// 发起 POST 请求
	auto res = cli.Post("/loggin", data, "application/json");

	// 检查是否请求成功
	if (res && res->status == 200) {
		std::cout << "请求成功！" << std::endl;

		printf_w(res->body);
	}
	else {
		std::cerr << "请求失败：" << (res ? res->status : 0) << std::endl;
	}
	return "sdf";
}

