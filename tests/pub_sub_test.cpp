#include <bits/stdc++.h>
#include "../include/sms/sms.hpp"

void Test( std::string name )
{
    const int NUM_PRODUCERS = 64;
    const int NUM_CONSUMERS = 2;
    const int NUM_ITERATIONS = 512;
    bool table[NUM_PRODUCERS*NUM_ITERATIONS];
    memset(table, 0, NUM_PRODUCERS*NUM_ITERATIONS*sizeof(bool));
    quick::sms::sms<int,NUM_PRODUCERS*NUM_ITERATIONS> queue(name);
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
