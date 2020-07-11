/*
Filename: mylimit.c

Student name: Hanzhang Yang
Student ID: 518030910022

this file is the module containing the gloabl variables.
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");

typedef struct
{
    uid_t uuid;
    unsigned long mmm_max;
    unsigned int time_stamp;
} MMLimits;


MMLimits *my_mm_limits;
int limit_nr;
EXPORT_SYMBOL(limit_nr);//global variables
EXPORT_SYMBOL(my_mm_limits);

static int limit_init(void)
{

    int i=0;
    limit_nr=0;
    my_mm_limits=(MMLimits*)kmalloc(sizeof(MMLimits)*3000,GFP_KERNEL);//applicate memory
    for(i=0; i<3000; i++)
    {
        my_mm_limits[i].uuid=0;
        my_mm_limits[i].mmm_max=0;
        my_mm_limits[i].time_stamp=0;
    };
    printk(KERN_INFO "module load!\n");
    return 0;
}

static void limit_exit(void)
{
    printk(KERN_INFO "module exit!\n");
}

module_init(limit_init);
module_exit(limit_exit);
