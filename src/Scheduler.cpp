#include "../include/Scheduler.h"

#define RAND(min, max)  ((int)((double) rand() / (RAND_MAX) * ((max) - (min))) + (int)(min))
#define NOR_RAND()      ((double)rand() / ((RAND_MAX)))




uint16_t Scheduler::GetSolution() {
    return station;
}

double Scheduler::GetCalculateTime() {
    return calculate_time;
}

void Scheduler::PreProcessing(mJob job) {
    mjob = job;
    // data structure init
    
    leaf_node = (bool *)malloc(mjob.taskNum * sizeof(bool));
    finish_node = (bool *)malloc(mjob.taskNum * sizeof(bool));
    root_node = (bool *)malloc(mjob.taskNum * sizeof(bool));
    for (int i = 0; i < mjob.taskNum; i++) {
        leaf_node[i] = true;
        finish_node[i] = false;
        root_node[i] = false;
        if (mjob.task_list[i].executor == 'e') {
            alternative_node.push_back(i + 1);
        }
    }
    // form tree structure
    for (int i = 0; i < mjob.taskNum; i++) {
        if (mjob.task_list[i].and_next.size() == 0 && mjob.task_list[i].or_next.size() == 0) {
            root_node[i] = true;
        }
        else {
            for(int j = 0; j < mjob.task_list[i].and_next.size(); j++){
                leaf_node[mjob.task_list[i].and_next[j] - 1] = false;
                mjob.task_list[mjob.task_list[i].and_next[j] - 1].and_prev.push_back(i + 1);
                if(PRINT) cout << "[INFO]: " << (int)mjob.task_list[mjob.task_list[i].and_next[j] - 1].num << " node and prev " << i+1 << endl;
            }
            for(int j = 0; j < mjob.task_list[i].or_next.size(); j++){
                leaf_node[mjob.task_list[i].or_next[j] - 1] = false;
                mjob.task_list[mjob.task_list[i].or_next[j] - 1].or_prev.push_back(i + 1);
                if(PRINT) cout << "[INFO]: " << (int)mjob.task_list[mjob.task_list[i].or_next[j] - 1].num << " node or prev " << i+1 << endl;
            }
        }
    }
    for (int i = 0; i < mjob.taskNum; i++) {
        if (leaf_node[i]) {
            if(PRINT) cout << "[INFO]: leaf node " << i + 1 << endl;
        }        
        if (root_node[i])
            if(PRINT) cout << "[INFO]: root node " << i + 1 << endl;  
            
    }
}


vector<vector<uint16_t>> Scheduler::Sequence_Generate(){
    vector<vector<uint16_t>> sequence;
    int rand_num;
    vector<uint16_t>::iterator it;
    
    for (int i = 0; i < mjob.taskNum; i++) {
        if (leaf_node[i]) {
            avaliable_node.push_back(i + 1);
        }
        finish_node[i] = false;
    }
    for (int i = 0; i < 2; i++) {
        sequence.push_back(vector<uint16_t>());
    }

    // Start random generate sequence
    while (avaliable_node.size() > 0){

        rand_num = RAND(0, avaliable_node.size()); // Select a random node in avaliable list
        if (rand_num == 1 && avaliable_node.size() == 0) rand_num = 0;
        
        if (PRINT) cout << "[INFO]: Chosen node " << (int)avaliable_node[rand_num] << endl;
        sequence[1].push_back(avaliable_node[rand_num]);
        finish_node[avaliable_node[rand_num] - 1] = true;
        
        // Add next posible node to the list
        for (int i = 0; i < mjob.task_list[avaliable_node[rand_num] - 1].or_next.size(); i++){
            uint16_t search_node = mjob.task_list[avaliable_node[rand_num] - 1].or_next[i];
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1]){
                avaliable_node.push_back(search_node);
            }
        }
        for (int i = 0; i < mjob.task_list[avaliable_node[rand_num] - 1].and_next.size(); i++){
            bool and_done = true;
            uint16_t search_node = mjob.task_list[avaliable_node[rand_num] - 1].and_next[i];
            if(PRINT) cout << "[INFO]: prev node" << (int)mjob.task_list[search_node - 1].and_prev.size() << endl;
            for (int j = 0; j < mjob.task_list[search_node - 1].and_prev.size(); j++) {
                if(PRINT) cout << "[INFO]: prev and node[" << (int)mjob.task_list[search_node - 1].and_prev[j] << "]finished." << endl;
                if (!finish_node[mjob.task_list[search_node - 1].and_prev[j] - 1]) 
                    and_done = false;
            }   
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1] && and_done){
                avaliable_node.push_back(search_node);
                if (PRINT) cout << "[INFO]: " << (int)search_node << " added." << endl;
            }
        }
        avaliable_node.erase(avaliable_node.begin() + rand_num);      
    }

    for (int i = 0; i < mjob.taskNum; i++) {
        if (mjob.task_list[i].executor == 'r')
            sequence[0].push_back(0);
        else if (mjob.task_list[i].executor == 'h')
            sequence[0].push_back(1);
        else if (mjob.task_list[i].executor == 'b')
            sequence[0].push_back(2);
        else{
            
            sequence[0].push_back(0);
        }
    }

    if (PRINT) cout << "[INFO]: output sequence "; 
    if (PRINT) PRINT_SEQUENCE(sequence);
    
    return sequence;
}


uint16_t Scheduler::Fitness(vector<vector<uint16_t>> sequence){
    // Calculate the fitness of the sequence
    uint16_t r_time = 0;
    uint16_t h_time = 0;


    for (int i = 0; i < sequence[1].size(); i++) {
        int idx = sequence[1][i] - 1;
        if (sequence[0][idx] == 0){
            r_time += mjob.task_list[idx].r_time;
        }
        else if (sequence[0][idx] == 1){
            h_time += mjob.task_list[idx].h_time;
        }
        else if (sequence[0][idx] == 2){
            uint16_t common_time = max(r_time, h_time);
            r_time = mjob.task_list[idx].r_time + common_time;
            h_time = mjob.task_list[idx].h_time + common_time;
        }
    }
    return max(r_time, h_time);
}


bool Scheduler::Valid(vector<uint16_t> sequence){
    if (PRINT) cout << "[INFO]: Varafied new sequence";
    for (int i = 0; i < sequence.size(); i++) {
        if (PRINT) cout << " " << (int)sequence[i];
        if (sequence[i] == 0)
            return false;
        finish_node[i] = false;
    }
    if (PRINT) cout << endl;
    
    finish_node[sequence[0] - 1] = true;
    bool legal_sequence = true;
    bool valid;

    if (!leaf_node[sequence[0] - 1]){
        if (PRINT) cout << "[INFO]: the sequence valid : 0" << endl;
        return false;
    }
    for (int i = 1; i < sequence.size(); i++) {
        // Check -or connection
        valid = false; 
        for (int j = 0; j < mjob.task_list[sequence[i] - 1].or_prev.size(); j++){
            if (finish_node[mjob.task_list[sequence[i] - 1].or_prev[j] - 1]) 
                valid = true; // Need at least one pre node finished
        }
        if (mjob.task_list[sequence[i] - 1].or_prev.size() != 0 && !valid) {
            legal_sequence = false;
            if(PRINT) cout << "[INFO]: invalid in node [or] " << (int)sequence[i] << endl;
            break;
        }
        // Check -and connection
        valid = true;
        for (int j = 0; j < mjob.task_list[sequence[i] - 1].and_prev.size(); j++){
            if (!finish_node[mjob.task_list[sequence[i] - 1].and_prev[j] - 1])
                valid = false; // Need all pre node finished
        }
        if (mjob.task_list[sequence[i] - 1].and_prev.size() != 0 && !valid) {
            legal_sequence = false;
            if(PRINT) cout << "[INFO]: invalid in node [and] " << (int)sequence[i] << endl;
            break;
        }
        finish_node[sequence[i] - 1] = true;
    }
    
    if (PRINT) cout << "[INFO]: the sequence valid :" << legal_sequence << endl;
    return legal_sequence;
}


void Scheduler::PRINT_SEQUENCE(vector<vector<uint16_t>> sequence) {
    cout << "[Operator] ";  
    for (int i = 0; i < sequence[0].size(); i++){
        cout << sequence[0][i] << " ";
    }
    cout << endl;
    cout << "[Sequence] ";
    for (int i = 0; i < sequence[1].size(); i++){
        cout << sequence[1][i] << " ";
    }
    cout << endl;
}
