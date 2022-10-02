/*#ifndef QUICK_IPC_PUBSUB_SUBSCRIBER_HPP_
#define QUICK_IPC_PUBSUB_SUBSCRIBER_HPP_

#include <string>
#include "../sms/sms.hpp"

namespace quick {
    namespace pubsub {
        class subscriber {
            public:
                subscriber(const sms::sms& smo);
                std::string read();
            private:
                const sms::sms& smp;
        };
    } // namespace pubsub
} // namespace quick

#endif
*/