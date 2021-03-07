#include "http_client.h"

#include <iostream>

auto main() -> int
{
	/*
		POST / HTTP / 1.1
		User - Agent: Fiddler
		Host : naver.com
	*/

	std::wstring url{ L"http://google.com/" };
	std::wstring get_message{ L"GET / HTTP / 1.1\r\nHost: www.google.com\r\nConnection: keep - alive\r\n\r\nUser - Agent: charg2\r\n\r\n" };

	c2::net::http_get_context context{ url, std::move(get_message) };

	c2::net::http_client client{ url };

	client.do_request_get( context );


	return 0;
}