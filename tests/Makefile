pubsub = ../include/pubsub
sms = ../include/sms
test_out = ../test_out

all: $(test_out)/pub_sub_test

$(test_out)/publisher.o : $(pubsub)/publisher.cpp $(pubsub)/publisher.hpp $(test_out)/sms.o
	g++ -c $(pubsub)/publisher.cpp -o $(test_out)/publisher.o

$(test_out)/subscriber.o : $(pubsub)/subscriber.cpp $(pubsub)/subscriber.hpp $(test_out)/sms.o
	g++ -c $(pubsub)/subscriber.cpp -o $(test_out)/subscriber.o

$(test_out)/sms.o : $(sms)/sms.cpp $(sms)/sms.hpp
	g++ -c $(sms)/sms.cpp -o $(test_out)/sms.o

$(test_out)/pub_sub_test : pub_sub_test.cpp $(test_out)/sms.o $(test_out)/publisher.o $(test_out)/subscriber.o
	g++ -o $(test_out)/pub_sub_test pub_sub_test.cpp $(test_out)/sms.o
