// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include "common.h"
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>

// this is the function you should implement
//
// runs Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the execution sequence but trimmed to max_seq_len size
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//



void simulate_rr(
int64_t quantum, 
int64_t max_seq_len,
std::vector<Process> & processes, 
std::vector<int> & seq) 
{

    seq.clear();
    std::vector<int64_t> remaining_bursts(processes.size(), 0); // initialize remaining bursts to 0
    int64_t curr_time = 0;
    std::vector<int> rq; // ready queue
    std::vector<int> jq; // job queue (holds all process indices)

    // initialize job queue with all processes
    for (int64_t i = 0; i <int64_t( processes.size()); i++) {
        jq.push_back(i);
        remaining_bursts[i] = processes[i].burst;  
    }
    bool jump;
    bool all_start_time;
    while (true) {
        jump= false;
        all_start_time =true;
       
        if(jq.empty() && rq.empty()) {
            break;
        }

        if(rq.empty() && !jq.empty()){
            curr_time = processes[jq.front()].arrival;
            if(curr_time != 0) if(seq.back()!= -1) seq.push_back(-1);
            rq.push_back(jq.front());
            
            jq.erase(jq.begin());
            
            continue;

        }
    
        if(processes[rq.front()].start_time == -1 ){
            
            processes[rq.front()].start_time = curr_time;
            
        }
        if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());

         std::vector<int64_t> rq_burst;
        for(auto& n : rq){
            rq_burst.push_back(remaining_bursts[n]);
            if(processes[n].start_time == -1) all_start_time = false;
            if(!seq.empty() )if(int64_t(seq.size()) < max_seq_len ) all_start_time = false;
            
        }
        if(rq.size()== 1 && (processes[rq.front()].start_time != -1)) all_start_time = true;
        

        int64_t min_value = *std::min_element(begin(rq_burst), end(rq_burst));
        int64_t N = min_value % quantum ==0 ? int64_t(min_value/quantum) - 1 : int64_t(min_value/quantum);
        if(N > 0 && all_start_time){
            if(!jq.empty()){
                while(N > 0){
                    if(  processes[jq.front()].arrival > int64_t((curr_time + N*rq.size()*quantum ))) {
                        jump = true;
                        break;
                    } 
                    N/=2;
                }
                

            }
            else{
                jump = true;
            } 
        }
        //std::cout << rq.front() << "::" << N << std::endl;
        if(N <= 0 ) jump = false;

        if(jump ){
           
            curr_time += N*rq.size()*quantum ;
            
            for(auto& index : rq) {
                remaining_bursts[index] -= N*quantum;
                //std::cout << index << remaining_bursts[index] <<"time" << curr_time<< std::endl;
            }
            


           //continue;
        }
        // only JQ is empty
        //     o execute full time slice from the next process in RQ, unless the process would finish
        //     during this time
        //     o if the process did not finish, re-insert the process into RQ
        //     o adjust current time accordingly
        if(jq.empty() && !rq.empty()){
            if(!jump || all_start_time){
                curr_time += quantum;
                remaining_bursts[rq.front()] -= quantum;

            }
            if (remaining_bursts[rq.front()] <= 0) {
                
                curr_time += remaining_bursts[rq.front()];
                remaining_bursts[rq.front()] = 0;
                //std::cout << "interval : " << interval <<"id : "<< rq.front() << "curr_time : " << curr_time << std::endl;
                if(processes[rq.front()].finish_time == -1) processes[rq.front()].finish_time = curr_time;
                //if(rq.front() == 2) std::cout << "here" << std::endl;
            }
            
            if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
            rq.erase(rq.begin());
            
            
            
           
        }

        // both JQ and RQ are not empty
        //     o the implementation here is similar to the case where JQ is empty, but with one important
        //     difference:
        //     o before you re-insert a process back into RQ, you must check to see if any processes
        //     arrived during the quantum, and if they did, put them into RQ before you re-insert the
        //     current process into RQ
        if(!jq.empty() && !rq.empty()){
            //if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());
            if(!jump || all_start_time){
                curr_time += quantum;
                remaining_bursts[rq.front()] -= quantum;

            }
            if (remaining_bursts[rq.front()] <= 0) {
                curr_time += remaining_bursts[rq.front()];
                remaining_bursts[rq.front()] = 0;
                if(processes[rq.front()].finish_time == -1) processes[rq.front()].finish_time = curr_time;
                
                
            }

            for(int64_t j = 0; j < int64_t(jq.size()); j++){
                if(processes[jq[j]].arrival < curr_time){
                    rq.push_back(jq[j]);
                    jq.erase(jq.begin() + j);
                    j--;  
                }
                else{break;}
            }
            if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
            rq.erase(rq.begin());
        } 
    }     
    seq.resize(std::min(max_seq_len, int64_t(seq.size()))); 
}  










/*
void simulate_rr(
int64_t quantum, 
int64_t max_seq_len,
std::vector<Process> & processes, 
std::vector<int> & seq) 
{

    seq.clear();
    std::vector<int64_t> remaining_bursts(processes.size(), 0); // initialize remaining bursts to 0
    int64_t curr_time = 0;
    std::vector<int> rq; // ready queue
    std::vector<int> jq; // job queue (holds all process indices)

    // initialize job queue with all processes
    for (int i = 0; i < processes.size(); i++) {
        jq.push_back(i);
        remaining_bursts[i] = processes[i].burst;  
    }
    bool jump;
    bool all_start_time;
    while (true) {
        jump= false;
        all_start_time =true;
       
        if(jq.empty() && rq.empty()) {
            break;
        }

        if(rq.empty() && !jq.empty()){
            curr_time = processes[jq.front()].arrival;
            if(curr_time != 0) if(seq.back()!= -1) seq.push_back(-1);
            rq.push_back(jq.front());
            
            jq.erase(jq.begin());
            
            continue;

        }
        //if(curr_time == 55)
        if(processes[rq.front()].start_time == -1 ){
            
            processes[rq.front()].start_time = curr_time;
            
        }
        if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());


        std::vector<int64_t> rq_burst;
        for(auto& n : rq){
            rq_burst.push_back(remaining_bursts[n]);
            if(processes[n].start_time == -1) all_start_time = false;
        }
        int64_t min_value = *std::min_element(begin(rq_burst), end(rq_burst));
        
        if(min_value > quantum && all_start_time){
            //std::cout <<jq.front();
            //if( rq.size()*quantum   > processes[jq.front()].arrival )  jump = true;
             //jump = true;
            if(!jq.empty()){
                if(  processes[jq.front()].arrival > curr_time + rq.size()*quantum  ) {
                    jump = true;
                
                //std::cout << rq.size()*quantum  << processes[jq.front()].arrival << std::endl;
                } 
            }
            else{
                jump = true;
            }
            
        }
       
        
        if(jump ){
            curr_time += rq.size()*quantum ;
            for(auto& index : rq) {
                remaining_bursts[index] -= quantum;
                //std::cout << index << remaining_bursts[index] <<"time" << curr_time<< std::endl;
            }

           //continue;
        }

        // only JQ is empty
        //     o execute full time slice from the next process in RQ, unless the process would finish
        //     during this time
        //     o if the process did not finish, re-insert the process into RQ
        //     o adjust current time accordingly
        if(jq.empty() && !rq.empty()){
            if(!jump || all_start_time){
                curr_time += quantum;
                remaining_bursts[rq.front()] -= quantum;

            }
            
            
           
            if (remaining_bursts[rq.front()] <= 0) {
                curr_time += remaining_bursts[rq.front()];
                remaining_bursts[rq.front()] = 0;
                //std::cout << "interval : " << interval <<"id : "<< rq.front() << "curr_time : " << curr_time << std::endl;
                if(processes[rq.front()].finish_time == -1) processes[rq.front()].finish_time = curr_time;
                //if(rq.front() == 2) std::cout << "here" << std::endl;
            }
            
            if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
            rq.erase(rq.begin());
            
            
            
           
        }

        // both JQ and RQ are not empty
        //     o the implementation here is similar to the case where JQ is empty, but with one important
        //     difference:
        //     o before you re-insert a process back into RQ, you must check to see if any processes
        //     arrived during the quantum, and if they did, put them into RQ before you re-insert the
        //     current process into RQ
        if(!jq.empty() && !rq.empty()){
            //if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());
            if(!jump || all_start_time){
                curr_time += quantum;
                remaining_bursts[rq.front()] -= quantum;

            }
                
            
            // curr_time += quantum;
            // remaining_bursts[rq.front()] -= quantum;
            
            if (remaining_bursts[rq.front()] <= 0) {
                curr_time += remaining_bursts[rq.front()];
                remaining_bursts[rq.front()] = 0;
                if(processes[rq.front()].finish_time == -1) processes[rq.front()].finish_time = curr_time;
                
                //if(rq.front() == 2) std::cout << "there "<< std::endl;
            }

            for(int j = 0; j < jq.size(); j++){
                if(processes[jq[j]].arrival < curr_time){
                    rq.push_back(jq[j]);
                    jq.erase(jq.begin() + j);
                    j--;  
                }
                else{break;}
            }
            
            if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
            rq.erase(rq.begin());
            
            
            
        } 
        
    }     
    seq.resize(std::min(max_seq_len, int64_t(seq.size()))); 
}  
*/



// void simulate_rr(
// int64_t quantum, 
// int64_t max_seq_len,
// std::vector<Process> & processes, 
// std::vector<int> & seq) 
// {

//     seq.clear();
//     std::vector<int64_t> remaining_bursts(processes.size(), 0); // initialize remaining bursts to 0
//     int64_t curr_time = 0;
//     std::vector<int> rq; // ready queue
//     std::vector<int> jq; // job queue (holds all process indices)

//     // initialize job queue with all processes
//     for (int i = 0; i < processes.size(); i++) {
//         jq.push_back(i);
//         remaining_bursts[i] = processes[i].burst;  
//     }

//     while (true) {
      
//         if(jq.empty() && rq.empty()) {
//             break;
//         }

//         if(rq.empty() && !jq.empty()){
//             curr_time = processes[jq.front()].arrival;
//             if(curr_time != 0) if(seq.back()!= -1) seq.push_back(-1);
//             rq.push_back(jq.front());
            
//             jq.erase(jq.begin());
            
//             continue;

//         }

//         if(processes[rq.front()].start_time == -1 )processes[rq.front()].start_time = curr_time;
//         if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());



//         // only JQ is empty
//         //     o execute full time slice from the next process in RQ, unless the process would finish
//         //     during this time
//         //     o if the process did not finish, re-insert the process into RQ
//         //     o adjust current time accordingly
//         if(jq.empty() && !rq.empty()){
            
//             curr_time += quantum;
//             remaining_bursts[rq.front()] -= quantum;
        
           
//             if (remaining_bursts[rq.front()] <= 0) {
//                 curr_time += remaining_bursts[rq.front()];
//                 remaining_bursts[rq.front()] = 0;
//                 //std::cout << "interval : " << interval <<"id : "<< rq.front() << "curr_time : " << curr_time << std::endl;
//                 processes[rq.front()].finish_time = curr_time;
//             }
            
//             if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
//             rq.erase(rq.begin());
            
           
//         }

//         // both JQ and RQ are not empty
//         //     o the implementation here is similar to the case where JQ is empty, but with one important
//         //     difference:
//         //     o before you re-insert a process back into RQ, you must check to see if any processes
//         //     arrived during the quantum, and if they did, put them into RQ before you re-insert the
//         //     current process into RQ
//         if(!jq.empty() && !rq.empty()){
//             //if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());
            
//             curr_time += quantum;
//             remaining_bursts[rq.front()] -= quantum;
                
            
//             // curr_time += quantum;
//             // remaining_bursts[rq.front()] -= quantum;
            
//             if (remaining_bursts[rq.front()] <= 0) {
//                 curr_time += remaining_bursts[rq.front()];
//                 remaining_bursts[rq.front()] = 0;
//                 processes[rq.front()].finish_time = curr_time;
//             }

//             for(int j = 0; j < jq.size(); j++){
//                 if(processes[jq[j]].arrival < curr_time){
//                     rq.push_back(jq[j]);
//                     jq.erase(jq.begin() + j);
//                     j--;  
//                 }
//                 else{break;}
//             }

//             if(remaining_bursts[rq.front()] > 0) rq.push_back(rq.front());
            
//             rq.erase(rq.begin());
//         } 
        
//     }     
//     seq.resize(std::min(max_seq_len, int64_t(seq.size()))); 
// }  
/*

void simulate_rr(
int64_t quantum, 
int64_t max_seq_len,
std::vector<Process> & processes, 
std::vector<int> & seq) 
{

    seq.clear();
    std::vector<int64_t> remaining_bursts(processes.size(), 0); // initialize remaining bursts to 0
    int64_t curr_time = 0;
    std::queue<int> rq; // ready queue
    std::queue<int> jq; // job queue (holds all process indices)
    //int cpu = -1;
    //int64_t remaining_slice = quantum;

    // initialize job queue with all processes
    for (int i = 0; i < processes.size(); i++) {
        jq.push(i);
        remaining_bursts[i] = processes[i].burst;
    }

    while (true) {
        std::cout << curr_time << " : " ;
        std::queue<int> rq_ = jq;
        while(!rq_.empty()) {
            std::cout<< rq_.front();
            if(!rq_.empty())rq_.pop();
            else{std::cout << " empty";}
        }
        std::cout << std::endl;
        
        if(jq.empty() && rq.empty()) {
            //if (cpu != -1) processes[cpu].finish_time = curr_time;
            break;
        }


        
        if(rq.empty() && !jq.empty()){
            curr_time = processes[jq.front()].arrival;
            if(curr_time != 0) if(seq.back()!= -1) seq.push_back(-1);
            rq.push(jq.front());
            //std::cout << curr_time  << " : " << jq.front() << std::endl;
            jq.pop();
            //continue;

        }



        // only JQ is empty
        //     o execute full time slice from the next process in RQ, unless the process would finish
        //     during this time
        //     o if the process did not finish, re-insert the process into RQ
        //     o adjust current time accordingly
        if(jq.empty() && !rq.empty()){
            if(processes[rq.front()].start_time == -1)processes[rq.front()].start_time = curr_time;
            if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());
            for(int i = 0; i < quantum; i++){
                remaining_bursts[rq.front()]--;
                curr_time++;
                if(remaining_bursts[rq.front()] == 0){
                    processes[rq.front()].finish_time = curr_time;
                    
                    break;
                }
            }
            if(remaining_bursts[rq.front()] > 0) rq.push(rq.front());
            rq.pop();
           // continue;
        }

        // both JQ and RQ are not empty
        //     o the implementation here is similar to the case where JQ is empty, but with one important
        //     difference:
        //     o before you re-insert a process back into RQ, you must check to see if any processes
        //     arrived during the quantum, and if they did, put them into RQ before you re-insert the
        //     current process into RQ
        if(!jq.empty() && !rq.empty()){
            if(processes[rq.front()].start_time == -1 )processes[rq.front()].start_time = curr_time;
            std::queue<int> jq_copy; 
            for(int i = curr_time; i < (curr_time + quantum); i++){
                jq_copy = jq;
                while(!jq_copy.empty()){
                    if(processes[jq_copy.front()].arrival == i){
                        rq.push(jq.front());
                        jq.pop();
                    }
                    jq_copy.pop();
                }
            }
            
            if(seq.back()!= processes[rq.front()].id) seq.push_back(rq.front());
            for(int i = 0; i < quantum; i++){
                remaining_bursts[rq.front()]--;
                curr_time++;
                if(remaining_bursts[rq.front()] == 0){
                    processes[rq.front()].finish_time = curr_time;
                    break;
                }
            }
            if(remaining_bursts[rq.front()] > 0) rq.push(rq.front());
            rq.pop();
           // continue;
        
        }
        
        
    }       
    

}     

*/         


// if(curr_time != 0){
//                 if(!seq.empty()){if(seq.back()!= -1) seq.push_back(-1);}
//                 else seq.push_back(-1);
//             }