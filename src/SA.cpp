#include "../include/SA.h"


SA *SA::inst_ = nullptr;

SA *SA::GetSA()
{
    if (inst_ == nullptr)
        inst_ = new SA();
    return inst_;
}

SA::SA(){
    srand(time(NULL));
}

void SA::PreProcessing(mJob job){
    Scheduler::PreProcessing(job);
}

uint16_t SA::GetSolution(){
    return Scheduler::GetSolution();
}

double SA::GetCalculateTime(){
    return Scheduler::GetCalculateTime();
}

void SA::RunAlgorithm(){
    
    auto start = high_resolution_clock::now();
    
    // Parameter setting
    vector<vector<uint16_t>> first_sequence = Sequence_Generate();
    // PRINT_SEQUENCE(first_sequence);
    vector<vector<uint16_t>> best_sequence = first_sequence;
    vector<vector<uint16_t>> current_sequence = first_sequence;
    vector<vector<uint16_t>> new_sequence = first_sequence;
    uint16_t min_fitness = Fitness(first_sequence);
    uint16_t current_fitness = Fitness(first_sequence);
    uint16_t new_fitness = Fitness(first_sequence);
    double T = T_init;
    // Flag
    uint8_t valid_cnt = 0;

    // Algorithm start
    while (T > T_min) {
        for (int it = 0; it < Max_iter; it++) {
            valid_cnt = 0;
            while(true){
                new_sequence = Neighbor(current_sequence);
                if (Valid(new_sequence[1]))
                    break;
                valid_cnt++;
                if (valid_cnt > 10){
                    // cout << "\033[1;33m[WARNING]: Can't find suitable sequence.\033[0m" << endl;
                    new_sequence = Sequence_Generate();
                    break;
                }       
            }
            if (new_sequence[1].size() != mjob.taskNum){
                cout << "\033[1;31m[ERROR]: Generate wrong sequence\033[0m" << endl;
                return;
            }
            // select a worse solusion 
            if (new_fitness < current_fitness) {
                current_sequence = new_sequence;
                if (new_fitness < min_fitness) {
                    best_sequence = new_sequence;
                    min_fitness = new_fitness;
                }
            }
            else{
                if (exp((new_fitness - current_fitness) / T) >= (double)NOR_RAND()) {
                    current_sequence = new_sequence;
                }      
            }
         }
        T *= Alpha;
        if (PRINT) cout << "T = " << (double)T << endl;
    }
    if (PRINT) cout << "[INFO]: Best value " << min_fitness << endl;
    station = min_fitness;
    if (PRINT) PRINT_SEQUENCE(best_sequence);


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if (PRINT) cout << "[INFO]: SA function spand " << (double)duration.count() / (double)1000000 << " s." << endl;
    calculate_time = (double)duration.count() / (double)1000000;
}

/**
 * There are four ways to find neighbor solution, insert, swap, kick and block moves,
 * choose spawn strategy in a equal properbility.
*/
vector<vector<uint16_t>> SA::Neighbor(vector<vector<uint16_t>> sequence){
    vector<uint16_t> new_sequence;
    vector<uint16_t> exe_sequence;
    vector<uint16_t> test_sequence;
    for (int i = 0; i < sequence[1].size(); i++) {
        new_sequence.push_back(sequence[1][i]);
        test_sequence.push_back(sequence[1][i]);
        exe_sequence.push_back(sequence[0][i]);
    }
    double rand_way = NOR_RAND();

    if(PRINT) cout << "[INFO]: origin sequence ";
    

    if (rand_way < 0.2) {
        // Addopt insert polocy
        if(PRINT) cout << "[INFO]: -----INSERT polocy" << endl;
        int rand_pick = RAND(1, test_sequence.size());
        uint16_t pick_task = test_sequence[rand_pick];
        if(PRINT) cout << "[INFO]: Select " << rand_pick << endl;
        test_sequence.erase(test_sequence.begin() + rand_pick);
        int rand_insert = RAND(0, test_sequence.size());
        test_sequence.emplace(test_sequence.begin() + rand_insert, pick_task);

        // If invalid
        if (!Valid(test_sequence)){

            if(PRINT) cout << "\033[1;33m[WARNING]: Invalid sequence, insert in other way \033[0m";
            
            
            vector<uint16_t> candadate_idx;

            // last pre-and node
            uint16_t last_and = new_sequence.size();
            bool find = false;
            for (int i = new_sequence.size() - 1; i >=0; i--) {
                for (int j = 0; j < mjob.task_list[pick_task - 1].and_prev.size(); j++){
                    if (mjob.task_list[pick_task - 1].and_prev[j] == new_sequence[i]) {
                        last_and = i;
                        candadate_idx.push_back(last_and);
                        if(PRINT) cout << "[INFO]: find last and " << i << endl;;
                        find = true;
                        break;
                    }
                }
                if (find)   break;
            }
            if (!find)
                last_and = 0;

            find = false;
            for (int i = new_sequence.size() - 1; i >= last_and; i--){
                // Find preceding node, and insert before it
                for (int j = 0; j < mjob.task_list[pick_task - 1].or_prev.size(); j++){
                    if (mjob.task_list[pick_task - 1].or_prev[j] == new_sequence[i]){
                        candadate_idx.push_back(i);
                        if(PRINT) cout << "[INFO]: find last or " << i << endl;
                        find = true;
                        break;
                    }
                }
            }

            // find exchange index
            if (candadate_idx.empty()){
                // cout << "\033[1;31m[ERROR]: No candadate value!\033[0m" << endl;
                new_sequence.erase(new_sequence.begin() + rand_pick);
                new_sequence.emplace(new_sequence.begin(), pick_task);
            }
            else {  
                int new_insert = RAND(0, candadate_idx.size());
                int idx = candadate_idx[new_insert] + 1;
                if(PRINT) cout << "[INFO]: select place " << idx << endl;
                if (rand_pick < idx)
                    idx--;
                new_sequence.erase(new_sequence.begin() + rand_pick);
                new_sequence.emplace(new_sequence.begin() + idx, pick_task);
            }
        }
        else {
            new_sequence = test_sequence;
        }
    }
    else if (rand_way >=0.2 && rand_way < 0.4) {
        // Addopt swap polocy
        if(PRINT) cout << "[INFO]: -----SWAP polocy" << endl;
        int rand_pick = RAND(0, new_sequence.size());
        int rand_swap = RAND(0, new_sequence.size());

        uint16_t tmp = new_sequence[rand_pick];
        new_sequence[rand_pick] = new_sequence[rand_swap];
        new_sequence[rand_swap] = tmp;
    }
    else if (rand_way >=0.4 && rand_way < 0.6) {
        // Addopt kick polocy
        if(PRINT) cout << "[INFO]: -----KICK polocy" << endl;
        vector<uint16_t> kick_idx;
        int rand_pick;
        while(kick_idx.size() < 3){
            rand_pick = RAND(0, new_sequence.size());
            if (find(kick_idx.begin(), kick_idx.end(), rand_pick) == kick_idx.end())
                kick_idx.push_back(rand_pick);
        }
        sort(kick_idx.begin(), kick_idx.end());

        uint16_t tmp1 = new_sequence[kick_idx[0]];
        uint16_t tmp2 = new_sequence[kick_idx[1]];
        new_sequence.erase(new_sequence.begin() + kick_idx[1]);
        new_sequence.emplace(new_sequence.begin() + kick_idx[2], tmp2);
        new_sequence.erase(new_sequence.begin() + kick_idx[0]);
        new_sequence.emplace(new_sequence.begin() + kick_idx[1], tmp1);
    }
    else if (rand_way >= 0.6 && rand_way < 0.8) {
        // Addopt block move polocy
        if(PRINT) cout << "[INFO]: -----BLOCK MOVE polocy" << endl;
        int block_size = (int)(new_sequence.size() * 0.2);
        int rand_window = RAND(0, new_sequence.size() - block_size);
        if(PRINT) cout << "[INFO]: window " << rand_window << endl;
        vector<uint16_t> block;
        uint16_t tmp;
        for (int i = 0; i < block_size; i++) {
            tmp = new_sequence[rand_window];
            block.push_back(tmp);
            new_sequence.erase(new_sequence.begin() + rand_window);
        }
        rand_window = RAND(0, new_sequence.size());
        uint16_t block_val;
        for (int i = 0; i < block_size; i++) {
            block_val = block.back();
            block.pop_back();
            new_sequence.emplace(new_sequence.begin() + rand_window, block_val);
        }
    }
    else if (rand_way >= 0.8 && rand_way < 1.0) {
        // Addopt change polocy
        if (PRINT)cout << "[INFO]: -----CHANGE polocy" << endl;

        // random pick a task from alternative_node
        int rand_pick = RAND(0, alternative_node.size());
        uint16_t pick_task = alternative_node[rand_pick];
        if (PRINT)cout << "[INFO]: pick task " << pick_task << endl;
        // find index of pick_task in new_sequence
        int idx = 0;
        for (int i = 0; i < new_sequence.size(); i++) {
            if (new_sequence[i] == pick_task) {
                idx = i;
                break;
            }
        }
        if (mjob.task_list[pick_task - 1].executor == 'h') {
            exe_sequence[idx] = 0;
        }
        else {
            exe_sequence[idx] = 1;
        }
    }
    else {
        cout << "\033[1;31m[ERROR]: No polocy!\033[0m" << endl;
    }

    if(PRINT) cout << "[INFO]: new sequence ";

    // combine exe_sequence and new_sequence into ret_sequence
    vector<vector<uint16_t>> ret_sequence;
    ret_sequence.push_back(exe_sequence);
    ret_sequence.push_back(new_sequence);

    return ret_sequence;
}

