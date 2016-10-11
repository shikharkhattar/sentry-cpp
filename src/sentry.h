#ifndef  __SENTRY_H_
#define  __SENTRY_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>

#include "json/json.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/network/protocol/http/client.hpp>

#include "types.h"
#include "SentryMessage.h"

namespace http = boost::network::http;

class Sentry
{
    private:
        std::string url;
        std::string public_key;
        std::string secret_key;
        std::string project_id;

        /* Function to generate unique id */
        std::string uuid4();

        /* Function to generate stacktrace */
        nlohmann::json generateStackTrace(uint32_t size = STACK_SIZE);

        boost::uuids::random_generator generator;

        http::client* client;
        http::client::options options;

    public:
        Sentry(std::string, int _timeout = 2);

        ~Sentry();

        bool capture(SentryMessage& msg);
};

#endif //__SENTRY_H_
