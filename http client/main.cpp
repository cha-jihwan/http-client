#include "http_client.h"

#include <iostream>

auto main() -> int
{
	/*
		POST / HTTP / 1.1
		User - Agent: Fiddler
		Host : naver.com
	*/
	std::wstring url{ L"http://naver.com/" };

	c2::net::http_client client(url);

	c2::net::http_get_context context;

	client.do_request_get(context);



	return 0;
}