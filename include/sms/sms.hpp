#ifndef QUICK_IPC_SMS_SMS_HPP_
#define QUICK_IPC_SMS_SMS_HPP_

#include <string>
#include <sys/types.h>

namespace quick {
    namespace sms {
        class sms {
            public:
                sms(const std::string& topic_name,
                    int size);
                void* attach() const;
                void detach(void* shm) const;
                void write(char* str, std::string data);
                ~sms();
            private:
                key_t key;
                int shmid;
                int capacity;
                int end;
        };
    } // namespace sms
} // namespace quick

#endif
