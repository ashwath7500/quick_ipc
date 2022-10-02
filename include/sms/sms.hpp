#ifndef QUICK_IPC_SMS_SMS_HPP_
#define QUICK_IPC_SMS_SMS_HPP_

#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <atomic>

namespace quick {
    namespace sms {
        template<typename T>
        class memnode {
            public:
                memnode():head(0),tail(0){
                    for(int i = 0;i<51;i++) {
                        readable[i].store(false,std::memory_order_relaxed);
                    }
                }
                void push(const T&);
                T pop();
            private:
                std::atomic<size_t> head;
                std::atomic<size_t> tail;
                std::atomic<bool> readable[51];
                T array[51];
        };
        template<typename T>
        class sms {
            public:
                sms(const std::string& topic_name);
                void write(const T&);
                T read();
                ~sms();
            private:
                int shmid;
                void* shobj;
        };
        template<typename T>
        void memnode<T>::push(const T& ele) {
            size_t current_tail = tail.load(std::memory_order_relaxed);
            size_t next_tail = (current_tail+1)%(51);
            if (head.load(std::memory_order_acquire) != next_tail) {
                if (tail.compare_exchange_weak(current_tail, next_tail, std::memory_order_acq_rel)) {
                    bool readability = readable[current_tail];
                    while (true) if (readable[current_tail].compare_exchange_weak(readability, true, std::memory_order_acq_rel)) {
                        array[current_tail] = ele;
                        return;
                    }
                }
                else  {
                    push(ele);
                    return;
                }
            }
            else {
                std::cout<<"Could not push, no space\n";
            }
        }
        template<typename T>
        T memnode<T>::pop() {
            size_t current_head = head.load(std::memory_order_relaxed);
            if (current_head == tail.load(std::memory_order_acquire)){
                std::cout<<"No element to consume\n";
                return array[head];
            }
            size_t next_head = (current_head+1)%(51);
            if (head.compare_exchange_weak(current_head, next_head, std::memory_order_acq_rel)) {
                bool readability = readable[current_head];
                while (true) if (readable[current_head].compare_exchange_weak(readability, false, std::memory_order_acq_rel)) {   
                    T ret = array[current_head];
                    return ret;
                }
            }
            else {
                return pop();
            }
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
            static_cast<memnode<T>* >(shobj)->push(ele);
        }
        template<typename T>
        T sms<T>::read() {
            return static_cast<memnode<T>* >(shobj)->pop();
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

#endif
