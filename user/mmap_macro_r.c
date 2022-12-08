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

char buf[10*PGSIZE];


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
  int n = 10;

  unlink(f);
  int fd = open(f, O_WRONLY | O_CREATE);
  if (fd == -1)
    err("open");
  memset(buf, 'A', 10 * PGSIZE);
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
  for (i = 0; i < PGSIZE*10; i++) {
    
    if (p[i] != 'A') {
    printf("mismatch at %d, wanted 'A', got 0x%x\n", i, p[i]);
    err("v1 mismatch (1)");
    }
    
  }
}

char read_buf1[10 * PGSIZE];
char read_buf2[10 * PGSIZE];
char read_buf3[10 * PGSIZE];
char read_buf4[10 * PGSIZE];
char read_buf5[10 * PGSIZE];


void
mmap_micro_compare(void)
{
  int fd1;
  int fd2;
  int fd3;
  int fd4;
  int fd5;

  int fd11;
  int fd12;
  int fd13;
  int fd14;
  int fd15;

  const char * const f1 = "mmap1.dur";
  const char * const f2 = "mmap2.dur";
  const char * const f3 = "mmap3.dur";
  const char * const f4 = "mmap4.dur";
  const char * const f5 = "mmap5.dur";

  const char * const f11 = "read1.dur";
  const char * const f12 = "read2.dur";
  const char * const f13 = "read3.dur";
  const char * const f14 = "read4.dur";
  const char * const f15 = "read5.dur";

  printf("mmap_test starting\n");
  testname = "mmap_test";

  // create a file with known content, map it into memory, check that
  // the mapped memory has the same bytes as originally written to the
  // file.

  //mmap
  
  makefile(f1);
  makefile(f2);
  makefile(f3);
  makefile(f4);
  makefile(f5);

  if ((fd1 = open(f1, O_RDONLY)) == -1)
    err("open");
  if ((fd2 = open(f2, O_RDONLY)) == -1)
    err("open");
  if ((fd3 = open(f3, O_RDONLY)) == -1)
    err("open");
  if ((fd4 = open(f4, O_RDONLY)) == -1)
    err("open");
  if ((fd5 = open(f5, O_RDONLY)) == -1)
    err("open");

  int bfmmap = cpu_cycle();

  char *p1 = mmap(0, PGSIZE*10, PROT_READ, MAP_SHARED, fd1, 0);
  char *p2 = mmap(0, PGSIZE*10, PROT_READ, MAP_SHARED, fd2, 0);
  char *p3 = mmap(0, PGSIZE*10, PROT_READ, MAP_SHARED, fd3, 0);
  char *p4 = mmap(0, PGSIZE*10, PROT_READ, MAP_SHARED, fd4, 0);
  char *p5 = mmap(0, PGSIZE*10, PROT_READ, MAP_SHARED, fd5, 0);

  
  if (p1[9*PGSIZE] != 'A') err("v1 mismatch (1)");
  if (p2[9*PGSIZE] != 'A') err("v2 mismatch (1)");
  if (p3[9*PGSIZE] != 'A') err("v3 mismatch (1)");
  if (p4[9*PGSIZE] != 'A') err("v4 mismatch (1)");
  if (p5[9*PGSIZE] != 'A') err("v5 mismatch (1)");

  
  int aftmmap = cpu_cycle();

  printf("mmap diff: %d\n", aftmmap - bfmmap);


  //read
  makefile(f11);
  makefile(f12);
  makefile(f13);
  makefile(f14);
  makefile(f15);

  if ((fd11 = open(f11, O_RDONLY)) == -1)
    err("open");
  if ((fd12 = open(f12, O_RDONLY)) == -1)
    err("open");
  if ((fd13 = open(f13, O_RDONLY)) == -1)
    err("open");
  if ((fd14 = open(f14, O_RDONLY)) == -1)
    err("open");
  if ((fd15 = open(f15, O_RDONLY)) == -1)
    err("open");


  int bfread = cpu_cycle();
  read(fd11, read_buf1, PGSIZE*10);
  read(fd12, read_buf2, PGSIZE*10);
  read(fd13, read_buf3, PGSIZE*10);
  read(fd14, read_buf4, PGSIZE*10);
  read(fd15, read_buf5, PGSIZE*10);
  if (read_buf1[9*PGSIZE] != 'A') err("v1 mismatch (1)");
  if (read_buf2[9*PGSIZE] != 'A') err("v2 mismatch (1)");
  if (read_buf3[9*PGSIZE] != 'A') err("v3 mismatch (1)");
  if (read_buf4[9*PGSIZE] != 'A') err("v4 mismatch (1)");
  if (read_buf5[9*PGSIZE] != 'A') err("v5 mismatch (1)");

  int aftread = cpu_cycle();

  printf("read diff: %d\n", aftread - bfread);

  printf("50pages Write 1page read - mmap diff diff: %d\n", (aftread - bfread) - (aftmmap - bfmmap));
}

