#include <iostream>
#include <thread>
#include "../include/sms/sms.hpp"

quick::sms::sms<std::string> test_sms("test");

void pub() {
    int i = 25;
    while(i--)
    test_sms.write("hello world\n");
}

void sub() {
    int i = 40;
    while(i--)
    std::cout<<test_sms.read();
}

int main() {
    std::thread thread2(sub);
    std::thread thread1(pub);
    thread1.join();
    thread2.join();
}
