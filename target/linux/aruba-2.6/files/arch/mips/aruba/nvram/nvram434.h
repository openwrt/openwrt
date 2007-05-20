/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     nvram definitions.
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


#ifndef _NVRAM_
#define _NVRAM_
#define NVOFFSET        0                 /* use all of NVRAM */

/* Offsets to reserved locations */
              /* size description */
#define NVOFF_MAGIC     (NVOFFSET + 0)    /* 2 magic value */
#define NVOFF_CSUM      (NVOFFSET + 2)    /* 2 NVRAM environment checksum */
#define NVOFF_ENVSIZE   (NVOFFSET + 4)    /* 2 size of 'environment' */
#define NVOFF_TEST      (NVOFFSET + 5)    /* 1 cold start test byte */
#define NVOFF_ETHADDR   (NVOFFSET + 6)    /* 6 decoded ethernet address */
#define NVOFF_UNUSED    (NVOFFSET + 12)   /* 0 current end of table */

#define NV_MAGIC        0xdeaf            /* nvram magic number */
#define NV_RESERVED     6                 /* number of reserved bytes */

#undef  NVOFF_ETHADDR
#define NVOFF_ETHADDR   (NVOFFSET + NV_RESERVED - 6)

/* number of bytes available for environment */
#define ENV_BASE        (NVOFFSET + NV_RESERVED)
#define ENV_TOP         0x2000
#define ENV_AVAIL       (ENV_TOP - ENV_BASE)

#endif /* _NVRAM_ */


