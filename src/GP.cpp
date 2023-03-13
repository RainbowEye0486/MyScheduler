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

double GP::GetAvgFitness(int i){
    return avg_fit_list[i];
}

/**
 * @brief
 * A Genetic Programming algorithm to solve workstation scheduling problem.
*/
void GP::RunAlgorithm(){
    auto start = high_resolution_clock::now();
    fitness_time = 0;

    avg_fit_list.clear();
    if (PRINT)cout << "[INFO]: GP algorithm start." << endl;
    // Parameter setting
    vector<Node *> population;
    vector<uint16_t> fitness;

    trim_ratio[0] = 0.8;
    trim_ratio[1] = 1;
    trim_ratio[2] = 0.64;
    trim_ratio[3] = 0.32;
    trim_ratio[4] = 0.13;
    trim_ratio[5] = 0;

    for (int i = 0; i < 11; i++)
        feature_usage[i] = 1; 


    // Use ramped half-and-half to initialize population
    for (int i = 0; i < POP_SIZE; i++) {
        if (i < POP_SIZE / 2)
            population.push_back(Tree_init(2, Max_deep, 1, "grow"));
            
        else
            population.push_back(Tree_init(2, Max_deep, 1, "full"));
        Validate_Depth(population[i], 1);
    }

    // shuffle population
    random_shuffle(population.begin(), population.end());

    // Best fitness
    uint16_t best_fitness = UINT16_MAX;


    // Start Genetic Programming Algorithm
    for (int it = 0; it < Max_iter; it++) {

        trim_success_rate = 0;

        random_shuffle(population.begin() + ELITE_SIZE, population.end());
        // [CrossOver] with crossover rate CR
        for (int i = 0 + ELITE_SIZE; i < POP_SIZE; i += 2) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < CR)
                CrossOver(population[i], population[i + 1]);
        }

        // [Mutation] with mutation rate MR
        for (int i = ELITE_SIZE; i < POP_SIZE; i++) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < MR) 
                Mutation(population[i]);
        }


        // [Fitness] calculation
        fitness.clear();
        int extinct_num = 0;
        node_num_accum = 0;
        for (int i = 0; i < POP_SIZE; i++) {
            int before = node_num_accum;
            uint16_t fit = GP_Fitness(population[i]);
            int after = node_num_accum;
            if (fit == UINT16_MAX)
                extinct_num++;
            fitness.push_back(fit);
            if (fitness[i] < best_fitness) {
                best_fitness = fitness[i];
                // print best tree node
                // cout << "[INFO]: GP best tree node: " << after - before << endl;
            }
                
        }
        // print avarage node number
        // cout << "[INFO]: GP average node number: " << (double)node_num_accum / (double)POP_SIZE << endl;

        // print average fitness
        double sum = 0;
        for (int i = 0; i < POP_SIZE; i++) {
            if (fitness[i] != UINT16_MAX)
                sum += (double)fitness[i];
        }
              
        // cout << "[INFO]: GP average fitness: " << (double)sum / (double)(POP_SIZE - extinct_num)<< endl;
        // print best fitness
        // cout << "[INFO]: GP best fitness: " << best_fitness << endl;
        avg_fit_list.push_back((double)sum / (double)POP_SIZE);

        vector<Node *> new_population;
        if (NEW_ALG) {
            // [Select] Elite selection
            // bubble sort population by fitness
            for (int i = 0; i < POP_SIZE; i++) {
                for (int j = 0; j < POP_SIZE - i - 1; j++) {
                    if (fitness[j] > fitness[j + 1]) {
                        swap(fitness[j], fitness[j + 1]);
                        swap(population[j], population[j + 1]);
                    }
                }
            }

            // record best tree occored in the history
            feature_flag = true;

            for (int i = 0; i < POP_SIZE; i++) {
                if (history_tree.size() < int(POP_SIZE * 0.1)) {
                    history_tree.push_back(CloneTree(population[i]));
                    history_fitness.push_back(fitness[i]);
                }
                else 
                    break;
            }

            for (int i = 0; i < POP_SIZE; i++) {
                for (int j = 0; j < history_tree.size(); j++) {
                    if (fitness[i] < history_fitness[j] || (j > 0 && history_fitness[j] == history_fitness[j - 1])) {
                        Node *tmp = history_tree[j];
                        history_tree[j] = CloneTree(population[i]);
                        history_fitness[j] = fitness[i];
                        FreeTree(tmp);
                        break;
                    }
                }
            }

            feature_flag = false;

            

            uint16_t tmp_fit = fitness[0];
            int start_idx = 0;
            int accum = 0;
            for (int i = 1; i < POP_SIZE; i++) {
                if (fitness[i] == tmp_fit) {
                    accum++;
                }
                else {
                    tmp_fit = fitness[i];
                    if (accum >= int(POP_SIZE * 0.1) && fitness[i] != UINT16_MAX) {
                        // cout << "\033[1;31m[INFO]: Annihilation!!!\033[0m" << endl;
                        for (int j = start_idx + 1; j <= start_idx + int(POP_SIZE * 0.025); j++) {
                            Mutation(population[j]);
                        }
                        for (int j = start_idx + int(POP_SIZE * 0.025) + 1; j <= i; j++) {
                            // randomly select a tree from history tree
                            int rand_idx = rand() % history_tree.size();
                            population[j] = CloneTree(history_tree[rand_idx]);
                        }
                    }
                    accum = 0;
                    start_idx = i;
                }
            }
            
            


            // [Trim] trim tree
            float sigmoid = 0.8 / (1.0 + (exp(-1.0 * ((double)it / (double)Max_iter - 0.4) * 15.0)));

            
            // for (int i = 0; i < POP_SIZE; i++) {
            //     population[i] = Trim(population[i], fitness[i]);        
            // }
            // cout << "[INFO]: GP trim success rate: " << trim_success_rate / POP_SIZE << endl;
  

            int offspring_cnt[POP_SIZE] = { 0 };
            
            // Rescale fitness and get probability
            vector<uint16_t> rest_fit;
            for (int i = ELITE_SIZE; i < POP_SIZE; i++)
                rest_fit.push_back(fitness[i]);
            

            // [Select] new population by fitnss use Roulette Wheel selection
            vector<double> properbility = Rescale(rest_fit);
            for (int i = 0; i < POP_SIZE - ELITE_SIZE - extinct_num; i++) {
                float rand_num = NOR_RAND();
                for (int j = 0; j < POP_SIZE - ELITE_SIZE; j++) {
                    if (properbility[j] > rand_num) {
                        offspring_cnt[j + ELITE_SIZE]++;
                        break;
                    }
                }
            }

            // [Select] new population by fitnss use Tournament selection
            // for (int i = 0; i < POP_SIZE - ELITE_SIZE - extinct_num; i++) {
            //     int rand_num1 = rand() % (POP_SIZE - ELITE_SIZE);
            //     int rand_num2 = rand() % (POP_SIZE - ELITE_SIZE);
            //     if (fitness[rand_num1 + ELITE_SIZE] < fitness[rand_num2 + ELITE_SIZE]) 
            //         offspring_cnt[rand_num1 + ELITE_SIZE]++;
            //     else 
            //         offspring_cnt[rand_num2 + ELITE_SIZE]++;
            // }
            
            


            for (int i = 0; i < POP_SIZE; i++) 
                for (int j = 0; j < offspring_cnt[i]; j++) 
                    new_population.push_back(CloneTree(population[i]));   
        


            // [Select] fill new population with extinct population
            for (int i = 0; i < extinct_num; i++) {
                if (i > int(POP_SIZE * 0.05))
                    new_population.push_back(Tree_init(2, Max_deep, 1, "grow"));
                else {
                    int rand_idx = rand() % history_tree.size();
                    new_population.push_back(CloneTree(history_tree[rand_idx]));
                }
            }       
        }
        else {
            // bubble sort population by fitness
            for (int i = 0; i < POP_SIZE; i++) {
                for (int j = 0; j < POP_SIZE - i - 1; j++) {
                    if (fitness[j] > fitness[j + 1]) {
                        swap(fitness[j], fitness[j + 1]);
                        swap(population[j], population[j + 1]);
                    }
                }
            }
            
            // [Select] new population use Binary Tournament selection
            for (int i = 0; i < POP_SIZE; i++) {
                int rand_num1 = rand() % POP_SIZE;
                int rand_num2 = rand() % POP_SIZE;
                if (fitness[rand_num1] < fitness[rand_num2]) {
                    new_population.push_back(CloneTree(population[rand_num1]));
                }
                else {
                    new_population.push_back(CloneTree(population[rand_num2]));
                }
            }
        }
        
        // Replace population
        for (int i = 0; i < POP_SIZE; i++) {
            FreeTree(population[i]);
        }
        population.clear();
        population = new_population;

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
    newNode->parent = NULL;
    newNode->depth = depth;
    newNode->sticky = 1;
    newNode->height = 0;
    
    return newNode;
}

/**
 * @brief
 * free all memory of nodes in tree
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
 * @param method: "grow" or "full"
 * @return Node*
*/
Node *GP::Tree_init(int min_deep, int max_deep, int init_deep, string method){
    
    
    int total_node;
    
    if (method == "grow") {
        total_node = RAND(pow(2, min_deep) - 1, pow(2, max_deep) - 1);
    } else if (method == "full") {
        total_node = pow(2, max_deep) - 1;
    } else {
        cout << "\033[1;31m[ERROR]: Tree_init method error\033[0m" << endl;
        return NULL;
    }

    if (total_node == 1)
    {
        Node *newNode = Node_init(init_deep);
        AssertHeight(newNode);
        AssertNode(newNode);
        return newNode;
    }
    
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
        tree[2 * task_num - 1]->parent = tree[task_num - 1];
        tree[2 * task_num]->parent = tree[task_num - 1];

        select_queue.erase(select_queue.begin() + rand_idx);
        
        if (4 * task_num + 1 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num);
        }
            
        if (4 * task_num + 3 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num + 1);
        }  
        
    }

    AssertNode(tree[0]);
    AssertHeight(tree[0]);

    return tree[0];

}

void GP::AssertNode(Node *root) {
    if (!root) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
        return;
    }
    if (!root->lnode && !root->rnode){
        int terminator;
        if (NEW_ALG) {    
            float rand_num = NOR_RAND();
            float sum = 0;
            vector <float>terminator_ratio;
            for (int i = 4; i < 11; i++) {
                sum += feature_usage[i];
                terminator_ratio.push_back((float)feature_usage[i]);
            }
            for (int i = 0; i < terminator_ratio.size(); i++) {
                terminator_ratio[i] /= (float)sum;
            }
            for (int i = 1; i < terminator_ratio.size(); i++) {
                terminator_ratio[i] += terminator_ratio[i - 1];
            }
            for (int i = 0; i < terminator_ratio.size(); i++) {
                if (rand_num < terminator_ratio[i]) {
                    terminator = i + Operand::PT;
                    break;
                }
            }
        }
        else {
            terminator = RAND(Operand::PT, Operand::ID + 1);
        }

        root->Operand = terminator;
        return;
    }
    else {
        int non_terminator = -1;
        if (NEW_ALG) {
            float rand_num = NOR_RAND();
            vector <float>non_terminator_ratio;
            float sum = 0;
            for (int i = 0; i < 4; i++) {
                sum += feature_usage[i];
                non_terminator_ratio.push_back((float)feature_usage[i]);
            }
            for (int i = 0; i < non_terminator_ratio.size(); i++) {
                non_terminator_ratio[i] /= (float)sum;
            }
            for (int i = 1; i < non_terminator_ratio.size(); i++) {
                non_terminator_ratio[i] += non_terminator_ratio[i - 1];
            }
            for (int i = 0; i < non_terminator_ratio.size(); i++) {
                if (rand_num < non_terminator_ratio[i]) {
                    non_terminator = i + Operand::ADD;
                    break;
                }
            }
        }
        else {
            non_terminator = RAND(Operand::ADD, Operand::DIV + 1);
        }
        root->Operand = non_terminator;
        
    }

    

    if (root->rnode) {
        AssertNode(root->rnode);
    }

    if (root->lnode) {
        AssertNode(root->lnode);
    }

}

int GP::Postorder(Node *node) {
    if (!node) return 0;

    int ln = Postorder(node->lnode);
    int rn = Postorder(node->rnode);
    stack_.push_back(node->Operand);
    return ln + rn + 1;
}

/**
 * @brief : Clone the tree, and return the root of new tree
*/
Node *GP::CloneTree(Node *root) {
    if (!root) return NULL;
    Node *newNode = new Node;
    newNode->Operand = root->Operand;
    if (feature_flag)
        feature_usage[root->Operand - 1]++;
    newNode->depth = root->depth;
    newNode->height = root->height;
    newNode->sticky = root->sticky;
    newNode->parent = NULL;
    newNode->lnode = CloneTree(root->lnode);
    newNode->rnode = CloneTree(root->rnode);
    if (newNode->lnode) newNode->lnode->parent = newNode;
    if (newNode->rnode) newNode->rnode->parent = newNode;
    return newNode;
}


/**
 * @brief : Rescale the fitness value, the input fitness array is sorted
*/
vector<double> GP::Rescale(vector<uint16_t> fitness) {
    vector<double> rescale_fitness;
    double total = 0;
    uint16_t rank_val = fitness[0];
    int rank = 1;
    for (int i = 0; i < fitness.size(); i++) {
        if (fitness[i] == UINT16_MAX) {
            rescale_fitness.push_back(0);
            continue;
        }
        
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
    Node *random_node;
    if (NEW_ALG)
        random_node = Random_node(root, true);
    else
        random_node = Random_node(root, false);

    // record random_node's parent 
    Node *parent = random_node->parent;

    int parent_deep = parent->depth;

    if (parent_deep > Max_deep) {
        cout << "\033[1;33m[WARNING]: parent_deep > Max_deep\033[0m" << endl;
    }
    
    // create a new subtree
    Node *new_subtree;
    if (NEW_ALG) {
        new_subtree = Tree_init(1, random_node->height, parent_deep + 1, "grow");
    }
    else {
        int new_deep = RAND(1, Max_deep - parent_deep);
        new_subtree = Tree_init(1, new_deep, parent_deep + 1, "grow");
    }

    // replace random_node with new_subtree
    if (parent->lnode == random_node) { // random_node is lnode of parent
        parent->lnode = new_subtree; 
    }
    else { // random_node is rnode of parent
        parent->rnode = new_subtree;
    }
    new_subtree->parent = parent;
    FreeTree(random_node);
    Resize_height(new_subtree);

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
    Node *random_node1;
    if (NEW_ALG)
        random_node1 = Random_node(root1, true);
    else
        random_node1 = Random_node(root1, false);
    
    // record random_node's parent 
    Node *parent1 = random_node1->parent;
    
    int parent_deep = parent1->depth;

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
        if ((node_queue2[i]->height + random_node1->depth <= Max_deep +1) && 
            (node_queue2[i]->depth + random_node1->height <= Max_deep +1)) {
            candaite_queue.push_back(node_queue2[i]);
        }
    }

    if (NEW_ALG) {
        vector<float> ratio;
        int idx = 0;
        int random_idx = 0;
        for (int i = 0; i < candaite_queue.size(); i++) {
            ratio.push_back(1 * trim_ratio[candaite_queue[i]->depth - 1] / (float)candaite_queue[i]->sticky);
        }
        float sum = 0;
        for (int i = 1; i < ratio.size(); i++) {
            sum += ratio[i];
        }
        for (int i = 1; i < ratio.size(); i++) {
            ratio[i] = ratio[i] / sum;
        }
        float random = (float)rand() / RAND_MAX;
        float tmp = 0;
        for (int i = 1; i < ratio.size(); i++) {
            tmp += ratio[i];
            if (tmp > random) {
                random_idx = i;
                break;
            }
        }
        random_node2 = candaite_queue[random_idx];
    }
    else {
        int random_idx2 = RAND(1, candaite_queue.size());
        random_node2 = candaite_queue[random_idx2];
    }

         
    if (random_node2 == root2) { // only head to choose
        Node *tmp = CloneTree(root2);
        
        FreeTree(root2);
        if (parent1->lnode == random_node1)  // random_node1 is lnode of parent1
            parent1->lnode = tmp; 
        else // random_node1 is rnode of parent1  
            parent1->rnode = tmp;
        root2 = random_node1;
        tmp->parent = parent1;
        root2->parent = NULL;
        Resize_height(tmp);
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
    Node *parent2 = random_node2->parent;

    // exchange random_node1 and random_node2
    if (parent1->lnode == random_node1) // random_node1 is lnode of parent1
        node1_is_lnode = true;
    if (parent2->lnode == random_node2) // random_node2 is lnode of parent2
        node2_is_lnode = true;
    if (node1_is_lnode)
        parent1->lnode = random_node2;
    else
        parent1->rnode = random_node2;

    if (node2_is_lnode)
        parent2->lnode = random_node1;
    else
        parent2->rnode = random_node1;
    random_node1->parent = parent2;
    random_node2->parent = parent1;

    // traverse subtree of random_node1 and random_node2
    Resize_deep(random_node1, parent1, parent2);
    Resize_deep(random_node2, parent2, parent1);


    // Validate the tree
    if (!Validate_Depth(root1, 1))
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    if (!Validate_Depth(root2, 1))
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    Resize_height(random_node1);
    Resize_height(random_node2);

    // free memory
    node_queue2.clear();


    return;
}


/**
 * @brief: Try to trim a tree, to make it smaller
*/
Node *GP::Trim(Node *root, uint16_t fit){
    Node *random_node = Random_node(root, true);
    Node *parent = random_node->parent;
    Node *grandParent = parent->parent;
    Node *changeNode;
    if (grandParent) {
        if (grandParent->lnode == parent) {
            if (parent->lnode == random_node) {
                changeNode = parent->rnode;
                parent->rnode = NULL;
            }
            else {
                changeNode = parent->lnode;
                parent->lnode = NULL;
            }
        }
        else {
            if (parent->lnode == random_node) {
                changeNode = parent->rnode;
                parent->rnode = NULL;
            }
            else {
                changeNode = parent->lnode;
                parent->lnode = NULL;
            }
        }
        // replace parent with changeNode
        if (grandParent->lnode == parent) {
            grandParent->lnode = changeNode;
        }
        else {
            grandParent->rnode = changeNode;
        }
        changeNode->parent = grandParent;
        uint16_t new_fit = GP_Fitness(root);
        if (new_fit < fit  && root->lnode && root->rnode) {
            changeNode->sticky++;
            Node *tmp = changeNode;
            while(tmp->parent) {
                tmp->parent->sticky++;
                tmp = tmp->parent;
            }
            trim_success_rate += 1;
            // resize depth
            Resize_deep(changeNode, parent, grandParent);
            // resize height
            Resize_height(changeNode);
            // free parent
            FreeTree(parent);
            // Validate the tree
            if (!Validate_Depth(root, 1))
                cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
            return root;
        }
        else {
            // recover the tree
            if (grandParent->lnode == changeNode) {
                grandParent->lnode = parent;
            }
            else {
                grandParent->rnode = parent;
            }
            if (parent->lnode == random_node) {
                parent->rnode = changeNode;
            }
            else {
                parent->lnode = changeNode;
            }
            changeNode->parent = parent;
            parent->parent = grandParent;
            random_node->parent = parent;
        }
    }
    else {
        if (parent->lnode == random_node) {
            changeNode = parent->rnode;
            parent->rnode = NULL;
        }
        else {
            changeNode = parent->lnode;
            parent->lnode = NULL;
        }
        root = changeNode;
        changeNode->parent = NULL;
        uint16_t new_fit = GP_Fitness(root);
        if (new_fit < fit && root->lnode && root->rnode) {
            trim_success_rate += 1;
            changeNode->sticky++;
            Node *tmp = changeNode;
            while(tmp->parent) {
                tmp->parent->sticky++;
                tmp = tmp->parent;
            }
            // resize depth
            Resize_deep(changeNode, parent, NULL);
            // resize height
            Resize_height(changeNode);
            // free parent
            FreeTree(parent);
            // Validate the tree
            if (!Validate_Depth(root, 1))
                cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
            return root;
        }
        else {
            // recover
            if (parent->lnode == random_node) {
                parent->rnode = changeNode;
            }
            else {
                parent->lnode = changeNode;
            }
            changeNode->parent = parent;
            random_node->parent = parent;
            root = parent;
        }
    }
    return root;
}





/**
 * @brief: check parent and child's height, if not equal, check parent's height till root
 * @param: node : current node
*/
void GP::Resize_height(Node *node) {
    Node *parent = node->parent;
    while (parent) {
        int hight;
        if (parent->lnode && parent->rnode)
            hight = max(parent->lnode->height, parent->rnode->height) + 1;
        else if (!parent->rnode)
            hight = parent->lnode->height + 1;
        else if (!parent->lnode)
            hight = parent->rnode->height + 1;
        else
            hight = 1;

        if (hight > parent->height) {
            parent->height = hight;
            parent = parent->parent;
            continue;
        }
        else {
            break;
        }
    }
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
        if (parent_new)
            tmp_queue[i]->depth = tmp_queue[i]->depth + parent_new->depth - parent_old->depth;
        else
            tmp_queue[i]->depth = tmp_queue[i]->depth - parent_old->depth;
    }
}


int GP::AssertHeight(Node *node) {
    if (node == NULL) {
        return 0;
    }
    int lheight = AssertHeight(node->lnode);
    int rheight = AssertHeight(node->rnode);
    node->height = max(lheight, rheight) + 1;
    return node->height;
}


Node *GP::Random_node(Node *root, bool weighted) {
    vector<Node *> node_queue;
    node_queue.push_back(root);
    vector<float> ratio;
    int idx = 0;
    int random_idx = 0;
    while (idx < node_queue.size()) {
        if (node_queue[idx]->lnode) {
            node_queue.push_back(node_queue[idx]->lnode);
        }
        if (node_queue[idx]->rnode) {
            node_queue.push_back(node_queue[idx]->rnode);
        }
        ratio.push_back(1 * trim_ratio[node_queue[idx]->height - 1] / (float)node_queue[idx]->sticky);
        idx++; 
    }
    // select node by ratio use Wheel Roulette Method, the node can't be root
    if (weighted) {
        float sum = 0;
        for (int i = 1; i < ratio.size(); i++) {
            sum += ratio[i];
        }
        for (int i = 1; i < ratio.size(); i++) {
            ratio[i] = ratio[i] / sum;
        }
        float random = (float)rand() / RAND_MAX;
        float tmp = 0;
        for (int i = 1; i < ratio.size(); i++) {
            tmp += ratio[i];
            if (tmp > random) {
                random_idx = i;
                break;
            }
        }
    }
    else {
        random_idx = RAND(1, node_queue.size() - 1);
    }

    return node_queue[random_idx];

}


/**
 * @brief: This function combine generate sequence and evaluate fitness
*/
uint16_t GP::GP_Fitness(Node *root){
    // Calculate total used station in the sequence
    
    auto in = high_resolution_clock::now();

    vector<uint16_t> sequence;
    vector<uint16_t> push_node(2);
    stack_.clear();
    int node_num = Postorder(root);
    node_num_accum += node_num; 

    int repeat = 0;
    
    avaliable_node_GP.clear();
    for (int i = 0; i < mjob.taskNum; i++) {
        if (leaf_node[i]) {
            char executor = mjob.task_list[i].executor;
            push_node[0] = i + 1;
            if (executor == 'r') {
                push_node[1] = Executor::ROBOT;
                avaliable_node_GP.push_back(push_node);
            }    
            else if (executor == 'h') {
                push_node[1] = Executor::HUMAN;
                avaliable_node_GP.push_back(push_node);
            }
            else if (executor == 'b') {
                push_node[1] = Executor::BOTH;
                avaliable_node_GP.push_back(push_node);
            }
            else { // case each
                push_node[1] = Executor::ROBOT;
                avaliable_node_GP.push_back(push_node);
                push_node[1] = Executor::HUMAN;
                avaliable_node_GP.push_back(push_node);
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
    uint16_t PT = 0;
    uint16_t ID = 0;

    // Calculate sequence and fitness
    while (avaliable_node_GP.size() > 0) {
        uint16_t chose_node = -1;
        uint16_t last_node = 0;
        vector<uint16_t> candidate_node;
        vector<double> prio_list;

        double best_prio = __DBL_MAX__;

        for (int i = 0; i < avaliable_node_GP.size(); i++) {
            // Calculate non_terminator
            uint16_t executor = avaliable_node_GP[i][1];
            if (executor == Executor::ROBOT) {
                PT = mjob.task_list[avaliable_node_GP[i][0] - 1].r_time;
                RT = r_cur_time + PT;
                HT = h_cur_time;
                ID = 0;
            }
            else if (executor == Executor::HUMAN){
                PT = mjob.task_list[avaliable_node_GP[i][0] - 1].h_time;
                RT = r_cur_time;
                HT = h_cur_time + PT;
                ID = 0;
            }
            else if (executor == Executor::BOTH){
                if (mjob.task_list[avaliable_node_GP[i][0] - 1].h_time != mjob.task_list[avaliable_node_GP[i][0] - 1].r_time)
                    cout << "\033[1;31m[ERROR]: Executor [Both] not match!\033[0m" << endl;
                PT = mjob.task_list[avaliable_node_GP[i][0] - 1].h_time;
                uint16_t maximum = max(r_cur_time, h_cur_time);
                RT = maximum + PT;
                HT = maximum + PT;
                ID = abs(r_cur_time - h_cur_time);
            }
            else {
                cout << "\033[1;31m[ERROR]: Executor not found!\033[0m" << endl;
            }
            MS = max(RT, HT);

            // set non_terminator aray
            terminate[0] = PT;
            terminate[1] = MS;
            terminate[2] = RT;
            terminate[3] = HT;
            terminate[4] = TJ;
            terminate[5] = RJ;
            terminate[6] = ID;
            
            double prio = Priority();
            prio_list.push_back(prio);
            if (prio < best_prio) {
                best_prio = prio;
            }
        }

        for (int i = 0; i < prio_list.size();i++) {
            if (prio_list[i] == best_prio) {
                candidate_node.push_back(i);
                if (candidate_node.size() > 1 )
                    repeat++;
            }
        }

        if (NEW_ALG)
            if (repeat > int(mjob.taskNum * 0.5))
                return __UINT16_MAX__;

        // stop random strategy
        int idx = RAND(0, candidate_node.size());
        if (candidate_node.size() == 1 && idx == 1){
            idx = 0;
        }

        if (candidate_node.size() == 0)
            cout << "[INFO]: candidate node size is 1" << endl;
        
        
        if (avaliable_node.size() == 1) {
            chose_node = avaliable_node_GP[0][0] - 1;
        }
        else {
            chose_node = avaliable_node_GP[candidate_node[idx]][0] - 1;
        }
        
        if (chose_node == -1)
            cout << "\033[1;33m[WARNING]: No avaliable node return.\033[0m" << endl;


        // Change condition
        char executor = avaliable_node_GP[candidate_node[idx]][1];
        if (executor == Executor::ROBOT) 
            r_cur_time += mjob.task_list[chose_node].r_time;
        else if (executor == Executor::HUMAN)
            h_cur_time += mjob.task_list[chose_node].h_time;
        else if (executor == Executor::BOTH) {
            uint16_t common_time = max(r_cur_time, h_cur_time);
            r_cur_time = mjob.task_list[chose_node].r_time + common_time;
            h_cur_time = mjob.task_list[chose_node].h_time + common_time;
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
            // find out if search_node is in avaliable_node_GP
            bool is_in = false;
            for (int j = 0; j < avaliable_node_GP.size(); j++) {
                if (avaliable_node_GP[j][0] == search_node) {
                    is_in = true;
                    break;
                }
            }       

            if (!is_in && !finish_node[search_node - 1]){
                push_node[0] = search_node;
                if (mjob.task_list[search_node - 1].executor == 'r') {
                    push_node[1] = Executor::ROBOT;
                    avaliable_node_GP.push_back(push_node);
                }
                else if (mjob.task_list[search_node - 1].executor == 'h') {
                    push_node[1] = Executor::HUMAN;
                    avaliable_node_GP.push_back(push_node);
                }
                else if (mjob.task_list[search_node - 1].executor == 'b') {
                    push_node[1] = Executor::BOTH;
                    avaliable_node_GP.push_back(push_node);
                }
                else {
                    push_node[1] = Executor::ROBOT;
                    avaliable_node_GP.push_back(push_node);
                    push_node[1] = Executor::HUMAN;
                    avaliable_node_GP.push_back(push_node);
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
            bool is_in = false;

            for (int j = 0; j < avaliable_node_GP.size(); j++) {
                if (avaliable_node_GP[j][0] == search_node) {
                    is_in = true;
                    break;
                }
            }
            if (!is_in && !finish_node[search_node - 1] && and_done){
                push_node[0] = search_node;
                if (mjob.task_list[search_node - 1].executor == 'r') {
                    push_node[1] = Executor::ROBOT;
                    avaliable_node_GP.push_back(push_node);
                }
                else if (mjob.task_list[search_node - 1].executor == 'h') {
                    push_node[1] = Executor::HUMAN;
                    avaliable_node_GP.push_back(push_node);
                }
                else if (mjob.task_list[search_node - 1].executor == 'b') {
                    push_node[1] = Executor::BOTH;
                    avaliable_node_GP.push_back(push_node);
                }
                else {
                    push_node[1] = Executor::ROBOT;
                    avaliable_node_GP.push_back(push_node);
                    push_node[1] = Executor::HUMAN;
                    avaliable_node_GP.push_back(push_node);
                }
                if (PRINT) cout << "[INFO]: " << (int)search_node << " added." << endl;
            }
        }
        // find chose node from avaliable_node_GP and remove it
        for (int i = 0; i < avaliable_node_GP.size(); i++) {
            if (avaliable_node_GP[i][0] == chose_node + 1) {
                avaliable_node_GP.erase(avaliable_node_GP.begin() + i);
                i--;
            }
        }
        
        
        

        sequence.push_back(chose_node + 1);
        // // print chosen node
        // cout << "[INFO]: chose node " << (int)chose_node + 1 << endl;
        // // print executor
        // cout << "[INFO]: executor " << (int)executor << endl;

    }
    auto out = high_resolution_clock::now();
    auto dura = duration_cast<microseconds>(out - in);
    fitness_time += (double)dura.count() / (double)1000000;
    uint16_t ret = max(r_cur_time, h_cur_time);

    return ret;
}


