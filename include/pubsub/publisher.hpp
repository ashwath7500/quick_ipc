#ifndef QUICK_IPC_PUBSUB_PUBLISHER_HPP_
#define QUICK_IPC_PUBSUB_PUBLISHER_HPP_

#include <string>
#include "../sms/sms.hpp"

namespace quick {
    namespace pubsub {
        class publisher{
            public:
                publisher(sms::sms& smo);
                void write(const std::string& data);
                ~publisher();
            private:
                char* str;
                sms::sms& smp;
        };
    } // namespace pubsub
} // namespace quick

#endif
