#include "sentry.h"

/* TODO:
 * Exception handling
 * Extra Data dict
 * Async
 */


Sentry::Sentry(std::string dsn, int _timeout)
{
    /* project_id */
    std::string tmp_str;

    unsigned found = dsn.find_last_of("/");
    this->project_id = dsn.substr(found + 1);
    tmp_str = dsn.substr(0, found);

    /* url */
    found = dsn.find_last_of("@");
    this->url = "http://";
    this->url = this->url.append(tmp_str.substr(found + 1));
    this->url = this->url.append("/api/");
    this->url = this->url.append(project_id);
    this->url = this->url.append("/store/");

    /* public and key */
    tmp_str = dsn.substr(0, found);
    found = tmp_str.find_first_of("//");
    std::string key = tmp_str.substr(found + 2);
    found = key.find_last_of(":");
    this->public_key = key.substr(0, found);
    this->secret_key = key.substr(found + 1);

    this->options.follow_redirects(true)
        .cache_resolved(true)
        .timeout(_timeout);

    this->client = new http::client(options);
}


Sentry::~Sentry()
{
    delete this->client;
}


bool Sentry::captureMessage(
        std::string title,
        std::string message,
        std::string level,
        void* extra_data)
{

    nlohmann::json data;

    http::client::request request(this->url);

    /* Request headers */
    std::stringstream sentry_auth_header;

    sentry_auth_header << "Sentry sentry_version=7"
        << ", sentry_timestamp=" << time(NULL)
        << ", sentry_key=" << this->public_key
        << ", sentry_secret=" << this->secret_key
        << ", sentry_client=sentry-cpp/1.0";

    request << boost::network::header("X-Sentry-Auth", sentry_auth_header.str());
    request << boost::network::header("User-Agent", "sentry-cpp/1.0");
    request << boost::network::header("Content-Type", "application/json");

    /*SK: TODO:FIXME:
     * request << boost::network::header("Content-Encoding", "gzip");
     * */

    /* Generate payload */
    data["event_id"] = uuid4();
    data["culprit"] = title;
    data["message"] = message;
    data["level"] = level;
    data["platform"] = "C++";

    /* System Information */
    char server_name[50];
    int retval = gethostname(server_name, sizeof(server_name));
    if (!retval) {
        data["server_name"] = server_name;
    }

    data["extra"]["backtrace"] = this->generateStackTrace();

    char body_str_len[8];
    sprintf(body_str_len, "%lu", data.dump().length());
    request << boost::network::header("Content-Length", body_str_len);

    http::client::response response = this->client->post(request, data.dump());
    if (status(response) == HTTP_OK) {
        return true;
    }
    else {
        std::cout << status(response) << std::endl;
        std::cout << body(response) << std::endl;
        return false;
    }
}

/* https://linux.die.net/man/3/backtrace */
nlohmann::json Sentry::generateStackTrace(uint32_t size)
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


std::string Sentry::uuid4()
{
    boost::uuids::uuid uuid = generator();
    std::string str_uuid = to_string(uuid);
    /* Remove '-' from the uuid. Sentry complains about max length */
    boost::erase_all(str_uuid, "-");
    return str_uuid;
}


bool Sentry::error(const char *title, const char *message, void *extra)
{
    if (!message) {
        return captureMessage(title, title, "error", extra);
    }
    else {
        return captureMessage(title, message, "error", extra);
    }
}


bool Sentry::warn(const char *title, const char *message, void *extra)
{
    if (!message) {
        return captureMessage(title, title, "warn", extra);
    }
    else {
        return captureMessage(title, message, "warn", extra);
    }
}


bool Sentry::debug(const char* title, const char *message, void *extra)
{
    if (!message) {
        return captureMessage(title, title, "debug", extra);
    }
    else {
        return captureMessage(title, message, "debug", extra);
    }
}


bool Sentry::info(const char* title, const char *message, void *extra)
{
    if (!message) {
        return captureMessage(title, title,"info", extra);
    }
    else {
        return captureMessage(title, message,"info", extra);
    }
}


int main(void)
{
    Sentry s("https://ba3860dad42246dfa851e18205e99232:b4d0113723ea440183cae69d30031251@sentry.io/104441");
    //Sentry s("http://3b218c1c711d4cf5b693717a4ae3c741:a41947bd4c18416f97190cda91ad6a83@127.0.0.1/api/");
    s.error("123testaaaa");
    s.info("123wtfaaa");
    std::cout << "Done" << std::endl;
    return 0;
}
