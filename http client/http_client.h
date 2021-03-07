#pragma once
/*

*/
#include <string_view>
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

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

	struct http_post_context
	{
		std::wstring	url;
		std::wstring	body;
	};

	struct http_get_context
	{
		std::wstring	url;
		std::wstring	message;
	};


	static constexpr uint16_t	HTTP_WELL_KNOWN_PORT{ 80 };
	static constexpr uint16_t	HTTPS_WELL_KNOWN_PORT{ 443 };
	static constexpr size_t		RESPONSE_BUFFER_SIZE{ 1500 };

	inline const wchar_t* GET_FORMAT =
		L"GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nContent-Length: %d\r\nUser-Agent: charg2";

	inline const wchar_t* POST_FORMAT =
		L"POST %s:%d HTTP/1.1\r\nHost: %s : %d\r\nConnection: keep-alive\r\nContent-Length: %d\r\nUser-Agent: charg2\r\n\r\n%s";


	// http://domain:5500/path/get_hash_key.php
	class http_client
	{
	public:
		http_client(const std::wstring& url);
		~http_client();

		// int setup_opt(e_http_req_opt new_opt);

		bool do_request_get(const http_get_context& context);
		bool do_request_post(const http_post_context& context);


	private:
		bool	parse_url(const std::wstring& url);
		bool	resolve_domain(std::wstring& ip);
		SOCKET	connect_to_host(const std::wstring& ip);
		bool	encode_utf16_to_utf8(std::wstring& msg);
		bool	encode_utf8_to_utf16(std::wstring& msg);

	private:
		std::wstring		host;
		std::wstring		path;
		uint16_t			port;
		e_http_client_error	last_error;
	};

}//	namespace c2::net

