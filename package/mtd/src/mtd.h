#ifndef __mtd_h
#define __mtd_h

extern int quiet;
extern int mtdsize;
extern int erasesize;

extern int mtd_open(const char *mtd);
extern int mtd_check_open(char *mtd);
extern int mtd_erase_block(int fd, int offset);
extern int mtd_write_buffer(int fd, char *buf, int offset, int length);
extern int mtd_write_jffs2(char *mtd, char *filename, char *dir);

#endif /* __mtd_h */
