#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/fs.h"
#include "user/user.h"

void mmap_micro_compare();

#define MAP_FAILED ((char *) -1)

char buf[BSIZE];


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
  int n = PGSIZE/BSIZE;

  unlink(f);
  int fd = open(f, O_WRONLY | O_CREATE);
  if (fd == -1)
    err("open");
  memset(buf, 'A', BSIZE);
  // write 1.5 page
  for (i = 0; i < n + n/2; i++) {
    if (write(fd, buf, BSIZE) != BSIZE)
      err("write 0 makefile");
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
  for (i = 0; i < PGSIZE*2; i++) {
    if (i < PGSIZE + (PGSIZE/2)) {
      if (p[i] != 'A') {
        printf("mismatch at %d, wanted 'A', got 0x%x\n", i, p[i]);
        err("v1 mismatch (1)");
      }
    } else {
      if (p[i] != 0) {
        printf("mismatch at %d, wanted zero, got 0x%x\n", i, p[i]);
        err("v1 mismatch (2)");
      }
    }
  }
}

char read_buf[2 * PGSIZE];

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
  if ((fd1 = open(f1, O_RDONLY)) == -1)
    err("open");
  int mmap_start = uptime();
  char *p = mmap(0, PGSIZE*2, PROT_READ, MAP_PRIVATE, fd1, 0);
  if (p == MAP_FAILED)
    err("mmap (1)");
  _v1(p);
  int mmap_end = uptime();
  printf("mmap_runtime: %d", mmap_end - mmap_start);

  //read
  makefile(f2);
  if ((fd2 = open(f2, O_RDONLY)) == -1)
    err("open");
  int read_start = uptime();
  read(fd2, read_buf, PGSIZE*2);
  _v1(read_buf);
  int read_end = uptime();
  printf("read_runtime: %d", read_end - read_start);
}