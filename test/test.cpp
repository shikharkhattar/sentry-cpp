#include "SentryClient.h"

int main(void)
{
    SentryClient s("https://ba3860dad42246dfa851e18205e99232:b4d0113723ea440183cae69d30031251@sentry.io/104441");
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
