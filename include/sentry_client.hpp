#ifndef  __SENTRY_CLIENT_H_
#define  __SENTRY_CLIENT_H_

/* TODO: FIXME (boost crashes)
 * #define BOOST_NETWORK_ENABLE_HTTPS
 */

#include <iostream>
#include <string>
#include <execinfo.h>
#include <exception>

#include "json/json.hpp"

#ifndef BOOST_NETWORK_ENABLE_HTTPS
#define BOOST_NETWORK_ENABLE_HTTPS
#endif
#include <boost/network/protocol/http/client.hpp>

#include "sentry_message.hpp"

/* SK: don't like this ... look for an alternative
 * problem is to capture file, line and function info
 * which is not trivial with a function call */
#define CAPTURE_MESSAGE(client, msg) do {                    \
    char source_file[64];                                    \
    snprintf(source_file, sizeof(source_file), "%s:%d",      \
            __FILE__, __LINE__);                             \
    (msg).setTitle(__PRETTY_FUNCTION__);                     \
    (msg).add_extra("source_file", source_file);             \
    (client).capture((msg));                                 \
} while (0)

namespace sentry {
    enum HTTP_STATUS_CODES
    {
        HTTP_OK = 200,
        HTTP_BAD_REQUEST = 400,
        HTTP_INTERNAL_SERVER_ERROR = 500
    };

    const uint32_t STACK_SIZE = 10;

    namespace http = boost::network::http;
    namespace network = boost::network;

    class SentryClient
    {
        private:

            /* Function to generate stacktrace */
            nlohmann::json generateStackTrace(uint32_t size = STACK_SIZE);

            /* Function to parse dsn */
            void parseDSN(std::string dsn);

            std::string url;
            std::string public_key;
            std::string secret_key;
            std::string project_id;

            http::client::options options;

            bool disabled;

        public:
            SentryClient(std::string, int _timeout = 10);

            ~SentryClient();

            bool capture(SentryMessage& msg);

            bool is_active();
    };
}

#endif //__SENTRY_CLIENT_H_
