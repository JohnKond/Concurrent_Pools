struct treeNode {
    int postID;
    struct treeNode *lc;
    struct treeNode *rc;
    int IsRightThreaded;
    int IsLeftThreaded;
    pthread_mutex_t lock;
};

extern  int N;
extern  int M;
struct treeNode *root = NULL;


int BST_Insert(int pid){ 
    struct treeNode *curr,*temp,*par=NULL; 

    if (root) pthread_mutex_lock(&root->lock);    
    curr = root;
    if (root) pthread_mutex_unlock(&root->lock);

    // if (curr) pthread_mutex_lock(&curr->lock);    
    while(curr){
        if (curr->postID == pid) {
            printf("Key already exists\n");
            return 0;
        }
        par = curr;
        if (curr->postID < pid){
            if (curr->IsRightThreaded == 0){
            // pthread_mutex_unlock(&par->lock);
            //par = curr;
            curr = curr->rc;
            // pthread_mutex_lock(&curr->lock);
            }
            else break;
        }
        else {
            if(curr->IsLeftThreaded == 0){
            // pthread_mutex_unlock(&par->lock);
            par = curr;
            curr = curr->lc;
            // pthread_mutex_lock(&curr->lock);
            }
            else break;
        }
    }


    if (par) pthread_mutex_lock(&par->lock);

    temp = (struct treeNode*)malloc(sizeof(struct treeNode));
    temp->postID = pid;
    temp->IsLeftThreaded = 1;
    temp->IsRightThreaded = 1;
    if (par == NULL){
        root = temp;
        temp->lc = NULL;
        temp->rc = NULL;
    }else if (par->postID > pid){
        temp->lc = par->lc;
        temp->rc = par;
        par->IsLeftThreaded = 0;
        par->lc = temp;
    }else {
        temp->lc = par;
        temp->rc = par->rc;
        par->IsRightThreaded = 0;
        par->rc = temp;
    }
    if (par) pthread_mutex_unlock(&par->lock);
    return 1;
}


struct treeNode *Predecessor(struct treeNode *curr){

    struct treeNode *par;
    if (curr->IsLeftThreaded == 1) return curr->lc;
    
    
    par = curr;
    curr = curr->lc;
    pthread_mutex_lock(&curr->lock);
    while (curr->IsRightThreaded == 0){
        if (par) pthread_mutex_unlock(&par->lock);
        par = curr;
        curr = curr->rc;
        pthread_mutex_lock(&curr->lock);
    }
    pthread_mutex_unlock(&par->lock);
    pthread_mutex_unlock(&curr->lock);
    return curr;
}

struct treeNode *Successor(struct treeNode *curr) 
{    
    struct treeNode *par;
    // If rthread is set, we can quickly find 
    if (curr -> IsRightThreaded == 1) 
        return curr->rc; 
  
    // Else return leftmost child of right subtree. 
    par = curr;
    curr = curr -> rc;
    // pthread_mutex_lock(&curr->lock);
    while (curr -> IsLeftThreaded == 0) {
        if (par) pthread_mutex_unlock(&par->lock);
        par = curr;
        curr = curr -> lc;
        pthread_mutex_lock(&curr->lock);
    }

    pthread_mutex_unlock(&par->lock);
    pthread_mutex_unlock(&curr->lock);   
    return curr; 
} 

/*
1st Case: Node to be deleted is Leaf 
*/
void firstCase(struct treeNode *par,struct treeNode *curr){
    
    
    if (par == NULL) root == NULL;
    else if (curr == par->lc){
        par->IsLeftThreaded = 1;
        par->lc = curr->lc;
    }else {
        par->IsRightThreaded = 1;
        par->rc = curr->rc;
    }

    pthread_mutex_destroy(&curr->lock);
    free(curr);
}



/*
2nd Case: Node to be deleted has only one child 
*/
void secCase(struct treeNode *par,struct treeNode *curr){
    struct treeNode *child,*pred,*succ,*temp;

    
    if (curr->IsLeftThreaded == 0) {
        printf("Waiting for curr lc\n");
        // pthread_mutex_lock(&curr->lc->lock);
        child = curr->lc;
    }else{
        printf("Waiting for curr rc\n");
        // pthread_mutex_lock(&curr->rc->lock);
        child = curr->rc;
    }

    // pthread_mutex_unlock(&child->lock);

    printf("Passed upper locks!\n");
    // pthread_mutex_lock(&par->lock);
    if (par == NULL) root = child;
    else if (curr == par->lc) par->lc = child;
    else par->rc = child;
    // pthread_mutex_unlock(&par->lock);

    
    printf("Stuck in pred/curr\n");

    // pthread_mutex_lock(&curr->lock);

    pred = Predecessor(curr);
    succ = Successor(curr);

    printf("Passed pred/curr\n");

    if (curr->IsLeftThreaded == 0) pred->rc = succ;
    else 
        if (succ->IsRightThreaded == 0) succ->lc = pred;
    // if (par) pthread_mutex_unlock(&par->lock);
    
    pthread_mutex_destroy(&curr->lock);
    free (curr);
}


/*
3rd Case: Node to be deleted has two children 
*/
void thirdCase(struct treeNode *par, struct treeNode *curr){
    struct treeNode *suc = curr->rc;
    struct treeNode *parent_suc = curr;

    // pthread_mutex_lock(&suc->lock);

    while(suc->lc){
        pthread_mutex_unlock(&parent_suc->lock);
        parent_suc = suc;
        suc = suc->lc;        
        if (suc) pthread_mutex_lock(&suc->lock);
    }

    curr->postID = suc->postID;

    if (suc->IsRightThreaded == 1 && suc->IsLeftThreaded == 1) {
        printf("Third->First\n");
        firstCase(parent_suc,suc);
    }
    else {
        printf("Third->Second\n");
        secCase(parent_suc,suc);
    }
}



void BST_Delete(int pid){
    struct treeNode *par=NULL,*curr;
    
    printf("Start delete pid %d\n",pid);

    if (root) pthread_mutex_lock(&root->lock);    
    curr = root;
    par = root;

    while (curr){
        if (pid == curr->postID){
            break;
        }

        if (pid > curr->postID){
            if (curr->IsRightThreaded == 0){
                if (par) pthread_mutex_unlock(&par->lock);
                par = curr;
                curr = curr->rc;
                if (curr) pthread_mutex_lock(&curr->lock);
            }
            else break;
        } else {
            if (curr->IsLeftThreaded  == 0){
                if (par) pthread_mutex_unlock(&par->lock);
                par = curr;
                curr = curr->lc;
                if (curr) pthread_mutex_lock(&curr->lock);
            }
            else break;
        }
    }



    if (curr->IsRightThreaded == 0 && curr->IsLeftThreaded == 0) {
        printf("Third case, pid : %d\n",pid);
        thirdCase(par,curr);
    }
    else if (curr->IsLeftThreaded == 0) {
        printf("Second case, pid : %d\n",pid);
        secCase(par,curr);
    }
    else if (curr->IsRightThreaded == 0){
        printf("Second case, pid : %d\n",pid);
        secCase(par,curr);
    }
    else {
        printf("First case, pid : %d\n",pid);
        firstCase(par,curr);
    }

    if (par) pthread_mutex_unlock(&par->lock);
    if (curr) pthread_mutex_unlock(&curr->lock);
}


int BST_Search(int pid){
    int found=0;
    struct treeNode *ptr,*par;
    if (root) (&root->lock);
    ptr = root;
    par = ptr;
    if (ptr == NULL) printf("Tree is empty\n");
    
    while (ptr) {
        // pthread_mutex_unlock(&ptr->lock);
        if (pid == ptr->postID) {
            found = 1;
            break;
        }
        
        if (pid < ptr->postID) {
            if (ptr->IsLeftThreaded == 0) {
                if (par) pthread_mutex_unlock(&par->lock);
                par = ptr;
                ptr = ptr->lc;
                if (ptr) pthread_mutex_lock(&ptr->lock);
            }
            else break;
        }else {
            if (ptr->IsRightThreaded == 0) {
                if (par) pthread_mutex_unlock(&par->lock);
                par = ptr;
                ptr = ptr->rc;
                if (ptr) pthread_mutex_lock(&ptr->lock);
            }
            else break;
        }
    }

    if (par) pthread_mutex_unlock(&par->lock);
    if (ptr) pthread_mutex_unlock(&ptr->lock);
    if (found) return 1;
    return 0;
}






int BST_count(int delete_flag) { 
    struct treeNode *curr = root;
    int counter=0,expected_count;

    if (delete_flag) expected_count = 0;
    else expected_count = N*N;


    if (root == NULL) printf("Tree is empty\n"); 

    // Reach leftmost node  
    while (curr -> IsLeftThreaded == 0) 
        curr = curr -> lc; 
  
    // One by one print successors 
    while (curr != NULL) 
    { 
        counter ++;
        // printf("%d\n",curr -> postID); 
        curr = Successor(curr); 
    } 
    printf("Tree's total size finished (expected: %d, found: %d)\n",expected_count,counter);
    if (counter != expected_count) return 0;
    return 1;
} 


struct treeNode* inSucc(struct treeNode *curr)
{
    // If rthread is set, we can quickly find
    if (curr->IsRightThreaded == 1)
        return curr->rc;
 
    // Else return leftmost child of right subtree
    curr = curr->rc;
    while (curr->IsLeftThreaded == 0)
        curr = curr->lc;
    return curr;
}

//To be deleted
void inorder() 
{ 
    if (root == NULL) 
        printf("Tree is empty"); 
  
    // Reach leftmost node 
    struct treeNode *ptr = root; 
    while (ptr -> IsLeftThreaded == 0) 
        ptr = ptr -> lc; 
  
    // One by one print successors 
    while (ptr != NULL) 
    { 
        printf("%d\n",ptr -> postID); 
        ptr = inSucc(ptr); 
    } 
} 