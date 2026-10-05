/* 2026 Fall COSE341 Operating Systems */
/* Project 1 */
/* Jaeyong Lim */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

#define PRINT_BUFFER_SIZE 256

struct os2026_stack_node {
    int number;
    struct os2026_stack_node *next;
};
static struct os2026_stack_node *top = NULL;

static DEFINE_MUTEX(os2026_stack_lock);

/* Print the current stack from top to bottom */
static void os2026_print_stack_locked(void) {
    char buffer[PRINT_BUFFER_SIZE] = {0};
    int len = 0;
    struct os2026_stack_node *cursor = top;
    while (cursor != NULL && len < PRINT_BUFFER_SIZE - 1) {
        len += scnprintf(buffer + len, PRINT_BUFFER_SIZE - len, " -> %d", cursor->number);
        cursor = cursor->next;
    }
    printk(KERN_INFO "[os2026_stack] top%s\n", buffer);
}

/* Push */
SYSCALL_DEFINE1(os2026_push, int, val) {
    struct os2026_stack_node *newnode = kmalloc(sizeof(struct os2026_stack_node), GFP_KERNEL);
    if (newnode == NULL) {
        printk(KERN_ERR "[os2026_stack] push(%d): cannot allocate kernel memory\n", val);
        return -ENOMEM;
    }
    mutex_lock(&os2026_stack_lock);
    newnode->number = val;
    newnode->next = top;
    top = newnode;
    printk(KERN_INFO "[os2026_stack] push(%d)\n", val);
    os2026_print_stack_locked();
    mutex_unlock(&os2026_stack_lock);
    return 0;
}

/* Pop */
SYSCALL_DEFINE0(os2026_pop) {
    mutex_lock(&os2026_stack_lock);
    if (top == NULL) {
        mutex_unlock(&os2026_stack_lock);
        printk(KERN_ERR "[os2026_stack] pop(): stack is empty\n");
        return -1;
    }
    int popped_value = top->number;
    struct os2026_stack_node *tmp = top;
    top = top->next;
    kfree(tmp);
    printk(KERN_INFO "[os2026_stack] pop() = %d\n", popped_value);
    os2026_print_stack_locked();
    mutex_unlock(&os2026_stack_lock);
    return popped_value;
}

/* Clear */
SYSCALL_DEFINE0(os2026_clear) {
    mutex_lock(&os2026_stack_lock);
    struct os2026_stack_node *tmp = top;
    int removed_nodes = 0;
    while (top != NULL) {
        tmp = top;
        top = top->next;
        kfree(tmp);
        removed_nodes++;
    }
    printk(KERN_INFO "[os2026_stack] clear(): removed %d node(s)\n", removed_nodes);
    os2026_print_stack_locked();
    mutex_unlock(&os2026_stack_lock);
    return removed_nodes;
}
