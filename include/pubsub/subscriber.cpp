/*#include <string>
#include "subscriber.hpp"
#include "../sms/sms.hpp"

namespace quick {
    namespace pubsub {
        subscriber::subscriber(const sms::sms&smo)
        : smp(smo) {}
        std::string subscriber::read() {
            char* str = static_cast<char*> (smp.attach());
            std::string ret(str);
            smp.detach(static_cast<void*> (str));
            return ret;
        }
    } // namespace pubsub
} // namespace quick
*/