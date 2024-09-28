
int minSize;
int currentsize;
struct MemNode*currentmemAdress;
struct MemNode * Memroot;
void initMemo();
void traverseAndGetMin(struct MemNode * p,struct Process *process);
int allocate(struct Process *process);
void forktillfind(struct MemNode * currentmemAdress,struct Process *process);
void freeMemory(struct MemNode * p);
void printmemory(struct MemNode*x)
{
    if(x==NULL)
    {
        return ;
    }
    if(x->left==NULL && x->right==NULL)
    {
        printf("start:%d end:%d size:%d status:%d usedsize: %d\n ",x->startidx,x->endidx,x->size,x->free,x->usedsize);
    }
    printmemory(x->left);
    printmemory(x->right);
}

void initMemo()
{
    Memroot=malloc(sizeof(struct MemNode));
    Memroot->startidx=0;
    Memroot->endidx=1023;
    Memroot->free=1;
    Memroot->left=NULL;
    Memroot->right=NULL;
    Memroot->Parent=NULL;
    Memroot->size=1024;
    Memroot->usedsize=0;
}

int allocate(struct Process *process)
{
    currentmemAdress=NULL;
    minSize=1025;
    traverseAndGetMin(Memroot,process);
    if(currentmemAdress==NULL)
    {
        return 0;
    }
    forktillfind(currentmemAdress,process);
   
    return 1;
}
void traverseAndGetMin(struct MemNode * p,struct Process *process)
{
    if(p->left==NULL && p->right==NULL)
    {
        if(p->free==1 && process->memorySize <= p->size && p->size <minSize)
        {
            currentmemAdress=p;
            minSize=p->size;
        }
        return;
    }
    traverseAndGetMin(p->left,process);
    traverseAndGetMin(p->right,process);
}
void forktillfind(struct MemNode * p,struct Process *process)
{
    while(p->size/2>=process->memorySize)
    {
        if(p->size==8)
        {
            break;
        }
        struct MemNode * right=malloc(sizeof(struct MemNode));
        struct MemNode * left=malloc(sizeof(struct MemNode));
        p->right=right;
        p->left=left;
        left->startidx=p->startidx;
        left->endidx=(p->endidx+p->startidx)/2;
        right->startidx=left->endidx+1;
        right->endidx=p->endidx;
        left->size=right->size=p->size/2;
        right->Parent=p;
        left->Parent=p;
        left->free=1;
        right->free=1;
        left->usedsize=0;
        right->usedsize=0;
        left->left=NULL;
        left->right=NULL;
        right->left=NULL;
        left->left=NULL;
        p=p->left;
    }
    p->free=0;
    p->usedsize+=p->size;
    struct MemNode * parent=p->Parent;
    while(parent)
    {
        parent->usedsize+=p->size;
        parent=parent->Parent;
    }
    process->myadress=p;
    return;
}
void freeMemory(struct MemNode * p)
{
    struct MemNode * parent;
    struct MemNode * parentred;
    int red=0;
    while(true)
    {
        p->free=1;
        p->usedsize=0;
        parentred=p->Parent;
        while(parentred!=NULL && red==0)
        {
            parentred->usedsize-=p->size;
            parentred=parentred->Parent;
        }
        red=1;
        parent=p->Parent;
        if(parent==NULL)
        {
            return;
        }

        if(parent->left->usedsize==0 && parent->right->usedsize==0)
        {
            free(parent->left);
            free(parent->right);
            parent->left=NULL;
            parent->right=NULL;
            p=parent;
        }
        else
        {
            break;
        }
    }
}