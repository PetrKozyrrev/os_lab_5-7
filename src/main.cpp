#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "zmq.h"
#include "../input/binary_tree.h"

typedef struct{
    int command;
    int subcommand;
    int node;
} data_for_node;


int create_node(int id, int parent)
{
    std::string id_s = std::to_string(id);
    std::string parent_s = std::to_string(parent);

    const char* arg0 = "./client";
    const char* arg1 = id_s.c_str();
    const char* arg2 = parent_s.c_str();
    if(execl("./client", arg0, arg1, arg2, NULL) == -1){
        return -1;
    }
    return 1;
}

int main(){
    
    binary_tree bin_tree(0);
    std::string command;
    int id; 
    
    pid_t child_pid;
    int child_id = 0;
    int parent_id = 0;

    std::vector<int> buzy_node;
    
    void* context = zmq_ctx_new();
    void* publiser = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publiser, "tcp://*:5000");
    if(rc == -1){
        std::cerr << std::strerror(errno) << std::endl;
    }
    assert(rc == 0);
    
    std::cout << "=== Commands === \n";
    std::cout << "create id\n";
    std::cout << "exec id subcommand (start/stop/time)\n";
    std::cout << "kill id\n";
    std::cout << "pingall\n";
    std::cout << "exit\n" << std::endl;

    while(true){
        std::cin >> command;
        
        if(command == "create"){
            std::cin >> id;
            std::cout << std::endl;
            if(bin_tree.find(id)){
                std::cout << "Error: Already exists" << std::endl;
                continue;
            }
            bin_tree.insert(id);
            parent_id = bin_tree.parent(id);

            if(parent_id == -1){
                std::cout << "Error: Parent not found" << std::endl;
                continue;
            }

            child_pid = fork();
            if(child_pid > 0){
                std::cout << "Ok: " << child_pid << std::endl;
            }
            else if(child_pid == 0){
                if(create_node(id, parent_id) == -1){
                    std::cout << "Error: in executing new node" << std::endl;
                    continue;
                }
            }
            else{
                std::cout << "Error: fork error" << std::endl;
                continue;
            }  
        }
        else if(command == "exec"){
            std::string subcommand;
            std::cin >> id >> subcommand;
            std::cout << std::endl;

            if(!bin_tree.find(id)){
                std::cout << "«Error:id: Not found" << std::endl;
                continue;
            }

            data_for_node* data = (data_for_node*)malloc(sizeof(data_for_node));

            data->command = 1;
            if(subcommand == "start"){
                data->subcommand = 1;
                buzy_node.push_back(id);
            }
            else if(subcommand == "stop"){
                data->subcommand = 2;
                buzy_node.erase(std::remove(buzy_node.begin(), buzy_node.end(), id), buzy_node.end());
            }
            else if(subcommand == "time") data->subcommand = 3;
            else{
                std::cout << "Error: Wrong subcommand" << std::endl;
                continue;
            } 
            data->node = id;


            zmq_msg_t message;
            zmq_msg_init_size(&message, sizeof(data_for_node));

            memcpy(zmq_msg_data(&message), data, zmq_msg_size(&message));

            zmq_msg_send(&message, publiser, 0);
            
		    zmq_msg_close(&message);

            free(data);
        }
        else if(command == "pingall"){
            if(buzy_node.size() == 0){
                std::cout << "Ok: -1" << std::endl;
            }
            else{
                std::cout << "Ok: ";
                for(int elem: buzy_node){
                    std::cout << elem << "; ";
                }
                std::cout << std::endl;
            }
        }
        else if(command == "kill"){
            std::cin >> id;
            std::cout << std::endl;
            if(!bin_tree.find(id)){
                std::cout << "Error: doesn`t exist" << std::endl;
                continue;
            }
            buzy_node.erase(std::remove(buzy_node.begin(), buzy_node.end(), id), buzy_node.end());
            bin_tree.erase(id);
            
            data_for_node* data = (data_for_node*)malloc(sizeof(data_for_node));
            data->command = 2;
            data->node = id;

            zmq_msg_t message;
            zmq_msg_init_size(&message, sizeof(data_for_node));

            memcpy(zmq_msg_data(&message), data, zmq_msg_size(&message));

            zmq_msg_send(&message, publiser, 0);
            
		    zmq_msg_close(&message);

            free(data);

        }
        else if(command == "exit"){
            int n = system("killall client");
            break;
        }
    }

    zmq_close(publiser);
	zmq_ctx_destroy(context);

    return 0;
}