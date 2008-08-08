#ifndef __mtd_h
#define __mtd_h

#define JFFS2_EOF "\xde\xad\xc0\xde"

extern int quiet;
extern int mtdsize;
extern int erasesize;

extern int mtd_open(const char *mtd);
extern int mtd_check_open(const char *mtd);
extern int mtd_erase_block(int fd, int offset);
extern int mtd_write_buffer(int fd, const char *buf, int offset, int length);
extern int mtd_write_jffs2(const char *mtd, const char *filename, const char *dir);
extern int mtd_replace_jffs2(int fd, int ofs, const char *filename);
extern void mtd_parse_jffs2data(const char *buf, const char *dir);

#endif /* __mtd_h */
