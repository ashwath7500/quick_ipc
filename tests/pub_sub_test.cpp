#include <iostream>
#include <thread>
#include "../include/sms/sms.hpp"
#include "../include/pubsub/publisher.hpp"
#include "../include/pubsub/subscriber.hpp"

void pub() {
    quick::sms::sms test_pub_sms("test", 1024);
    quick::pubsub::publisher p(test_pub_sms);
    p.write("Hello world");
}

void sub() {
    quick::sms::sms test_sub_sms("test", 1024);
    quick::pubsub::subscriber s(test_sub_sms);
    std::cout<<s.read();
}

int main() {
    pub();
    pub();
    //sub();
}
