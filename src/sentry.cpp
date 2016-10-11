#include "sentry.h"

/* TODO:
 * Exception handling
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


bool Sentry::capture(SentryMessage& msg)
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

    data["extra"] = msg.getExtra();
    data["extra"]["backtrace"] = this->generateStackTrace();

    /* Send the POST request */
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


int main(void)
{
    Sentry s("https://ba3860dad42246dfa851e18205e99232:b4d0113723ea440183cae69d30031251@sentry.io/104441");
    SentryMessage msg_info("info test", "info");
    SentryMessage msg_error("error test 2", "error");
    std::vector<int> x = {0, 1, 2};
    msg_error.add_extra("k1", 0);
    msg_error.add_extra("k2", "asdf");
    msg_error.add_extra("k3", x);

    s.capture(msg_info);
    s.capture(msg_error);

    std::cout << "Done" << std::endl;
    return 0;
}
