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

namespace http = boost::network::http;


enum HTTP_STATUS_CODES
{
    HTTP_OK = 200
};


const int STACK_SIZE = 10;

class Sentry
{
    private:
        std::string url;

        std::string public_key;

        std::string secret_key;

        std::string project_id;

        std::string uuid4();

        nlohmann::json generateStackTrace(uint32_t size = STACK_SIZE);

        bool captureMessage(std::string,
                std::string,
                std::string,
                void* extra_data = NULL);

        boost::uuids::random_generator generator;

        http::client* client;
        http::client::options options;

    public:
        Sentry(std::string, int _timeout = 2);

        ~Sentry();

        bool error(const char *title, const char *message = NULL, void *extra = NULL);

        bool warn (const char *title, const char *message = NULL, void *extra = NULL);

        bool info (const char *title, const char *message = NULL, void *extra = NULL);

        bool debug(const char *title, const char *message = NULL, void *extra = NULL);
};

#endif //__SENTRY_H_
