#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
// new includes for semaphores
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0xDEADBEEF

union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO */
};


int main() {
  //getting semaphore
  int semd;
  semd = semget(KEY, 1, 0);



  //DOWNING the semaphore
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;
  semop(semd, &sb, 1);

  printf("Resources are available!\n");


  //getting shared memory
  int shmid = shmget(KEY, 202, 0);

  if(shmid == -1){
    printf("Errno 39 %d : %s\n",errno , strerror(errno ));
  }

  //Pointer to shared memory
  char * shminfo = shmat(shmid, (void *)0, 0);

  //Printing last line
  if(strcmp(shminfo, "")){
    printf("The last line that was entered was:\n %s\n", shminfo);
  }


  // Prmpt user to add next line
  printf("enter what u want to add:\n");
  char line[202];
  fgets(line, 202, stdin);
  line[strlen(line) - 1] = 0;

  int storyfd = open("call.txt", O_WRONLY, 0);

  //Copying line to shared memory
  strncpy(shminfo, line, 202);

  //appending line to call.txt
  lseek(storyfd, 0, SEEK_END);

  write(storyfd, shminfo, strlen(line));
  write(storyfd, "\n", 1);

  //UPPING the semaphore
  sb.sem_op = 1;
  semop(semd, &sb, 1);

  return 0;
}
