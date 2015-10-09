/*
 * sshm.c
 * 
 * Routines for Simpler shared memory operations
 * Version : 1.0.0.
 * Date : 10 Jan 2002
 *
 */

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "sshm.h"

#define PERMS 0600

int shm_get(int key, void **start_ptr, int size)
{
	int shmid;
	shmid = shmget((key_t) key, size, PERMS | IPC_CREAT);
	(*start_ptr) = (void *) shmat(shmid, (char *) 0, 0);
	return shmid;
}


int shm_rm(int shmid)
{
	return shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);

}

