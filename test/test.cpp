#include "SentryClient.h"

int main(void)
{
    /* Format (as per API):
     * '{PROTOCOL}://{PUBLIC_KEY}:{SECRET_KEY}@{HOST}/{PATH}{PROJECT_ID}'
     * */
    SentryClient s("https://*****:******@sentry.io/****");
    SentryMessage msg_info("info test", "info");
    SentryMessage msg_error("error test 2", "error");
    std::vector<int> x = {0, 1, 2};
    msg_error.add_extra("k1", 0);
    msg_error.add_extra("k2", "asdf");
    msg_error.add_extra("k3", x);

    s.capture(msg_info);
    CAPTURE_MESSAGE(s, msg_error);

    std::cout << "Done" << std::endl;
    return 0;
}
