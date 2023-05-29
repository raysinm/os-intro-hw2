#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/err.h>

void get_heaviest_descendant_aux(struct task_struct* curr_tsk, long* max_w, long* max_pid);


asmlinkage long sys_hello(void) {
    printk("Hello, World!\n");
    return 0;
}

asmlinkage long sys_set_weight(int weight){
    if (weight < 0){
        return -EINVAL;
    }
    current->weight = weight;
    return 0;
}

asmlinkage long sys_get_weight(void){
    long p_weight = current->weight;
    return p_weight;
}


asmlinkage long sys_get_ancestor_sum(void){
    long sum = 0;

    struct task_struct* tsk = current;
    while(tsk->pid != 1){
        sum += tsk->weight;
        tsk = tsk->parent;
    }

    return sum;

}

void get_heaviest_descendant_aux(struct task_struct* curr_tsk, long* max_w, long* max_pid){
    /*Helper function for sys_get_heaviest_descendant*/
    struct task_struct* child;

    if (list_empty(&curr_tsk->children)){
        return;
    }

    
    list_for_each_entry(child, &curr_tsk->children, sibling){
        if (child->weight > *max_w || (child->weight==*max_w && child->pid < *max_pid)){
            *max_w = child->weight;
            *max_pid = child->pid;
        }
        get_heaviest_descendant_aux(child, max_w, max_pid);
    }

    return;
    
}

asmlinkage long sys_get_heaviest_descendant(void){
    
    long max_weight = -1;
    long max_w_pid = -1;

    get_heaviest_descendant_aux(current, &max_weight, &max_w_pid);
    if (max_weight == -1){
        return -ECHILD;
    }

    return max_w_pid;
}