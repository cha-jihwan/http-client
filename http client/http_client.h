#pragma once
/*
GET / HTTP/1.1
User-Agent: Fiddler
Host: naver.com

POST
*/
#include <string_view>
#include <string>
#include <WinSock2.h>

namespace c2::net
{
	enum class e_http_protocol
	{
		http_none,
		http_get,
		http_post,
		http_delete,
		http_put,
		http_connect,
		http_max
	};

	enum class e_http_req_opt
	{
	};

	enum class e_http_client_error : uint16_t
	{
		hcem_err_none,
		hcem_parsing_failure,
		hcem_winsock_init_failure,
		hcem_err_max,
	};

	struct http_context
	{
		e_http_protocol protocol;
	};

	namespace constant
	{
		static constexpr uint16_t	HTTP_WELL_KNOWN_PORT{ 80 };
		static constexpr uint16_t	HTTPS_WELL_KNOWN_PORT{ 443 };
		static constexpr size_t		RESPONSE_BUFFER_SIZE{ 1500 };

		const wchar_t* GET_FORMAT =
			L"GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nContent-Length: %d\r\nUser-Agent: charg2\r\n\r\n";

		const wchar_t* POST_FORMAT =
			L"POST %s:%d HTTP/1.1\r\nHost: %s : %d\r\nConnection: keep-alive\r\nContent-Length: %d\r\nUser-Agent: charg2\r\n\r\n%s";
	} // namespace detail


	// http://webserver_domain:5500/path/get_hash_key.php
	class http_client
	{
	public:
		http_client(const wchar_t* url);
		~http_client();

		int setup_opt(e_http_req_opt new_opt);
		size_t do_request_post(wchar_t* url, wchar_t* body, wchar_t* out_buffer);

	private:
		bool	parse_url(const wchar_t* url);
		SOCKET	connect_to_host(wchar_t* host, int port);
		bool	resolve_domain_to_ip();

		// 요청한다.
		//int do_http_request(wchar_t* url, const char* post_data, char* recv_buffer);
		//int recv_http_message_from_host(SOCKET sock, char* buffer, size_t buffer_size);

	private:
		wchar_t				host[256];
		uint16_t			port;
		e_http_client_error	last_error;
	};
}//	namespace c2::net

