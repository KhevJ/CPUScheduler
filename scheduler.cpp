

#include "scheduler.h"
#include "common.h"
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>





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








 
