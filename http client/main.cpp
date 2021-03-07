#include "http_client.h"

#include <iostream>

auto main() -> int
{
	/*
		POST / HTTP / 1.1
		User - Agent: Fiddler
		Host : naver.com
	*/

	//std::wstring url{ L"http://naver.com/" };
	std::wstring url{ L"https://search.naver.com/search.naver?where=nexearch&sm=top_hty&fbm=1&ie=utf8&query=%ED%9C%B4%EC%9D%BC" };

	// parse
	c2::net::http_client client(url);

	c2::net::http_get_context context;
	context.url = url;

	client.do_request_get(context);


	return 0;
}