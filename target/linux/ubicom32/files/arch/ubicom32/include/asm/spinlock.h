/*
 * arch/ubicom32/include/asm/spinlock.h
 *   Spinlock related definitions for Ubicom32 architecture.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_SPINLOCK_H
#define _ASM_UBICOM32_SPINLOCK_H

#include <asm/system.h>
#include <asm/processor.h>
#include <asm/spinlock_types.h>

/*
 * __raw_spin_lock()
 *	Lock the lock.
 */
static inline void __raw_spin_lock(raw_spinlock_t *x)
{
	asm volatile (
	"1:	bset	%0, %0, #0	\n\t"
	"	jmpne.f	1b		\n\t"
		: "+U4" (x->lock)
		:
		: "memory", "cc"
	);
}

/*
 * __raw_spin_unlock()
 *	Unlock the lock.
 */
static inline void __raw_spin_unlock(raw_spinlock_t *x)
{
	asm volatile (
	"	bclr	%0, %0, #0	\n\t"
		: "+U4" (x->lock)
		:
		: "memory", "cc"
	);
}

/*
 * __raw_spin_is_locked()
 *	Test if the lock is locked.
 */
static inline int __raw_spin_is_locked(raw_spinlock_t *x)
{
	return x->lock;
}

/*
 * __raw_spin_unlock_wait()
 *	Wait for the lock to be unlocked.
 *
 * Note: the caller has not guarantee that the lock will not
 * be acquired before they get to it.
 */
static inline void __raw_spin_unlock_wait(raw_spinlock_t *x)
{
	do {
		cpu_relax();
	} while (__raw_spin_is_locked(x));
}

/*
 * __raw_spin_trylock()
 *	Try the lock, return 0 on failure, 1 on success.
 */
static inline int __raw_spin_trylock(raw_spinlock_t *x)
{
	int ret = 0;

	asm volatile (
	"	bset	%1, %1, #0	\n\t"
	"	jmpne.f	1f		\n\t"
	"	move.4	%0, #1		\n\t"
	"1:				\n\t"
		: "+r" (ret), "+U4" (x->lock)
		:
		: "memory", "cc"
	);

	return ret;
}

/*
 * __raw_spin_lock_flags()
 *	Spin waiting for the lock (enabling IRQ(s))
 */
static inline void __raw_spin_lock_flags(raw_spinlock_t *x, unsigned long flags)
{
	mb();
	while (!__raw_spin_trylock(x)) {
		/*
		 * If the flags from the IRQ are set, interrupts are disabled and we
		 * need to re-enable them.
		 */
		if (!flags) {
			cpu_relax();
		} else {
			raw_local_irq_enable();
			cpu_relax();
			raw_local_irq_disable();
		}
	}
	mb();
}

/*
 * Read-write spinlocks, allowing multiple readers but only one writer.
 * Linux rwlocks are unfair to writers; they can be starved for an indefinite
 * time by readers.  With care, they can also be taken in interrupt context.
 *
 * In Ubicom32 architecture implementation, we have a spinlock and a counter.
 * Readers use the lock to serialise their access to the counter (which
 * records how many readers currently hold the lock).
 * Writers hold the spinlock, preventing any readers or other writers from
 * grabbing the rwlock.
 */

/*
 * __raw_read_lock()
 *	Increment the counter in the rwlock.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline void __raw_read_lock(raw_rwlock_t *rw)
{
	unsigned long flags;
	raw_local_irq_save(flags);
	__raw_spin_lock_flags(&rw->lock, flags);
	rw->counter++;
	__raw_spin_unlock(&rw->lock);
	raw_local_irq_restore(flags);
}

/*
 * __raw_read_unlock()
 *	Decrement the counter.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline void __raw_read_unlock(raw_rwlock_t *rw)
{
	unsigned long flags;
	raw_local_irq_save(flags);
	__raw_spin_lock_flags(&rw->lock, flags);
	rw->counter--;
	__raw_spin_unlock(&rw->lock);
	raw_local_irq_restore(flags);
}

/*
 * __raw_read_trylock()
 *	Increment the counter if we can.
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to grab the same read lock
 */
static inline int __raw_read_trylock(raw_rwlock_t *rw)
{
	unsigned long flags;
 retry:
	raw_local_irq_save(flags);
	if (__raw_spin_trylock(&rw->lock)) {
		rw->counter++;
		__raw_spin_unlock(&rw->lock);
		raw_local_irq_restore(flags);
		return 1;
	}

	raw_local_irq_restore(flags);

	/*
	 * If write-locked, we fail to acquire the lock
	 */
	if (rw->counter < 0) {
		return 0;
	}

	/*
	 * Wait until we have a realistic chance at the lock
	 */
	while (__raw_spin_is_locked(&rw->lock) && rw->counter >= 0) {
		cpu_relax();
	}

	goto retry;
}

/*
 * __raw_write_lock()
 *
 * Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to read_trylock() this lock
 */
static inline void __raw_write_lock(raw_rwlock_t *rw)
{
	unsigned long flags;
retry:
	raw_local_irq_save(flags);
	__raw_spin_lock_flags(&rw->lock, flags);

	if (rw->counter != 0) {
		__raw_spin_unlock(&rw->lock);
		raw_local_irq_restore(flags);

		while (rw->counter != 0)
			cpu_relax();

		goto retry;
	}

	rw->counter = -1; /* mark as write-locked */
	mb();
	raw_local_irq_restore(flags);
}

static inline void __raw_write_unlock(raw_rwlock_t *rw)
{
	rw->counter = 0;
	__raw_spin_unlock(&rw->lock);
}

/* Note that we have to ensure interrupts are disabled in case we're
 * interrupted by some other code that wants to read_trylock() this lock */
static inline int __raw_write_trylock(raw_rwlock_t *rw)
{
	unsigned long flags;
	int result = 0;

	raw_local_irq_save(flags);
	if (__raw_spin_trylock(&rw->lock)) {
		if (rw->counter == 0) {
			rw->counter = -1;
			result = 1;
		} else {
			/* Read-locked.  Oh well. */
			__raw_spin_unlock(&rw->lock);
		}
	}
	raw_local_irq_restore(flags);

	return result;
}

/*
 * read_can_lock - would read_trylock() succeed?
 * @lock: the rwlock in question.
 */
static inline int __raw_read_can_lock(raw_rwlock_t *rw)
{
	return rw->counter >= 0;
}

/*
 * write_can_lock - would write_trylock() succeed?
 * @lock: the rwlock in question.
 */
static inline int __raw_write_can_lock(raw_rwlock_t *rw)
{
	return !rw->counter;
}

#define __raw_read_lock_flags(lock, flags) __raw_read_lock(lock)
#define __raw_write_lock_flags(lock, flags) __raw_write_lock(lock)

#define _raw_spin_relax(lock)	cpu_relax()
#define _raw_read_relax(lock)	cpu_relax()
#define _raw_write_relax(lock)	cpu_relax()

#endif /* _ASM_UBICOM32_SPINLOCK_H */
