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


int main(int argc, char *argv[]) {
  int file;
  if (argc != 2){
    printf("Invalid arguements\n");
  }
  else{
    //create
    if(!strcmp(argv[1], "-c")){
      printf("Creating story, shared memory, and semaphore.\n");
      // shared memory
      int shmid = shmget(KEY, 202, 0644 | IPC_CREAT);
      if(shmid == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }
      // story file
      file = open("call.txt", O_CREAT | O_TRUNC, 0644);
      if(file == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }
      // semaphore
      int semid = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
      if(semid == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }
      union semun us;
      us.val = 1;
      semctl(semid, 0, SETVAL, us);
    }
    // remove
    else if(!strcmp(argv[1], "-r")){

      int semid = semget(KEY, 1, IPC_CREAT);

      // Make sure no other operations are running
      struct sembuf* sbuf = calloc(sizeof(struct sembuf), 1);
      sbuf->sem_num = 0;
      sbuf->sem_op = -1;
      sbuf->sem_flg = SEM_UNDO;
      // This should wait until memory becomes available
      int sem_op_status = semop(semid, sbuf, 1);
      free(sbuf);

      // removes semaphore
      semctl(semid, 1, IPC_RMID);
      if(semid == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }

      file = open("call.txt" , O_RDONLY);
      if(file < 0){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }


      // shows content before removal
      char * text = calloc(5000, sizeof(char));
      int rd= read(file, text, 5000);
      close(file);
      if (rd) printf("%s", text );
      else printf("----No lines added----\n");
      free(text);

      printf("\nRemoving story, shared memory, and semaphore.\n");
      // deleting shared memory
      int shmid = shmget(KEY, 200, 0644);
      if(shmid == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }

      int shm = shmctl(shmid, IPC_RMID, NULL);
      if(shm == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }
      // removes file
      int rem = remove("call.txt");
      if(rem == -1){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
      }
    }
    // view
    else if(!strcmp(argv[1], "-v")){
      printf("Viewing story :\n");
      file = open("call.txt" , O_RDONLY, 0644);
      if(file < 0){
        printf("Errno  %d : %s\n",errno , strerror(errno ));
        exit(0);
      }
      //reads file
      char * text = calloc(5000, sizeof(char));
      int rd= read(file, text, 5000);
      close(file);
      if (rd) printf("%s", text );
      else printf("----No lines added----\n");
      free(text);
    }
    else printf("Invalid command\n" );
  }

  return 0;
}
