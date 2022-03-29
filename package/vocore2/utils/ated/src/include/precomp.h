#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/select.h>
#ifdef DBG
#include <stdarg.h>
#endif // DBG //

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ate.h"
#include "cmd_io/eth.h"	/*For Ethernet*/
#include "os_ioctl/ioctl2driver.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#ifdef MT_ATED_THREAD
#include <pthread.h>
#include "thread_model/multi_thread.h"
#elif MT_ATED_SHM
#include "thread_model/multi_fork.h"
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#elif MT_ATED_IPC_SOCK
#include "thread_model/ipc_socket.h"
#endif

#ifdef MT_ATED_APSOC
#include "linux/autoconf.h"
#include "nvram.h"
#endif

