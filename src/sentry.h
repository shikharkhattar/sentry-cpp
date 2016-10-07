#ifndef  __SENTRY_H_
#define  __SENTRY_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <ctime>

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


class Sentry
{
    private:
        std::string url;

        std::string public_key;

        std::string secret_key;

        std::string project_id;

        std::string uuid4();

        bool captureMessage(std::string,
                std::string,
                std::string,
                void* extra_data = NULL);

        const char *_file;
        int _line;
        const char *_function;
        const char *_function_header;

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
