#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <string>
#include "sms.hpp"

namespace quick {
    namespace sms {
        sms::sms(const std::string& topic_name, int size): capacity(size),end(0) {
            key = ftok(topic_name.c_str(), 65);
            shmid  = shmget(key, size, 0666|IPC_CREAT);
        }
        void* sms::attach() const {
            return shmat(shmid,(void*)0,0);
        }
        void sms::detach(void* shm) const {
            shmdt(shm);
        }
        void sms::write(char* str, std::string data) {
            if (capacity<end+data.size()) {
                std::cout<<"No space to add this string";
                return;
            }
            std::strcpy(str+end,data.c_str());
            end+=data.size();
        }
        sms::~sms() {
            shmid_ds* buf;
            shmctl(shmid, IPC_STAT, buf);
            std::cout<<"number of shmed: "<<buf->shm_nattch<<"\n";
            if (buf->shm_nattch ==  0)
                shmctl(shmid, IPC_RMID, NULL);
        }
    } // namespace sms
} // namespace quick
