/*
Filename: oom.c

Student name: Hanzhang Yang
Student ID: 518030910022

This file is the module that set the system call 382. This system call will check all the process and compares them with exiting memory limits. 
If it finds some user is out of its limit, it will kill some of its process.
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
#include <linux/mm.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_ptree 382

static int (*oldcall)(void);

typedef struct
{
    uid_t uuid;
    unsigned long mmm_max;
    unsigned int time_stamp;
} MMLimits;

extern int limit_nr;
extern MMLimits *my_mm_limits;//exten define global variables

struct prinfo   //process information
{
    pid_t pid;
    unsigned long rss;
};
struct useinfo  //user information
{
    uid_t uid;
    unsigned long cur_mm;
    int pro_nr;
    struct prinfo *proarry;
};

struct useinfo *mymm;
int use_nr;

void checkout(unsigned uid,unsigned long limit)//check the limit and all users to decide whether to kill processes
{
    int i=0,j=0,maxpro=0;
    unsigned long maxrss=0;
    for(i=0; i<use_nr; i++)
    {
        //printk("%u\t%u\n",mymm[i].cur_mm,mymm[i].uid);
        if(uid==mymm[i].uid&&mymm[i].cur_mm>=limit)
        {
            //printk("find!\n");
            while(mymm[i].cur_mm>=limit)//until memory is under limit
            {
                maxrss=0;
                //printk("%d\n",mymm[i].pro_nr);
                for(j=0; j<mymm[i].pro_nr; j++)
                {
                    //printk("%u\t%lu\n",mymm[i].proarry[j].pid,mymm[i].proarry[j].rss);
                    if(mymm[i].proarry[j].rss>maxrss)
                    {
                        maxpro=j;
                        maxrss=mymm[i].proarry[j].rss;
                    }
                }
                kill_pid(find_get_pid( mymm[i].proarry[maxpro].pid), SIGTERM, 1);
                printk("A process has been killed:\n uid=%u, uRSS=%lu, mm_max=%u, pid=%u, pRSS=%lu\n", uid,
                       mymm[i].cur_mm, limit, mymm[i].proarry[maxpro].pid, maxrss);
                mymm[i].cur_mm-=maxrss;
                mymm[i].pro_nr--;
                for(j=maxpro; j<mymm[i].pro_nr; j++)
                {
                    mymm[i].proarry[j].pid=mymm[i].proarry[j+1].pid;
                    mymm[i].proarry[j].rss=mymm[i].proarry[j+1].rss;
                }
            }
        }
    }

};



void oom(int time_step)
{
    mymm=(struct useinfo*)kmalloc(sizeof(struct useinfo)*300,GFP_KERNEL);
    int i=0;
    use_nr=0;
    bool isfind=false;

    for(i=0; i<300; i++)
        mymm[i].proarry=(struct prinfo*)kmalloc(sizeof(struct prinfo)*1000,GFP_KERNEL);
    struct task_struct *task;
    struct mm_struct *mm;
    unsigned long tmprss=0;

    read_lock(&tasklist_lock);
    for_each_process(task)//gain the information of all informations
    {
        isfind=false;
        mm = get_task_mm(task);
        if (!mm)
            continue;
        tmprss=get_mm_rss(mm)<< (PAGE_SHIFT);//the unit of get_mm_rss(mm) is pages
        for(i=0; i<use_nr; i++)
        {
            if(task->cred->uid==mymm[i].uid)
            {
                mymm[i].cur_mm+=tmprss;
                mymm[i].proarry[mymm[i].pro_nr].pid=task->pid;
                mymm[i].proarry[mymm[i].pro_nr].rss=tmprss;
                mymm[i].pro_nr++;
                isfind=true;
                break;
            }

        }
        if(isfind==true)
            continue;
        mymm[use_nr].uid=task->cred->uid;
        mymm[use_nr].cur_mm=tmprss;
        mymm[use_nr].proarry[0].pid=task->pid;
        mymm[use_nr].proarry[0].rss=tmprss;
        mymm[use_nr].pro_nr=1;
        use_nr++;
    };

    for(i=0; i<limit_nr; i++)
    {
        if(my_mm_limits[i].time_stamp>=time_step)// check the time stamp
        {
            my_mm_limits[i].time_stamp-=time_step;//decrease the time stamp
            break;
        };
        checkout(my_mm_limits[i].uuid,my_mm_limits[i].mmm_max);
    }

    read_unlock(&tasklist_lock);
    for(i=0; i<300; i++)
        kfree(mymm[i].proarry);

    kfree(mymm);

}
static int addsyscall_init(void)
{
    long *syscall = (long*) 0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_ptree]);
    syscall[__NR_ptree] = (unsigned long ) oom;
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


