/*
Filename: set_mm_limit.c

Student name: Hanzhang Yang
Student ID: 518030910022

this file is the module that set the system call setting memory limits.
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptree 381

static int (*oldcall)(void);

typedef struct
{
    uid_t uuid;
    unsigned long mmm_max;
    unsigned int time_stamp;
} MMLimits;

extern int limit_nr;
extern MMLimits *my_mm_limits;//extern define global variables
int set_mm_limit(uid_t uid,unsigned long mm_max,unsigned int time_allow_exceed)
{
    int i=0;
    for(i=0; i<3000; i++)
    {
        if(uid==my_mm_limits[i].uuid)//find exiting limit
        {
            my_mm_limits[i].mmm_max=mm_max;
            my_mm_limits[i].time_stamp=time_allow_exceed;
            break;
        }
        if(my_mm_limits[i].uuid==0)//add a new limit
        {
            my_mm_limits[i].uuid=uid;
            my_mm_limits[i].mmm_max=mm_max;
            my_mm_limits[i].time_stamp=time_allow_exceed;
            limit_nr++;
            break;
        }
    };
    for(i=0; i<limit_nr; i++)
    {
        printk("Existing memory limitations:\n");
        if(my_mm_limits[i].mmm_max!=0)
            printk("uid=%d,mm_max=%d,time_stamp=%u\n",my_mm_limits[i].uuid,my_mm_limits[i].mmm_max,my_mm_limits[i].time_stamp);//print information
    };
    return 0;


}
static int addsyscall_init(void)
{
    long *syscall = (long*) 0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree] = (unsigned long ) set_mm_limit;
    printk(KERN_INFO "module load!\n");
    return 0;
}

static void addsyscall_exit(void)
{
    long *syscall= (long*)0xc000d8c4;
    syscall[__NR_ptree] = (unsigned long )oldcall;
    printk(KERN_INFO "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);
