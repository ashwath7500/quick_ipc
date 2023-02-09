#include <bits/stdc++.h>
#include "../include/sms/sms.hpp"

template<typename T, size_t siz>
class mpmc_queue{
public:
    mpmc_queue() : head_(0),tail_(0) {
        for (size_t i = 0;i<=siz;i++) readable_[i].store(false,memory_order_relaxed);
        for (size_t i = 0;i<=siz;i++) used_[i].store(false,memory_order_relaxed);
    }
    bool write(const T& ele) {
        while(true) {
            int current_head = head_.load(memory_order_acquire);
            int current_tail = tail_.load(memory_order_acquire);
            int next_tail = (current_tail+1)%(siz+1);
            if (next_tail != current_head) {
                if (tail_.compare_exchange_weak(current_tail,next_tail,memory_order_acq_rel)) {
                    while(true) {
                        bool being_used = false;
                        if (!readable_[current_tail].load(memory_order_acquire)&&used_[current_tail].compare_exchange_weak(being_used,true,memory_order_acq_rel)) {
                            arr_[current_tail] = ele;
                            readable_[current_tail].store(true,memory_order_release);
                            used_[current_tail].store(false,memory_order_release);
                            return true;
                        }
                    }
                }
            }
            else return false;
        }
    }
    bool read(T& ele) {
        while(true) {
            int current_head = head_.load(memory_order_acquire);
            int current_tail = tail_.load(memory_order_acquire);
            int next_head = (current_head+1)%(siz+1);
            if (current_head != current_tail) {
                if (head_.compare_exchange_weak(current_head,next_head,memory_order_acq_rel)) {
                    while(true) {
                        bool being_used = false;
                        if (readable_[current_head].load(memory_order_release)&&used_[current_head].compare_exchange_weak(being_used,true,memory_order_acq_rel)) {
                            ele = arr_[current_head];
                            readable_[current_head].store(false,memory_order_release);
                            used_[current_head].store(false,memory_order_release);
                            return true;
                        }
                    }
                }
            }
            else return false;
        }
    }
private:
    atomic<int> head_,tail_;
    T arr_[siz+1];
    atomic<bool> readable_[siz+1];
    atomic<bool> used_[siz+1];
};

void Test( std::string name )
{
    const int NUM_PRODUCERS = 64;
    const int NUM_CONSUMERS = 2;
    const int NUM_ITERATIONS = 512;
    bool table[NUM_PRODUCERS*NUM_ITERATIONS];
    memset(table, 0, NUM_PRODUCERS*NUM_ITERATIONS*sizeof(bool));
    //quick::sms::sms<int,NUM_PRODUCERS*NUM_ITERATIONS> queue(name);
    mpmc_queue<int,NUM_PRODUCERS*NUM_ITERATIONS> queue;
    std::vector<std::thread> threads(NUM_PRODUCERS+NUM_CONSUMERS);

    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    std::atomic<int> pop_count (NUM_PRODUCERS * NUM_ITERATIONS);
    std::atomic<int> push_count (0);

    for( int thread_id = 0; thread_id < NUM_PRODUCERS; ++thread_id )
    {
        threads[thread_id] = std::thread([&queue,thread_id,&push_count]()
                                 {
                                     int base = thread_id * NUM_ITERATIONS;

                                     for( int i = 0; i < NUM_ITERATIONS; ++i )
                                     {
                                         while( !queue.write( base + i ) ){};
                                         push_count.fetch_add(1);
                                     }
                                 });
    }

    for( int thread_id = 0; thread_id < ( NUM_CONSUMERS ); ++thread_id )
    {
        threads[thread_id+NUM_PRODUCERS] = std::thread([&]()
                                         {
                                             int v;

                                             while( pop_count.load() > 0 )
                                             {
                                                 if( queue.read( v ) )
                                                 {
                                                     if( table[v] )
                                                     {
                                                         std::cout << v << " already set" << std::endl;
                                                     }
                                                     table[v] = true;
                                                     pop_count.fetch_sub(1);
                                                 }
                                             }
                                         });

    }

    for( int i = 0; i < ( NUM_PRODUCERS + NUM_CONSUMERS ); ++i )
    {
        threads[i].join();
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << name << " " << duration.count() << std::endl;

    bool result = true;
    for( int i = 0; i < NUM_PRODUCERS * NUM_ITERATIONS; ++i )
    {
        if( !table[i] )
        {
            std::cout << "failed at " << i << std::endl;
            result = false;
        }
    }
    std::cout << name << " " << ( result? "success" : "fail" ) << std::endl;
}

int main() {
    Test("testit");
}
