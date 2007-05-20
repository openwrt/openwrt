/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     nvram interface routines.
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
 *         
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 * May 2004 rkt, neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#include <linux/ctype.h>
#include <linux/string.h>

//#include <asm/ds1553rtc.h>
#include "nvram434.h"
#define  NVRAM_BASE 0xbfff8000

extern void setenv (char *e, char *v, int rewrite);
extern void unsetenv (char *e);
extern void mapenv (int (*func)(char *, char *));
extern char *getenv (char *s);
extern void purgeenv(void);

static void nvram_initenv(void);

static unsigned char
nvram_getbyte(int offs)
{
  return(*((unsigned char*)(NVRAM_BASE + offs)));
}

static void
nvram_setbyte(int offs, unsigned char val)
{
  unsigned char* nvramDataPointer = (unsigned char*)(NVRAM_BASE + offs);

  *nvramDataPointer = val;
}

/*
 * BigEndian!
 */
static unsigned short
nvram_getshort(int offs)
{
  return((nvram_getbyte(offs) << 8) | nvram_getbyte(offs + 1));
}

static void
nvram_setshort(int offs, unsigned short val)
{
  nvram_setbyte(offs, (unsigned char)((val >> 8) & 0xff));
  nvram_setbyte(offs + 1, (unsigned char)(val & 0xff));
}
#if 0
static unsigned int
nvram_getint(int offs)
{
  unsigned int val;
  val = nvram_getbyte(offs) << 24;
  val |= nvram_getbyte(offs + 1) << 16;
  val |= nvram_getbyte(offs + 2) << 8;
  val |= nvram_getbyte(offs + 3);
  return(val);
}

static void
nvram_setint(int offs, unsigned int val)
{
  nvram_setbyte(offs, val >> 24);
  nvram_setbyte(offs + 1, val >> 16);
  nvram_setbyte(offs + 2, val >> 8);
  nvram_setbyte(offs + 3, val);
}
#endif
/*
 * calculate NVRAM checksum
 */
static unsigned short
nvram_calcsum(void)
{
  unsigned short sum = NV_MAGIC;
  int     i;

  for (i = ENV_BASE; i < ENV_TOP; i += 2)
    sum += nvram_getshort(i);
  return(sum);
}

/*
 * update the nvram checksum
 */
static void
nvram_updatesum (void)
{
  nvram_setshort(NVOFF_CSUM, nvram_calcsum());
}

/*
 * test validity of nvram by checksumming it
 */
static int
nvram_isvalid(void)
{
  static int  is_valid;

  if (is_valid)
    return(1);

  if (nvram_getshort(NVOFF_MAGIC) != NV_MAGIC) {
	printk("nvram_isvalid FAILED\n");
    //nvram_initenv();
  }
  is_valid = 1;
  return(1);
}

/* return nvram address of environment string */
static int
nvram_matchenv(char *s)
{
  int envsize, envp, n, i, varsize;
  char *var;

  envsize = nvram_getshort(NVOFF_ENVSIZE);

  if (envsize > ENV_AVAIL)
    return(0);     /* sanity */
    
  envp = ENV_BASE;

  if ((n = strlen (s)) > 255)
    return(0);
    
  while (envsize > 0) {
    varsize = nvram_getbyte(envp);
    if (varsize == 0 || (envp + varsize) > ENV_TOP)
      return(0);   /* sanity */
    for (i = envp + 1, var = s; i <= envp + n; i++, var++) {
      char c1 = nvram_getbyte(i);
      char c2 = *var;
      if (islower(c1))
        c1 = toupper(c1);
      if (islower(c2))
        c2 = toupper(c2);
      if (c1 != c2)
        break;
    }
    if (i > envp + n) {       /* match so far */
      if (n == varsize - 1)   /* match on boolean */
        return(envp);
      if (nvram_getbyte(i) == '=')  /* exact match on variable */
        return(envp);
    }
    envsize -= varsize;
    envp += varsize;
  }
  return(0);
}

static void nvram_initenv(void)
{
  nvram_setshort(NVOFF_MAGIC, NV_MAGIC);
  nvram_setshort(NVOFF_ENVSIZE, 0);

  nvram_updatesum();
}

static void
nvram_delenv(char *s)
{
  int nenvp, envp, envsize, nbytes;

  envp = nvram_matchenv(s);
  if (envp == 0)
    return;

  nenvp = envp + nvram_getbyte(envp);
  envsize = nvram_getshort(NVOFF_ENVSIZE);
  nbytes = envsize - (nenvp - ENV_BASE);
  nvram_setshort(NVOFF_ENVSIZE, envsize - (nenvp - envp));
  while (nbytes--) {
    nvram_setbyte(envp, nvram_getbyte(nenvp));
    envp++;
    nenvp++;
  }
  nvram_updatesum();
}

static int
nvram_setenv(char *s, char *v)
{
  int ns, nv, total;
  int envp;

  if (!nvram_isvalid())
    return(-1);

  nvram_delenv(s);
  ns = strlen(s);
  if (ns == 0)
    return (-1);
  if (v && *v) {
    nv = strlen(v);
    total = ns + nv + 2;
  }
  else {
    nv = 0;
    total = ns + 1;
  }
  if (total > 255 || total > ENV_AVAIL - nvram_getshort(NVOFF_ENVSIZE))
    return(-1);

  envp = ENV_BASE + nvram_getshort(NVOFF_ENVSIZE);

  nvram_setbyte(envp, (unsigned char) total); 
  envp++;

  while (ns--) {
    nvram_setbyte(envp, *s); 
    envp++; 
    s++;
  }

  if (nv) {
    nvram_setbyte(envp, '='); 
    envp++;
    while (nv--) {
      nvram_setbyte(envp, *v); 
      envp++; 
      v++;
    }
  }
  nvram_setshort(NVOFF_ENVSIZE, envp-ENV_BASE);
  nvram_updatesum();
  return 0;
}

static char *
nvram_getenv(char *s)
{
  static char buf[256];   /* FIXME: this cannot be static */
  int envp, ns, nbytes, i;

  if (!nvram_isvalid())
    return "INVALID NVRAM"; //((char *)0);

  envp = nvram_matchenv(s);
  if (envp == 0)
    return "NOT FOUND"; //((char *)0);
  ns = strlen(s);
  if (nvram_getbyte(envp) == ns + 1)  /* boolean */
    buf[0] = '\0';
  else {
    nbytes = nvram_getbyte(envp) - (ns + 2);
    envp += ns + 2;
    for (i = 0; i < nbytes; i++)
      buf[i] = nvram_getbyte(envp++);
    buf[i] = '\0';
  }
  return(buf);
}

static void
nvram_unsetenv(char *s)
{
  if (!nvram_isvalid())
    return;

  nvram_delenv(s);
}

/*
 * apply func to each string in environment
 */
static void
nvram_mapenv(int (*func)(char *, char *))
{
  int envsize, envp, n, i, seeneql;
  char name[256], value[256];
  char c, *s;

  if (!nvram_isvalid())
    return;

  envsize = nvram_getshort(NVOFF_ENVSIZE);
  envp = ENV_BASE;

  while (envsize > 0) {
    value[0] = '\0';
    seeneql = 0;
    s = name;
    n = nvram_getbyte(envp);
    for (i = envp + 1; i < envp + n; i++) {
      c = nvram_getbyte(i);
      if ((c == '=') && !seeneql) {
        *s = '\0';
        s = value;
        seeneql = 1;
        continue;
      }
      *s++ = c;
    }
    *s = '\0';
    (*func)(name, value);
    envsize -= n;
    envp += n;
  }
}
#if 0
static unsigned int
digit(char c)
{
  if ('0' <= c && c <= '9')
    return (c - '0');
  if ('A' <= c && c <= 'Z')
    return (10 + c - 'A');
  if ('a' <= c && c <= 'z')
    return (10 + c - 'a');
  return (~0);
}
#endif
/*
 * Wrappers to allow 'special' environment variables to get processed
 */
void
setenv(char *e, char *v, int rewrite)
{
  if (nvram_getenv(e) && !rewrite)
    return;
    
  nvram_setenv(e, v);
}

char *
getenv(char *e)
{
  return(nvram_getenv(e));
}

void
unsetenv(char *e)
{
  nvram_unsetenv(e);
}

void
purgeenv()
{
  int i;
  unsigned char* nvramDataPointer = (unsigned char*)(NVRAM_BASE);
  
  for (i = ENV_BASE; i < ENV_TOP; i++)
    *nvramDataPointer++ = 0;
  nvram_setshort(NVOFF_MAGIC, NV_MAGIC);
  nvram_setshort(NVOFF_ENVSIZE, 0);
  nvram_setshort(NVOFF_CSUM, NV_MAGIC);
}

void
mapenv(int (*func)(char *, char *))
{
  nvram_mapenv(func);
}
