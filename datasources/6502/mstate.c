#include <unistd.h>
#include <errno.h>

#include "machine.h"
#include "mstate.h"

int
mstate_img_load(int fd)
{
  int rc, n=0;

  while (n < sizeof(struct mstate)) {
    rc = read(fd, &(m.vs.ms), sizeof(struct mstate) - n);
    if (rc == 0 || (rc < 0 && errno != EINTR)) break;
    n += rc;
  }
  
  return 1;
}
