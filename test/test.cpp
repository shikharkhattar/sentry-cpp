#include <iostream>
#include "../include/sentry.hpp"

int main(void)
{
    /* Format (as per API):
     * '{PROTOCOL}://{PUBLIC_KEY}:{SECRET_KEY}@{HOST}/{PATH}{PROJECT_ID}'
     * */
    SentryClient s("https://e18205e9b4d0113ae69d30031251@sentry.io/104");

    /* Level can be one of the following (as per Sentry API)
     * "info"
     * "debug"
     * "warning"
     * "error"
     * "fatal"
     */
    SentryMessage msg_info("info test", "info");
    SentryMessage msg_error("error test", "error");
    SentryMessage msg_debug("debug test", "debug");
    SentryMessage msg_fatal("fatal test", "fatal");
    SentryMessage msg_warn("warning test", "warning");

    std::vector<int> x = {0, 1, 2};
    msg_error.add_extra("k1", 0);
    msg_error.add_extra("k2", "asdf");
    msg_error.add_extra("k3", x);

    try {
        s.capture(msg_info);
        s.capture(msg_warn);
        CAPTURE_MESSAGE(s, msg_error);
        CAPTURE_MESSAGE(s, msg_debug);
        CAPTURE_MESSAGE(s, msg_fatal);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << "Done" << std::endl;
    return 0;
}
