#ifndef QUICK_IPC_SMS_SMS_HPP_
#define QUICK_IPC_SMS_SMS_HPP_

#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
#include <atomic>
using namespace std;

namespace quick {
    namespace sms {
        template<typename T, size_t size>
        class memnode {
            public:
                memnode():head(0),tail(0){
                    for(int i = 0;i<51;i++) {
                        readable[i].store(false,std::memory_order_relaxed);
                    }
                }
                bool push(const T&);
                bool pop(T&);
            private:
                std::atomic<size_t> head;
                std::atomic<size_t> tail;
                std::atomic<bool> readable[size+1];
                std::atomic<bool> used[size+1];
                T array[size+1];
        };
        template<typename T, size_t size>
        class sms {
            public:
                sms(const std::string& topic_name);
                bool write(const T&);
                bool read(T&);
                ~sms();
            private:
                int shmid;
                void* shobj;
        };
        template<typename T, size_t size>
        bool memnode<T,size>::push(const T& ele) {
            size_t current_tail = tail.load(std::memory_order_relaxed);
            size_t next_tail = (current_tail+1)%(size+1);
            if (head.load(std::memory_order_acquire) != next_tail) {
                if (tail.compare_exchange_weak(current_tail, next_tail, std::memory_order_acq_rel)) {
                    bool being_used = false;
                    while (true) if (!readable[current_tail].load(memory_order_acquire)&&used[current_tail].compare_exchange_weak(being_used,true,memory_order_acq_rel)) {
                        array[current_tail] = ele;
                        readable[current_tail].store(true,memory_order_release);
                        used[current_tail].store(false,memory_order_release);
                        return true;
                    }
                }
                else  {
                    return push(ele);
                }
            }
            else {
                std::cout<<"Could not push, no space\n";
                return false;
            }
        }
        template<typename T, size_t size>
        bool memnode<T,size>::pop(T& ele) {
            size_t current_head = head.load(std::memory_order_relaxed);
            if (current_head == tail.load(std::memory_order_acquire)){
                std::cout<<"No element to consume\n";
                return false;
            }
            size_t next_head = (current_head+1)%(size+1);
            if (head.compare_exchange_weak(current_head, next_head, std::memory_order_acq_rel)) {
                bool being_used = false;
                while (true) if (readable[current_head].load(memory_order_acquire)&&used[current_head].compare_exchange_weak(being_used,true,memory_order_acq_rel)) {   
                    T ret = array[current_head];
                    ele = ret;
                    readable[current_head].store(false,memory_order_release);
                    used[current_head].store(false,memory_order_release);
                    return true;
                }
            }
            else {
                return pop(ele);
            }
        }
        template<typename T, size_t size>
        sms<T,size>::sms(const std::string& topic_name) {
            key_t key = ftok(topic_name.c_str(), 65);
            shmid = shmget(key, sizeof(memnode<T,size>), 0666|IPC_CREAT|IPC_EXCL);
            if (shmid == -1) {
                std::cout<<"Shared memory segment already exists.\n";
                shmid = shmget(key, sizeof(memnode<T,size>), 0666);
                shobj = shmat(shmid,(void*)0,0);
            }
            else {
                shobj = shmat(shmid,(void*)0,0);
                new(shobj) memnode<T,size>();
            }
        }
        template<typename T, size_t size>
        bool sms<T,size>::write(const T& ele) {
            return static_cast<memnode<T,size>* >(shobj)->push(ele);
        }
        template<typename T, size_t size>
        bool sms<T,size>::read(T& ele) {
            return static_cast<memnode<T,size>* >(shobj)->pop(ele);
        }
        template<typename T, size_t size>
        sms<T,size>::~sms() {
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
