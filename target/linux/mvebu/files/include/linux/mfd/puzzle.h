/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _LINUX_PUZZLE_H_
#define _LINUX_PUZZLE_H_

struct puzzle;
int puzzle_led(struct puzzle *pz, u8 ledn, u8 ledmode);
int puzzle_fan(struct puzzle *pz, u8 speed);
int puzzle_buzzer(struct puzzle *pz, u8 len);

#endif /* _LINUX_PUZZLE_H_ */
