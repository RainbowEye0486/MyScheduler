# include "../include/GP.h"



GP *GP::inst_ = nullptr;

GP *GP::GetGP()
{
    if (inst_ == nullptr)
        inst_ = new GP();
    return inst_;
}

GP::GP(){
    srand(time(NULL));
}

void GP::PreProcessing(mJob job){
    Scheduler::PreProcessing(job);
}

uint16_t GP::GetSolution(){
    return Scheduler::GetSolution();
}

double GP::GetCalculateTime(){
    return Scheduler::GetCalculateTime();
}

/**
 * @brief
 * A Genetic Programming algorithm to solve workstation scheduling problem.
*/
void GP::RunAlgorithm(){
    auto start = high_resolution_clock::now();
    fitness_time = 0;


    if (PRINT)cout << "[INFO]: GP algorithm start." << endl;
    // Parameter setting
    vector<Node *> population;
    vector<uint16_t> fitness;
    for (int i = 0; i < POP_SIZE; i++) {
        population.push_back(Tree_init(2, Init_deep, 1));
    }

    // Best fitness
    uint16_t best_fitness = UINT16_MAX;

    // Start Genetic Programming Algorithm
    for (int it = 0; it < Max_iter; it++) {
        
        fitness.clear();
        // Calculate fitness
        for (int i = 0; i < POP_SIZE; i++) {
            fitness.push_back(GP_Fitness(population[i]));
            if (fitness[i] < best_fitness) {
                best_fitness = fitness[i];
                
            }
        }

        // Rescale Fitness
        vector<double> possibility = Rescale(fitness);

        // CrossOver with crossover rate CR
        for (int i = 0; i < POP_SIZE; i += 2) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < CR) {
                CrossOver(population[i], population[i + 1]);
            }
        }

        // Mutation with mutation rate MR
        for (int i = 0; i < POP_SIZE; i++) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < MR) {
                Mutation(population[i]);
            }
        } 

    }


    

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
     if (PRINT)cout << "[INFO]: GP function spand " << (double)duration.count() / (double)1000000 << " s." << endl;
    // print fitness_time
    // cout << "[INFO]: GP function fitness " << fitness_time << " s." << endl;
    calculate_time = (double)duration.count() / (double)1000000;
    station = best_fitness;
}

Node *GP::Node_init(int depth){
    Node *newNode = new Node;
    newNode->Operand = -1;
    newNode->lnode = NULL;
    newNode->rnode = NULL; 
    newNode->depth = depth;
    return newNode;
}

/**
 * @brief
 * free all node in tree
*/
void GP::FreeTree(Node *root){
    if (root == NULL) return;
    FreeTree(root->lnode);
    FreeTree(root->rnode);
    delete root;
}



bool GP::Validate_Depth(Node *root, int depth) {
    if (root == NULL) return true;
    if (root->depth != depth) {
        cout << "\033[1;31m[ERROR]: depth error, level not match\033[0m" << endl;
        return false;
    }
    if (root->depth > Max_deep) {
        cout << "\033[1;31m[ERROR]: depth error, overflow\033[0m" << endl;
        return false;
    }
    if (Validate_Depth(root->lnode, depth + 1) && Validate_Depth(root->rnode, depth + 1)) {
        return true;
    }
    return false; 
}

/**
 * @brief
 * Init a tree with random node.
 * 
 * @param min_deep: min depth of tree
 * @param max_deep: max depth of tree
 * @param init_deep: if is a sub tree, init depth
 * @return Node*
*/
Node *GP::Tree_init(int min_deep, int max_deep, int init_deep){
    int total_node = RAND(pow(2, min_deep) - 1, pow(2, max_deep) - 1);
    if(PRINT) cout << "[INFO]: total node " << total_node << endl;
   
    vector<Node *> tree;
    tree.resize((int)pow(2, max_deep) - 1);
    vector<uint16_t> select_queue;
    select_queue.push_back(1);

    int deep = init_deep;
    tree[0] = Node_init(deep);

    for (int i = 1; i < total_node; i += 2) {

        int rand_idx = RAND(0, select_queue.size());
        uint16_t task_num = select_queue[rand_idx];
        if(PRINT) cout << "[INFO]: Select node " << task_num << endl;
             
        deep = init_deep + log2(task_num);

        tree[2 * task_num] = Node_init(deep + 1);
        tree[2 * task_num - 1] = Node_init(deep + 1);
        tree[task_num - 1]->lnode = tree[2 * task_num - 1];
        tree[task_num - 1]->rnode = tree[2 * task_num];

        select_queue.erase(select_queue.begin() + rand_idx);
        
        if (4 * task_num + 1 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num);
        }
            
        if (4 * task_num + 3 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num + 1);
        }  
        
    }

    AssertNode(tree[0]);

    return tree[0];

}

void GP::AssertNode(Node *root) {
    if (!root) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
        return;
    }
    if (!root->lnode && !root->rnode){
        int terminate = RAND(Operand::PT, Operand::ID + 1);
        root->Operand = terminate;
        return;
    }

    int non_terminate = RAND(Operand::ADD, Operand::MIN + 1);
    root->Operand = non_terminate;

    if (root->rnode) {
        AssertNode(root->rnode);
    }

    if (root->lnode) {
        AssertNode(root->lnode);
    }

}

void GP::Postorder(Node *node) {
    if (!node) return;

    Postorder(node->lnode);
    Postorder(node->rnode);
    stack_.push_back(node->Operand);

}

/**
 * @brief : Clone the tree, and return the root of new tree
*/
Node *GP::CloneTree(Node *root) {
    if (!root) return NULL;
    Node *newNode = new Node;
    newNode->Operand = root->Operand;
    newNode->depth = root->depth;
    newNode->lnode = CloneTree(root->lnode);
    newNode->rnode = CloneTree(root->rnode);
    return newNode;
}




/**
 * @brief : Rescale the fitness value
*/
vector<double> GP::Rescale(vector<uint16_t> fitness) {
    vector<double> rescale_fitness;
    double total = 0;
    uint16_t rank_val = fitness[0];
    int rank = 1;
    for (int i = 0; i < fitness.size(); i++) {
        if (fitness[i] > rank_val) {
            rank_val = fitness[i];
            rank ++;
        }
        rescale_fitness.push_back(sqrt( 1 / (double)rank));
        total += rescale_fitness[i];
    }

    double accum = 0;
    for (int i = 0; i < fitness.size(); i++) {
        rescale_fitness[i] /= total;
        accum += rescale_fitness[i];
        rescale_fitness[i] = accum;
    }

    return rescale_fitness;
}

/**
 * @brief
 * Calculate postfix stack stack_ and output the result.
*/
double GP::Priority() {
    stack<double> prosses_s;

    for (int i = 0; i < stack_.size(); i++) {
        uint16_t element = stack_[i];
        if (element < Operand::PT) { // terminel set
            double _operator = element;
            double _operand2 = prosses_s.top();
            prosses_s.pop();
            double _operand1 = prosses_s.top();
            prosses_s.pop();
            switch (element)
            {
            case ADD:
                prosses_s.push(_operand1 + _operand2);
                break;
            case SUB:
                prosses_s.push(_operand1 - _operand2);
                break;
            case MUL:
                prosses_s.push(_operand1 * _operand2);
                break;
            case DIV:
                if (_operand2 == 0) // protect division
                    prosses_s.push(1);
                else 
                    prosses_s.push(_operand1 / _operand2);
                break;
            case MAX:
                prosses_s.push(max(_operand1, _operand2));
                break;
            case MIN:
                prosses_s.push(min(_operand1, _operand2));
                break;
            default:
                break;
            }

        }
        else { 
            switch (element)
            {
            case Operand::PT:
                prosses_s.push(terminate[0]);
                break;
            case Operand::MS:
                prosses_s.push(terminate[1]);
                break;
            case Operand::RT:
                prosses_s.push(terminate[2]);
                break;
            case Operand::HT:
                prosses_s.push(terminate[3]);
                break;
            case Operand::TJ:
                prosses_s.push(terminate[4]);
                break;
            case Operand::RJ:
                prosses_s.push(terminate[5]);
                break;
            case Operand::ID:
                prosses_s.push(terminate[6]);
                break;
            default:
                break;
            }
        }
    }
    if (prosses_s.size() != 1){
        cout << "\033[1;33m[WARNING]: Priority illegal.\033[0m" << endl;
    }
    if(PRINT) cout << "[INFO]: priority " << prosses_s.top() << endl;
    return prosses_s.top();

}


/**
 * @brief: This function find a random node in the tree, this node is other than the root node,
 *        and the node can be a non-terminel node or a terminel node. Than, replace the node with a new subtree,
 *       the new subtree is generated by the function Tree_init(deep). deep is (1, Max_deep - current_deep)
*/
void GP::Mutation(Node *root) {
    if (!root) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
    }
        if (!Validate_Depth(root, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
    
    // random select a node from the tree
    vector<Node *> node_queue;
    node_queue.push_back(root);
    int node_idx = 0;
    while (node_idx < node_queue.size()) {
        if (node_queue[node_idx]->lnode) {
            node_queue.push_back(node_queue[node_idx]->lnode);
        }
        if (node_queue[node_idx]->rnode) {
            node_queue.push_back(node_queue[node_idx]->rnode);
        }
        node_idx++;
    }

    int random_idx = RAND(1, node_queue.size());
    Node *random_node = node_queue[random_idx];

    // record random_node's parent 
    Node *parent = NULL;
    for (int i = 0; i < node_queue.size(); i++) {
        if (node_queue[i]->lnode == random_node || node_queue[i]->rnode == random_node) {
            parent = node_queue[i];
            break;
        }
    }

    int parent_deep = parent->depth;

    if (parent_deep > Max_deep) {
        cout << "\033[1;33m[WARNING]: parent_deep > Max_deep\033[0m" << endl;
    }
    
    // create a new subtree
    int new_deep = RAND(1, Max_deep - parent_deep);
    Node *new_subtree = Tree_init(1, new_deep, parent_deep + 1);

    // replace random_node with new_subtree
    if (parent->lnode == random_node) { // random_node is lnode of parent
        Node *tmp = parent->lnode;
        parent->lnode = new_subtree; 
        FreeTree(tmp);
    }
    else { // random_node is rnode of parent
        Node *tmp = parent->rnode;
        parent->rnode = new_subtree;
        FreeTree(tmp);
    }

    if (!Validate_Depth(root, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
}


/**
 * @brief: ramdomly select two subtree from two tree, and exchange them. after exchange
 *          , the depth of two tree won't exceed Max_deep
*/
void GP::CrossOver(Node *root1, Node *root2) {
    bool node1_is_lnode = false;
    bool node2_is_lnode = false;
    if (!root1 || !root2) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
    }

    // random select a node from the tree
    vector<Node *> node_queue1;
    node_queue1.push_back(root1);
    int node_idx1 = 0;
    while (node_idx1 < node_queue1.size()) {
        if (node_queue1[node_idx1]->lnode) {
            node_queue1.push_back(node_queue1[node_idx1]->lnode);
        }
        if (node_queue1[node_idx1]->rnode) {
            node_queue1.push_back(node_queue1[node_idx1]->rnode);
        }
        node_idx1++;
    }

    int random_idx1 = RAND(1, node_queue1.size());
    Node *random_node1 = node_queue1[random_idx1];

    // record random_node's parent 
    Node *parent1 = NULL;
    for (int i = 0; i < node_queue1.size(); i++) {
        if (node_queue1[i]->lnode == random_node1 || node_queue1[i]->rnode == random_node1) {
            parent1 = node_queue1[i];
            break;
        }
    }
    
    int parent_deep = parent1->depth;

    int random1_height = Tree_Height(random_node1);

    // random select a node from the tree
    vector<Node *> node_queue2;
    node_queue2.push_back(root2);
    int node_idx2 = 0;
    while (node_idx2 < node_queue2.size()) {
        if (node_queue2[node_idx2]->lnode) {
            node_queue2.push_back(node_queue2[node_idx2]->lnode);
        }
        if (node_queue2[node_idx2]->rnode) {
            node_queue2.push_back(node_queue2[node_idx2]->rnode);
        }
        node_idx2++;
    }

    Node *random_node2 = nullptr;
    // clone  node_queue2 to candaite_queue
    vector<Node *> candaite_queue;
    for (int i = 0; i < node_queue2.size(); i++) {
        candaite_queue.push_back(node_queue2[i]);
    }

    while (candaite_queue.size() > 1) {
        int random_idx2 = RAND(1, candaite_queue.size());
        int random2_height = Tree_Height(candaite_queue[random_idx2]);
        if ((random2_height > random1_height) || 
            (candaite_queue[random_idx2]->depth + random1_height > Max_deep)) {
            candaite_queue.erase(candaite_queue.begin() + random_idx2);
        }
        else {
            random_node2 = candaite_queue[random_idx2];
            break;
        }
    }


         
    if (!random_node2) { // only head to choose
        Node *tmp = CloneTree(root2);
        FreeTree(root2);
        if (parent1->lnode == random_node1) { // random_node1 is lnode of parent1
            parent1->lnode = tmp;
            root2 = random_node1;
        }
        else { // random_node1 is rnode of parent1  
            parent1->rnode = tmp;
            root2 = random_node1;
        }
        vector<Node *> tmp_queue1;
        tmp_queue1.push_back(random_node1);
        int tmp_idx1 = 0;
        vector<Node *> tmp_queue2;
        tmp_queue2.push_back(root2);
        int tmp_idx2 = 0;
        while (tmp_idx1 < tmp_queue1.size()) {
            if (tmp_queue1[tmp_idx1]->lnode) {
                tmp_queue1.push_back(tmp_queue1[tmp_idx1]->lnode);
            }
            if (tmp_queue1[tmp_idx1]->rnode) {
                tmp_queue1.push_back(tmp_queue1[tmp_idx1]->rnode);
            }
            tmp_idx1++;
        }
        while (tmp_idx2 < tmp_queue2.size()) {
            if (tmp_queue2[tmp_idx2]->lnode) {
                tmp_queue2.push_back(tmp_queue2[tmp_idx2]->lnode);
            }
            if (tmp_queue2[tmp_idx2]->rnode) {
                tmp_queue2.push_back(tmp_queue2[tmp_idx2]->rnode);
            }
            tmp_idx2++;
        }
        for (int i = 0; i < tmp_queue1.size(); i++) {
            tmp_queue1[i]->depth = tmp_queue1[i]->depth + parent1->depth - 1;
        }
        for (int i = 0; i < tmp_queue2.size(); i++) {
            tmp_queue2[i]->depth = tmp_queue2[i]->depth - parent1->depth + 1;
        }
        Validate_Depth(root2, 1);
        Validate_Depth(root1, 1);
        cout << "only head to choose" << endl;
        return;
    }
        


    // record random_node's parent
    Node *parent2 = NULL;
    for (int i = 0; i < node_queue2.size(); i++) {
        if (node_queue2[i]->lnode == random_node2 || node_queue2[i]->rnode == random_node2) {
            parent2 = node_queue2[i];
            break;
        }
    }

    // exchange random_node1 and random_node2
    if (parent1->lnode == random_node1) { // random_node1 is lnode of parent1
        node1_is_lnode = true;
    }
    if (parent2->lnode == random_node2) { // random_node2 is lnode of parent2
        node2_is_lnode = true;
    }
    if (node1_is_lnode) {
        parent1->lnode = random_node2;
    }
    else {
        parent1->rnode = random_node2;
    }

    if (node2_is_lnode) {
        parent2->lnode = random_node1;
    }
    else {
        parent2->rnode = random_node1;
    }
    // traverse subtree of random_node1 and random_node2
    Resize_deep(random_node1, parent1, parent2);
    Resize_deep(random_node2, parent2, parent1);

    // Validate the tree
    if (!Validate_Depth(root1, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
    if (!Validate_Depth(root2, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }

    // free memory
    node_queue1.clear();
    node_queue2.clear();


    return;
}


int GP::Tree_Height(Node *root) {
    if (!root) {
        return 0;
    }
    int left_height = Tree_Height(root->lnode);
    int right_height = Tree_Height(root->rnode);
    return max(left_height, right_height) + 1;
}


void GP::Resize_deep(Node *root, Node *parent_old, Node *parent_new) {
    vector<Node *> tmp_queue;
    tmp_queue.push_back(root);
    int tmp_idx = 0;
    while (tmp_idx < tmp_queue.size()) {
        if (tmp_queue[tmp_idx]->lnode) {
            tmp_queue.push_back(tmp_queue[tmp_idx]->lnode);
        }
        if (tmp_queue[tmp_idx]->rnode) {
            tmp_queue.push_back(tmp_queue[tmp_idx]->rnode);
        }
        tmp_idx++;
    }
    for (int i = 0; i < tmp_queue.size(); i++) {
        tmp_queue[i]->depth = tmp_queue[i]->depth + parent_new->depth - parent_old->depth;
    }
}

/**
 * @brief: This function combine generate sequence and evaluate fitness
*/
uint16_t GP::GP_Fitness(Node *root){
    // Calculate total used station in the sequence
    
    auto in = high_resolution_clock::now();

    vector<uint16_t> sequence;
    vector<uint16_t> r_avaliable;
    vector<uint16_t> h_avaliable;
    stack_.clear();
    Postorder(root);
    
    avaliable_node.clear();
    for (int i = 0; i < mjob.taskNum; i++) {
        if (leaf_node[i]) {
            avaliable_node.push_back(i + 1);
            char executor = mjob.task_list[i].executor;
            if (executor == 'b' || executor == 'r' || executor == 'e') {
                r_avaliable.push_back(i + 1);
            }
            if (executor == 'b' || executor == 'h' || executor == 'e') {
                h_avaliable.push_back(i + 1);
            }
        }

        finish_node[i] = false;
    }

    uint16_t r_cur_time = 0;
    uint16_t h_cur_time = 0;

    // Input parameter

    uint16_t TJ = mjob.taskNum;
    uint16_t RJ = mjob.taskNum;
    uint16_t MS = 0;
    uint16_t RT = 0;
    uint16_t HT = 0;

    // Calculate sequence and fitness
    while (avaliable_node.size() > 0) {
        uint16_t chose_node = -1;
        uint16_t last_node = 0;
        vector<double> prio_list_r;
        vector<double> prio_list_h;
        vector<uint16_t> candidate_node;
        char executor = ' ';

        double best_prio_r = __DBL_MAX__;
        double best_prio_h = __DBL_MAX__;
        for (int i = 0; i < r_avaliable.size(); i++) {
            // Calculate terminate
            uint16_t PT = mjob.task_list[r_avaliable[i] - 1].r_time;
            RT = r_cur_time + PT;
            uint16_t ID = 0;
            if (mjob.task_list[r_avaliable[i] - 1].executor == 'b') {
                ID = abs(r_cur_time + PT - h_cur_time);
            }

            // set terminate aray
            terminate[0] = PT;
            terminate[1] = MS;
            terminate[2] = RT;
            terminate[3] = HT;
            terminate[4] = TJ;
            terminate[5] = RJ;
            terminate[6] = ID;
            
            double prio = Priority();
            prio_list_r.push_back(prio);
            if (prio < best_prio_r) {
                best_prio_r = prio;
            }
        }

        // Calculate for human too
        for (int i = 0; i < h_avaliable.size(); i++) {
            // Calculate terminate
            uint16_t PT = mjob.task_list[h_avaliable[i] - 1].h_time;
            HT = h_cur_time + PT;
            uint16_t ID = 0;
            if (mjob.task_list[h_avaliable[i] - 1].executor == 'b') {
                ID = abs(h_cur_time + PT - r_cur_time);
            }

            // set terminate aray
            terminate[0] = PT;
            terminate[1] = MS;
            terminate[2] = RT;
            terminate[3] = HT;
            terminate[4] = TJ;
            terminate[5] = RJ;
            terminate[6] = ID;
            
            double prio = Priority();
            prio_list_h.push_back(prio);
            if (prio < best_prio_h) {
                best_prio_h = prio;
            }
        }

        int candidate_bound = 0;
        // Find best priority
        if (best_prio_r < best_prio_h) {
            for (int i = 0; i < prio_list_r.size();i++) {
                if (prio_list_r[i] == best_prio_r)
                    candidate_node.push_back(i);
            }
            executor = 'r';
        }
        else if (best_prio_r > best_prio_h) {
            for (int i = 0; i < prio_list_h.size();i++) {
                if (prio_list_h[i] == best_prio_h)
                    candidate_node.push_back(i);
            }
            executor = 'h';
        }
        else {
            for (int i = 0; i < prio_list_h.size();i++) {
                if (prio_list_h[i] == best_prio_h)
                    candidate_node.push_back(i);
                    candidate_bound++;
            }
            for (int i = 0; i < prio_list_r.size();i++) {
                if (prio_list_r[i] == best_prio_r) {
                    // if i is in candidate_node, skip
                    bool skip = false;
                    for (int j = 0; j < candidate_node.size(); j++) {
                        if (i == candidate_node[j]) {
                            skip = true;
                            break;
                        }
                    }
                    if (!skip)
                        candidate_node.push_back(i);
                }
            }
        }



        // stop random strategy
        int idx = RAND(0, candidate_node.size());
        if (candidate_node.size() == 1 && idx == 1){
            idx = 0;
        }


        if (candidate_node.size() == 0)
            cout << "[INFO]: candidate node size is 1" << endl;
        

        if (executor == ' ' && idx >= candidate_bound) {
            executor = 'r';
        }
        else if (executor == ' ' && idx < candidate_bound) {
            executor = 'h';
        }
        
        if (avaliable_node.size() == 1) {
            chose_node = avaliable_node[0] - 1;
        }
        else {
            chose_node = avaliable_node[candidate_node[idx]] - 1;
        }
        
        if (executor == ' ' && mjob.task_list[chose_node].executor == 'b') {
            executor = 'b';
        }

        if (executor == ' '){
            cout << "[ERROR]: executor is not set" << endl;
        }

        // print chose node
        if (PRINT)cout << "[INFO]: chose node " << (int)chose_node + 1 << endl;
        
        if (chose_node == -1)
            cout << "\033[1;33m[WARNING]: No avaliable node return.\033[0m" << endl;


        // Change condition
        if (executor == 'r') {
            r_cur_time += mjob.task_list[chose_node].r_time;
            MS = max(h_cur_time, r_cur_time);
        }
        else if (executor == 'h') {
            h_cur_time += mjob.task_list[chose_node].h_time;
            MS = max(r_cur_time, h_cur_time);
        }
        else if (executor == 'b') {
            uint16_t common_time = max(r_cur_time, h_cur_time);
            r_cur_time = mjob.task_list[idx].r_time + common_time;
            h_cur_time = mjob.task_list[idx].h_time + common_time;
        }
        else {
            cout << "\033[1;31m[ERROR]: No executor.\033[0m" << endl;
        }
        RJ--;
        
        // remove executed node and add next avaliable node
        finish_node[chose_node] = true;
        
        // Add next posible node to the list
        for (int i = 0; i < mjob.task_list[chose_node].or_next.size(); i++){
            uint16_t search_node = mjob.task_list[chose_node].or_next[i];
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1]){
                avaliable_node.push_back(search_node);
                if (executor == 'r') {
                    r_avaliable.push_back(search_node);
                }
                else if (executor == 'h') {
                    h_avaliable.push_back(search_node);
                }
                else if (executor == 'b') {
                    r_avaliable.push_back(search_node);
                    h_avaliable.push_back(search_node);
                }
                else {
                    cout << "\033[1;31m[ERROR]: No executor.\033[0m" << endl;
                }
            }
        }
        for (int i = 0; i < mjob.task_list[chose_node].and_next.size(); i++){
            bool and_done = true;
            uint16_t search_node = mjob.task_list[chose_node].and_next[i];
            if(PRINT) cout << "[INFO]: prev node" << (int)mjob.task_list[search_node - 1].and_prev.size() << endl;
            for (int j = 0; j < mjob.task_list[search_node - 1].and_prev.size(); j++) {
                if(PRINT) cout << "[INFO]: prev and node[" << (int)mjob.task_list[search_node - 1].and_prev[j] << "]finished." << endl;
                if (!finish_node[mjob.task_list[search_node - 1].and_prev[j] - 1]) 
                    and_done = false;
            }   
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1] && and_done){
                avaliable_node.push_back(search_node);
                if (executor == 'r') {
                    r_avaliable.push_back(search_node);
                }
                else if (executor == 'h') {
                    h_avaliable.push_back(search_node);
                }
                else if (executor == 'b') {
                    r_avaliable.push_back(search_node);
                    h_avaliable.push_back(search_node);
                }
                else {
                    cout << "\033[1;31m[ERROR]: No executor.\033[0m" << endl;
                }
                if (PRINT) cout << "[INFO]: " << (int)search_node << " added." << endl;
            }
        }

        avaliable_node.erase(avaliable_node.begin() + candidate_node[idx]);
        
        int idx_erase = 0;
        if (r_avaliable.size() > 0) {
            for (int i = 0; i < r_avaliable.size(); i++) {
                if (r_avaliable[i] == chose_node + 1) {
                    idx_erase = i;
                    r_avaliable.erase(r_avaliable.begin() + idx_erase);
                    break;
                }
            }
        }

         
        idx_erase = 0;
        if (h_avaliable.size() > 0) {
            for (int i = 0; i < h_avaliable.size(); i++) {
                if (h_avaliable[i] == chose_node + 1) {
                    idx_erase = i;
                    h_avaliable.erase(h_avaliable.begin() + idx_erase);
                    break;
                }
            }
        }
        

        sequence.push_back(chose_node + 1);

    }
    
    

    auto out = high_resolution_clock::now();
    auto dura = duration_cast<microseconds>(out - in);
    fitness_time += (double)dura.count() / (double)1000000;

    return max(r_cur_time, h_cur_time);
}


