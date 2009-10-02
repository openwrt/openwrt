#ifndef _IFXMIPS_PPE_PROC_H__
#define _IFXMIPS_PPE_PROC_H__

void proc_file_create(void);
void proc_file_delete(void);
int proc_read_idle_counter(char *page, char **start, off_t off, int count, int *eof, void *data);
int proc_read_stats(char *page, char **start, off_t off, int count, int *eof, void *data);

#endif
