#ifndef  __SENTRY_CLIENT_H_
#define  __SENTRY_CLIENT_H_

/* TODO: FIXME (boost crashes)
 * #define BOOST_NETWORK_ENABLE_HTTPS
 */

#include <iostream>
#include <string>
#include <execinfo.h>

#include "json/json.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/network/protocol/http/client.hpp>

#include "types.h"
#include "SentryMessage.h"

namespace http = boost::network::http;
namespace network = boost::network;

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


/* API documentation: https://docs.sentry.io/clientdev/overview/ */
class SentryClient
{
    private:
        /* Function to generate unique id */
        std::string uuid4();

        /* Function to generate stacktrace */
        nlohmann::json generateStackTrace(uint32_t size = STACK_SIZE);

        /* Function to parse dsn */
        void parseDSN(std::string dsn);

        std::string url;
        std::string public_key;
        std::string secret_key;
        std::string project_id;

        boost::uuids::random_generator generator;

        http::client* client;
        http::client::options options;

        bool disabled;

    public:
        SentryClient(std::string, int _timeout = 5);

        ~SentryClient();

        bool capture(SentryMessage& msg);
};

#endif //__SENTRY_CLIENT_H_
