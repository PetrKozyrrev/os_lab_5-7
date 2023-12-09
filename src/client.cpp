#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <exception>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include <cerrno>
#include <cstring>
#include "zmq.h"
#include "../input/timer.h"

int PORT = 5000;

typedef struct{
    int command;
    int subcommand;
    int node;
} data_for_node;

int main(int argc, char* argv[]){

    if(argc != 3){
        std::cerr << "Error: few arguments" << std::endl;
        return 1;
    }
    
    int CURRENT_NODE = atoi(argv[1]);
    int PARENT_NODE = atoi(argv[2]);

    int rc;

    Timer timer;

    void* context = zmq_ctx_new(); 
    void* subscriber = zmq_socket(context, ZMQ_SUB);

    std::string adress_par = "tcp://localhost:" + std::to_string(PORT + PARENT_NODE);

    rc = zmq_connect(subscriber, adress_par.c_str());
    assert(rc == 0);

    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE,"",0);
    assert(rc == 0);

    void* publisher = zmq_socket(context, ZMQ_PUB);

    std::string adress_cur = "tcp://*:" + std::to_string(PORT + CURRENT_NODE);
    rc = zmq_bind(publisher, adress_cur.c_str());
    if(rc == -1){
        std::cerr << std::strerror(errno) << std::endl;
    }
    assert(rc == 0);

    for(;;){
        zmq_msg_t reply;
	    zmq_msg_init(&reply); 

        zmq_msg_recv (&reply, subscriber, 0);  

        data_for_node* data = (data_for_node*)malloc(zmq_msg_size(&reply));
        memcpy(data, zmq_msg_data(&reply), zmq_msg_size(&reply));

        if (data->command == 1){

            if(data->node != CURRENT_NODE){
                zmq_msg_send(&reply, publisher, 0);
                zmq_msg_close(&reply);

                free(data);
                continue;
            }
            zmq_msg_close(&reply);

            if(data->subcommand == 1){
                timer.start_timer();
                std::cout << "Ok: " << CURRENT_NODE << std::endl;
            }
            else if(data->subcommand == 2){
                timer.stop_timer();
                std::cout <<"Ok: " << CURRENT_NODE << std::endl;
            }
            else{
                std::cout << "Ok: " << CURRENT_NODE << ": " << timer.get_time() << "ms" << std::endl; 
            }
            
        }
        else if(data->command == 2){
            if(data->node == CURRENT_NODE || data->node == -1){

                printf("Ok:%d\n", CURRENT_NODE);

                zmq_msg_close(&reply);
                free(data);
                
                data_for_node* data2 = (data_for_node*)malloc(sizeof(data_for_node));
                data2->command = 2;
                data2->node = -1;

                zmq_msg_t message;
                zmq_msg_init_size(&message, sizeof(data_for_node));

                memcpy(zmq_msg_data(&message), data2, zmq_msg_size(&message));

                zmq_msg_send(&message, publisher, 0);
            
		        zmq_msg_close(&message);

                free(data);
                break;
            } 
            else {
                zmq_msg_send(&reply, publisher, 0);
                zmq_msg_close(&reply);
            }
        }
        else if(data->command == 2){
            if(data->node == CURRENT_NODE){
                zmq_close(publisher);
            }
        }
        free(data);
    }

    zmq_close(subscriber);
    zmq_close(publisher);
	zmq_ctx_destroy(context);

    return 0;
}