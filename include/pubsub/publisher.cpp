/*#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include "publisher.hpp"

namespace quick {
    namespace pubsub {
        publisher::publisher(sms::sms& smo) : smp(smo) {
            str = static_cast<char*> (smo.attach());
        }
        void publisher::write(const std::string& data) {
            smp.write(str, data);
        }
        publisher::~publisher() {
            // std::cout<<"i control order";
            smp.detach(static_cast<void*> (str));
        }
    }
}
*/