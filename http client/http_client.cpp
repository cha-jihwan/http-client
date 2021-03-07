#include "http_client.h"
#include <ws2tcpip.h>

c2::net::http_client::http_client(const std::wstring& url)
	:last_error{ e_http_client_error::hcem_err_none }
{
	//if (false == parse_url(url->c_str()))
	//{
	//	last_error = e_http_client_error::hcem_parsing_failure;
	//	return;
	//}

	WSADATA wsa_data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		int error_code = WSAGetLastError();
		return;// -1;
	}

}

c2::net::http_client::~http_client()
{
	WSACleanup();
}

bool c2::net::http_client::do_request_get(const http_get_context& context)
{
	parse_url(context.url);
	resolve_domain(this->host);
	//SOCKET sock = connect_to_host();

	//send_to_host();
	//recv_from_host();

	return true;
}

bool c2::net::http_client::do_request_post(const http_post_context& context)
{
	return false;
}



SOCKET c2::net::http_client::connect_to_host(const std::wstring& ip, uint16_t port)
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	// bind address
	SOCKADDR_IN sock_addr{};
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	InetPtonW(AF_INET, ip.c_str(), &sock_addr.sin_addr);


	// non-blocking 소켓으로 변경.
	unsigned long enable_non_bloking_mode = true;
	if (SOCKET_ERROR != ioctlsocket(sock, FIONBIO, &enable_non_bloking_mode))
	{
		size_t err_code = WSAGetLastError();
		printf("ioctlsocket failed with error: %ld\n", err_code);
	}

	// 이건 옵션의 여지로?
	int disable_nagle = true;
	if (SOCKET_ERROR == setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&disable_nagle, sizeof(char)))
	{
		size_t err_code = WSAGetLastError();
		printf("ioctlsocket failed with error: %ld\n", err_code);

		return INVALID_SOCKET;
	}

	// trying connect non-blocking socket
	int ret_val = ::connect(sock, (sockaddr*)&sock_addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == ret_val)
	{
		int ret = WSAGetLastError();
		switch (ret)
		{
			case WSAEISCONN: // 이미 연결된 소켓.
			{
				break;
			}
			case WSAEWOULDBLOCK: // 연결중 time out 
			{	// select로 연결 확인.
				fd_set write_set{ 1, {sock} };
				fd_set except_set{ 1, {sock} };
				timeval time_val{ 3, 0 }; // 3초 대기

				select(0, 0, &write_set, &except_set, &time_val);
				if (write_set.fd_count == 0)
				{
					return INVALID_SOCKET;
				}

				break;
			}
			default:
			{
				size_t err_code = WSAGetLastError();
				printf("ioctlsocket failed with error: %ld\n", err_code);
				return INVALID_SOCKET;
			}
		}
	}


	// 다시 blocking socket 로 변경
	// blocking send recv 를 위해 
	enable_non_bloking_mode = false;
	if (SOCKET_ERROR != ioctlsocket(sock, FIONBIO, &enable_non_bloking_mode))
	{
		//printf("ioctlsocket failed with error: %ld\n", ret);
		closesocket(sock);
		return INVALID_SOCKET;
	}


	// time wait 없애기.
	LINGER linger_opt{ 1, 0 };
	if (SOCKET_ERROR == setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&linger_opt, sizeof(linger_opt)))
	{
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}

bool c2::net::http_client::resolve_domain(const std::wstring& ip)
{
	addrinfoW hints{ AI_CANONNAME, PF_UNSPEC, SOCK_STREAM };
	addrinfoW* result{};

	int errcode = GetAddrInfoW(host.c_str(), NULL, &hints, &result);
	if (errcode != 0)
	{
		return false;
	}
	
	void* ptr{};
	switch (result->ai_family)
	{
	case AF_INET:
		ptr = &((struct sockaddr_in*)result->ai_addr)->sin_addr;
		break;
	case AF_INET6:
		ptr = &((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
		break;
	}

	wchar_t buffer[1024]{};
	InetNtopW(result->ai_family, ptr, buffer, sizeof(buffer) / sizeof(wchar_t));

	FreeAddrInfoW(result);

	return true;
}



bool c2::net::http_client::parse_url(const std::wstring& url)
{
	constexpr std::wstring_view c_protocol_https{ L"https://" };
	constexpr std::wstring_view c_protocol_http{ L"http://" };

	std::wstring_view url_str{ url };
	uint16_t port{ HTTP_WELL_KNOWN_PORT };
	uint16_t protocol{ HTTP_WELL_KNOWN_PORT };

	size_t pos = url_str.find(c_protocol_https); // 443
	if (std::wstring_view::npos != pos)
	{
		port = HTTPS_WELL_KNOWN_PORT;

		url_str.remove_prefix(c_protocol_https.length());
	}
	else
	{
		pos = url_str.find(c_protocol_http); // 443
		if (std::wstring_view::npos != pos)
		{
			port = HTTP_WELL_KNOWN_PORT;

			url_str.remove_prefix(c_protocol_http.length());
		}
	}


	// 포트 세팅 /를 찾는다.
	// / 있으면 : 찾고
	// :가 없으면 포트 그대로...
	// :가 있으면 포트 변경 + 1부터 끝가지 (이미 /로 끝을낸 substr)
	// / 없으면 끝.
	size_t server_name_end_pos = url_str.find(L"/");
	std::wstring_view host;
	std::wstring_view path;
	if (std::wstring_view::npos == server_name_end_pos) // / 없으면 끝.
	{
		host = url_str;
	}
	else
	{
		path = url_str.substr(server_name_end_pos + 1);
		url_str.remove_suffix(url_str.size() - server_name_end_pos);
		host = url_str;//.substr(0, server_name_end_pos);
	}

	this->path = path;

	//// : 포트가 있는지...
	size_t port_identifire_pos = host.find(L":");
	if (std::wstring_view::npos == port_identifire_pos) // 없다면... PASS // 있다면 처리.
	{
		this->host = host;
		this->port = port;

		return true;
	}


	std::wstring port_str(&host.data()[port_identifire_pos + 1], host.size() - port_identifire_pos - 1);
	this->port = std::stoi(port_str);
	host.remove_suffix(port_str.size());

	this->host = host;

	return true;
}
