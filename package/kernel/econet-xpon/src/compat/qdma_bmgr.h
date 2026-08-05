/* compat shim: the real definitions now come from the vendor ecnt_hook QDMA
 * headers. Forward to them (include-guarded) to avoid duplicate definitions. */
#ifndef _COMPAT_QDMA_BMGR_H_
#define _COMPAT_QDMA_BMGR_H_
#include <ecnt_hook/ecnt_hook_qdma.h>
#endif
