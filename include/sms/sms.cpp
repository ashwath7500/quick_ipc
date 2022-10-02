/*#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <string>
#include "sms.hpp"

namespace quick {
    namespace sms {
        template<typename T>
        void memnode<T>::push(const T& ele) {
            int next_tail = (tail.load(std::memory_order_relaxed)+1)%(51);
            if (head.load(std::memory_order_acquire) != next_tail) {
                array[tail.load(std::memory_order_relaxed)] = ele;
                tail.store(next_tail,std::memory_order_release);
            }
            else {
                std::cout<<"Could not push, no space\n";
            }
        }
        template<typename T>
        T memnode<T>::pop() {
            if (head.load(std::memory_order_relaxed) == tail.load(std::memory_order_acquire)){
                std::cout<<"No element to consume\n";
                return array[head];
            }
            T ret = array[head.load(std::memory_order_relaxed)];
            int next_head = (head.load(std::memory_order_relaxed)+1)%(51);
            head.store(next_head,std::memory_order_release);
            return ret;
        }
        template<typename T>
        sms<T>::sms(const std::string& topic_name) {
            key_t key = ftok(topic_name.c_str(), 65);
            shmid = shmget(key, sizeof(memnode<T>), 0666|IPC_CREAT|IPC_EXCL);
            if (shmid == -1) {
                std::cout<<"Shared memory segment already exists.\n";
                shmid = shmget(key, sizeof(memnode<T>), 0666);
                shobj = shmat(shmid,(void*)0,0);
            }
            else {
                shobj = shmat(shmid,(void*)0,0);
                new(shobj) memnode<T>();
            }
        }
        template<typename T>
        void sms<T>::write(const T& ele) {
            static_cast<memnode<T> >(shobj)->push(ele);
        }
        template<typename T>
        T sms<T>::read() {
            return static_cast<memnode<T> >(shobj)->pop();
        }
        template<typename T>
        sms<T>::~sms() {
            shmdt(shobj);
            shmid_ds buf;
            shmctl(shmid, IPC_STAT, &buf);
            std::cout<<"number of shmed: "<<buf.shm_nattch<<"\n";
            if (buf.shm_nattch ==  0)
                shmctl(shmid, IPC_RMID, NULL);
        }
    } // namespace sms
} // namespace quick
*/