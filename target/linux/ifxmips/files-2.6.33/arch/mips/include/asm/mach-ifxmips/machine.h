#include <asm/mips_machine.h>

enum ifxmips_mach_type {
	IFXMIPS_MACH_GENERIC,

	/* Danube/Twinpass based machines */
	IFXMIPS_MACH_EASY50712,		/* Danube evalkit */
	IFXMIPS_MACH_EASY4010,		/* Twinpass evalkit */
	IFXMIPS_MACH_ARV4519,		/* Airties WAV-221 */
	IFXMIPS_MACH_ARV4520,		/* Airties WAV-281, Arcor EasyboxA800 */
	IFXMIPS_MACH_ARV4525,		/* Speedport W502V */

	/* ASE based machines */
	IFXMIPS_MACH_EASY50601,		/* ASE wave board */

	/* AR9 based machines */
	IFXMIPS_MACH_EASY50822,		/* AR9 eval board */

	/* VR9 based machines */
	IFXMIPS_MACH_EASY80920,		/* VRX200 eval board */
};

