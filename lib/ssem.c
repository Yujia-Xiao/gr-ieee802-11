/*
 * ssem.c
 * 
 * Version 1.0.0
 * Date : 10 Jan 2002
 *
 */


#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#include "ssem.h"

#define PERMS 0600

static struct sembuf op_lock[1] = {
        0, -1, 0
};

static struct sembuf op_unlock[1] = {
        0, 1, IPC_NOWAIT
};



int sem_create(int key,int initval)
{
        int semid,i;
        semid = semget((key_t)key, 1, IPC_CREAT | PERMS);

	for(i=0;i<initval;i++)
        	semop(semid,&op_unlock[0],1);
		

      return semid;

}

int sem_open(int key)
{
        int semid;
        semid = semget(key,0,0);
        return semid;
}


int sem_wait(int semid)
{
        return semop(semid,&op_lock[0],1);
}


int sem_signal(int semid)
{
        return semop(semid,&op_unlock[0],1);
}


int sem_rm(int semid)
{
        return semctl(semid, 0, IPC_RMID, 0);
}
