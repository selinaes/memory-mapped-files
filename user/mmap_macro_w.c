#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/memlayout.h"
// #include "kernel/defs.h"

void mmap_micro_compare();

#define MAP_FAILED ((char *) -1)

char buf[50*PGSIZE];


char *testname = "???";

int
main(int argc, char *argv[]){
    mmap_micro_compare();
    printf("mmapevaluation: finished\n");
    exit(0);
}

void
err(char *why)
{
  printf("mmaptest: %s failed: %s, pid=%d\n", testname, why, getpid());
  exit(1);
}

//
// create a file to be mapped, containing
// 1.5 pages of 'A' and half a page of zeros.
//
void
makefile(const char *f)
{
  int i;
  int n = 50;

  unlink(f);
  int fd = open(f, O_WRONLY | O_CREATE);
  if (fd == -1)
    err("open");
  memset(buf, 'A', 50 * PGSIZE);
  // write 100 page
  for (i = 0; i < n; i++) {
    if (write(fd, buf, PGSIZE) != PGSIZE) {
      printf("current position %d",i);
      err("write 0 makefile");
    }
      
  }


  if (close(fd) == -1)
    err("close");
}


//
// check the content of the two mapped pages.
//
void
_v1(char *p)
{
  int i;
  for (i = 0; i < PGSIZE*50; i++) {
    
    if (p[i] != 'A') {
    printf("mismatch at %d, wanted 'A', got 0x%x\n", i, p[i]);
    err("v1 mismatch (1)");
    }
    
  }
}

char read_buf[50 * PGSIZE];

void
mmap_micro_compare(void)
{
  int fd1;
  int fd2;
  const char * const f1 = "mmap.dur";
  const char * const f2 = "read.dur";
  printf("mmap_test starting\n");
  testname = "mmap_test";

  // create a file with known content, map it into memory, check that
  // the mapped memory has the same bytes as originally written to the
  // file.

  //mmap
  
  makefile(f1);
  if ((fd1 = open(f1, O_WRONLY)) == -1)
    err("open");
  int bfmmap = cpu_cycle();

  char *p = mmap(0, PGSIZE*50, PROT_WRITE, MAP_SHARED, fd1, 0);
  if (p == MAP_FAILED)
    err("mmap (1)");
  for (int i = 0; i < 50; i++) {
      p[49*PGSIZE] = 'i';
      munmap(p+49*PGSIZE, 1);
  }
  int aftmmap = cpu_cycle();

  printf("mmap diff: %d\n", aftmmap - bfmmap);


  //read
  makefile(f2);
  if ((fd2 = open(f2, O_WRONLY)) == -1)
    err("open");
  int bfread = cpu_cycle();

  read(fd2, read_buf, PGSIZE*50);
  for (int i = 0; i < 50; i++) {
      read_buf[49*PGSIZE] = 'i';
      write(fd2, read_buf, PGSIZE*50);
  }
  int aftread = cpu_cycle();

  printf("read diff: %d\n", aftread - bfread);

  printf("50pages Write 1page read - mmap diff diff: %d\n", (aftread - bfread) - (aftmmap - bfmmap));
}

