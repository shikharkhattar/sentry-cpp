#ifndef __SENTRYCLIENT_IMPL_H_
#define __SENTRYCLIENT_IMPL_H_

#include "sentry_client.hpp"

/* TODO:
 * Async
 */

using namespace sentry;

inline SentryClient::SentryClient(std::string dsn, int _timeout)
{
    if (!dsn.length()) {
        /* Empty DSN. Disable client (as per docs) */
        this->disabled = true;
    }
    else {
        this->disabled = false;

        /* parse DSN */
        this->parseDSN(dsn);

        /* Setup http client */
        this->options.follow_redirects(true)
            .cache_resolved(true)
            .timeout(_timeout);
    }
}


inline SentryClient::~SentryClient()
{}


inline void SentryClient::parseDSN(std::string dsn)
{
    /* Format (as per API):
     * '{PROTOCOL}://{PUBLIC_KEY}:{SECRET_KEY}@{HOST}/{PATH}{PROJECT_ID}' */
    std::string tmp_str;

    /* TODO: check / validate dsn */
    /* project_id */
    unsigned found = dsn.find_last_of("/");
    this->project_id = dsn.substr(found + 1);
    tmp_str = dsn.substr(0, found);

    /* url */
    found = dsn.find_last_of("@");
    this->url = "https://";
    this->url = this->url.append(tmp_str.substr(found + 1));
    this->url = this->url.append("/api/");
    this->url = this->url.append(project_id);
    this->url = this->url.append("/store/");

    /* public and secret key */
    tmp_str = dsn.substr(0, found);
    found = tmp_str.find_first_of("//");
    std::string key = tmp_str.substr(found + 2);
    found = key.find_last_of(":");
    this->public_key = key.substr(0, found);
    this->secret_key = key.substr(found + 1);
}


inline bool SentryClient::capture(SentryMessage& msg)
{
    if (this->disabled)
        return true;

    try {
        http::client::request request(this->url);

        /* Request headers */
        std::stringstream sentry_auth_header;

        sentry_auth_header << "Sentry sentry_version=7"
            << ", sentry_timestamp=" << time(NULL)
            << ", sentry_key=" << this->public_key
            << ", sentry_secret=" << this->secret_key
            << ", sentry_client=sentry-cpp/1.0";

        request << network::header("X-Sentry-Auth", sentry_auth_header.str());
        request << network::header("User-Agent", "sentry-cpp/1.0");
        request << network::header("Content-Type", "application/json");

        /*SK: TODO:FIXME:
         * request << network::header("Content-Encoding", "gzip");
         * */

        /* Generate payload */
        nlohmann::json data;

        data["event_id"] = msg.getID();
        data["culprit"] = msg.getTitle();
        data["message"] = msg.getMessage();
        data["level"] = msg.getLevel();
        data["platform"] = "C++";

        /* System Information */
        char server_name[50];
        int retval = gethostname(server_name, sizeof(server_name));
        if (!retval) {
            data["server_name"] = server_name;
        }

        /* Get extra data (if any) added by the user */
        data["extra"] = msg.getExtra();
        /* Add stacktrace to extra data */
        data["extra"]["backtrace"] = this->generateStackTrace();

        /* Send the POST request */
        char body_str_len[8];
        sprintf(body_str_len, "%lu", data.dump().length());
        request << network::header("Content-Length", body_str_len);

        /* Using a local variable because using a shared_ptr (as object variable)
         * led to race conditions and segfaults
         */
        http::client client(this->options);
        http::client::response response = client.post(request, data.dump());
        if (status(response) == HTTP_OK) {
            return true;
        }
        else {
            std::cout << "Error while sending payload to sentry" << std::endl;
            std::cout << status(response) << std::endl;
            std::cout << body(response) << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cout << "Exception thrown while pushing to sentry:" <<
            e.what() << std::endl;
        return false;
    }
}

/* https://linux.die.net/man/3/backtrace */
inline nlohmann::json SentryClient::generateStackTrace(uint32_t size)
{
    int nptrs;
    void *buffer[size];
    char **strings;
    nlohmann::json bt;

    nptrs = backtrace(buffer, size);
    strings = backtrace_symbols(buffer, nptrs);
    if (!strings) {
        return bt;
    }

    for (int j = 0; j < nptrs; j++) {
        bt.push_back(strings[j]);
    }

    free(strings);
    return bt;
}

inline bool SentryClient::is_active()
{
    return !(this->disabled);
}

#endif
