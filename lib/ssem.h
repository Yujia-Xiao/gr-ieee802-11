/*
 * ssem.h
 *
 * Header file for "Simple Semaphore Operations"
 * Version : 1.0.0
 * Date: 10 Jan 2002
 * Modifications: 10 Jan 2004 Updated for C++
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Functions available to the user */

int sem_create(int key, int initial_value);
int sem_open(int key);
int sem_wait(int semid);
int sem_signal(int semid);
int sem_rm(int semid);

#ifdef __cplusplus
};
#endif

