#include <stdlib.h>
#include <stdio.h>
#include "../src/list.h"

struct test
{
    int val;
    struct list_head entry;
};

int main(void)
{
    struct list_head head;
    struct test *t;
    int i;
    struct list_head *pos, *next;
    INIT_LIST_HEAD(&head);
 
    for(i = 0; i < 5; i++)
    {
        t = (struct test *)malloc(sizeof(struct test));
        t->val = i;
        list_add_tail(&(t->entry), &head);
    }
 
    list_for_each(pos, &head)
    {
        t = list_entry(pos, struct test, entry);
        printf("%d\t", t->val);
    }
 
    list_for_each_safe(pos, next, &head)
    {
        t = list_entry(pos, struct test, entry);
 
        if(t->val == 2)
        {
            list_del_init(pos);
            free(t);
        }
    }
 
    printf("\n");
    list_for_each(pos, &head)
    {
        t = list_entry(pos, struct test, entry);
        printf("%d\t", t->val);
    }
 
    return 0;
}   
