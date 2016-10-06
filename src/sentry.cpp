#include "sentry.h"

/* TODO:
 * Exception handling
 * Stack Trace
 * Extra Data dict
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
    request << boost::network::header("Content-Encoding", "gzip");

    /* Generate timestamp */
    std::time_t now = std::time(NULL);
    char timestamp[32];
    std::strftime(timestamp, sizeof(timestamp),
            "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    /* Generate payload */
    data["project_id"] = project_id;
    data["event_id"] = uuid4();
    data["culprit"] = title;
    data["timestamp"] = timestamp;
    data["message"] = message;
    data["level"] = level;
    data["platform"] = "C++";

    //SK: FIXME
    /*
    nlohmann::json extra_val;
    if (extra_data  {
        extra_val = *(static_cast<nlohmann::json *>(extra_data));
    }

    extra_val["current file"] = _file;
    extra_val["current line"] = _line;
    extra_val["current function"] = _function;
    extra_val["current function declare"] = _function_header;
    data["extra"] = extra_val;
    */

    /* System Information */
    char server_name[50];
    int retval = gethostname(server_name, sizeof(server_name));
    if (!retval) {
        data["server_name"] = server_name;
    }

    std::cout << data.dump() << std::endl;
    request << boost::network::body(data.dump());
    std::cout << body(request) << std::endl;
    http::client::response response = this->client->post(request);
    /*for (auto& header : request.headers()) {
        std::cout << header.first << ":" << header.second << std::endl;
    }*/
    std::cout << status(response) << std::endl;
    std::cout << body(response) << std::endl;
}


std::string Sentry::uuid4()
{
    boost::uuids::uuid uuid = generator();
    return to_string(uuid);
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
    Sentry s("https://57a3094fbaf7439f88e8cb879f033386:e83661341e25497eb0558a45090be4d5@app.getsentry.com/82534");
    s.error("testaaaa");
    s.error("wtfaaa");
    return 0;
}
