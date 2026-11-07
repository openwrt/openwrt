/* SPDX-License-Identifier: GPL-2.0 */
/*
 * PON optical transceiver ("laser") interface for MAC drivers.
 *
 * A PON MAC has to drive an optical front end it knows nothing about: it has
 * to tell it that traffic came up, hand it a way to stop the MAC burst
 * interface around a DAC write, and be able to clear a latched transmitter
 * fault.  In return it has to hear about the faults the transceiver sees and
 * cannot deal with itself, because the reactions - the state machine, the dying
 * gasp, the emergency silence - belong to the MAC.
 *
 * The interface follows the shape cfg80211 gives a wireless driver: the core
 * defines the operations, the driver implements them and registers itself, and
 * the driver exports nothing.  Which way round it is matters.  A transceiver
 * is a leaf device (an I2C part, with a calibration matrix of its own) and a
 * MAC is the core above it; if the MAC knew about the transceiver the leaf
 * would have to expose its internals for the core to reach, so the dependency
 * is deliberately the other way: the MAC declares this, the laser includes it.
 *
 * Nothing here names a part, and nothing carries a handle.  A driver that has
 * no optical front end simply does not register one, and a driver that has one
 * has only ever one, so there is nothing for either side to look up: the core
 * holds the two addresses and calls them, and holds nothing at all once the
 * front end has unregistered.
 *
 * Locking, both directions, and the two rules that keep it deadlock-free.
 *
 * Going up (xpon_alarm(), xpon_burst_ctrl()) the core takes its registry lock
 * and then a MAC lock inside - the state machine and its mutex for an alarm, a
 * register spinlock for a burst - and it holds the registry lock for the whole
 * of that, which is what lets it tell a caller that no call is in flight.
 * Going down (set_traffic, safe_circuit_reset) the core reads the front end's
 * addresses without the registry lock and calls them without it and without a
 * MAC lock, so that direction takes only the front end's own lock.
 *
 * So: the core must not call into a front end while holding its registry lock
 * or a MAC lock; the front end, in turn, may hold its own lock across
 * xpon_alarm() and xpon_burst_ctrl(), because those only add the registry lock
 * and a MAC lock below it.  That is one order, front end -> registry -> MAC,
 * and the reverse edge is the one the core is forbidden to create.  The EN7571
 * takes exactly that order from its single 1 s work item: priv->lock, then
 * xpon_burst_ctrl() -> laser_lock -> the MAC's burst register lock.  A front
 * end with more than one caller has to make sure its own lock cannot be taken
 * from a downcall, which the core makes while none of its locks is held.
 */

#ifndef _UAPI_LINUX_PON_LASER_H
#define _UAPI_LINUX_PON_LASER_H

#include <linux/types.h>

/**
 * PON_LASER_MODE_GPON - GPON, one upstream burst per T-CONT
 * PON_LASER_MODE_GPON_SYM - GPON with the symmetric rate
 * PON_LASER_MODE_EPON - EPON
 * PON_LASER_MODE_OFF - no PON traffic, stop the transmitter
 *
 * What the core asks for in set_traffic(), which carries both things at once:
 * which mode to run in, and whether traffic flows yet.  A running mode means
 * the stack above it has traffic and the transmitter may work;
 * PON_LASER_MODE_OFF is the same core with nothing to send, so the front end
 * takes its loops open and stops measuring transmit power.
 *
 * The three modes are bits rather than the SerDes mode numbers because what
 * the core needs to say is which mode it means, not how that particular part
 * numbers it.  OFF is 0xff instead, chosen so that it cannot collide with a
 * bit that means something else.
 *
 * The core imposes and the front end answers, so there is no list of what a
 * part supports to check against and no negotiation: a core that cannot run a
 * mode never asks, and a part that cannot do one says so.
 */
#define PON_LASER_MODE_GPON		BIT(0)
#define PON_LASER_MODE_GPON_SYM		BIT(1)
#define PON_LASER_MODE_EPON		BIT(2)
#define PON_LASER_MODE_OFF		0xff

/**
 * pon_laser_set_traffic_fn - run the transmitter in @mode
 * @mode: one PON_LASER_MODE_*, or PON_LASER_MODE_OFF to stop it
 *
 * The core imposes the mode, because it is the side that knows what the stack
 * above it runs and which modes that stack speaks, and it refuses a mode it
 * cannot run before asking.  What comes back is whether it can be entered now:
 * -EBUSY while the link is up on a mode change, since the mode sets the burst
 * timing the transmitter sends with and re-writing that under a running burst
 * would leave the burst described by a word that no longer matches it;
 * -EOPNOTSUPP for a mode this particular part cannot do at all; -EINVAL for a
 * value that is neither PON_LASER_MODE_OFF nor a single PON_LASER_MODE_* bit.
 * May sleep.
 */
typedef int (*pon_laser_set_traffic_fn)(u32 mode);

/**
 * pon_laser_safe_circuit_reset_fn - clear a latched transmitter fault
 *
 * Called when the transmitter is enabled again.  May sleep.
 */
typedef int (*pon_laser_safe_circuit_reset_fn)(void);

/**
 * PON_LASER_ALARM_LOS - no light coming from the PON
 * PON_LASER_ALARM_TX_FAULT - the transmitter cannot be trusted
 *
 * The mask a transceiver passes to xpon_alarm().  Two bits because the MAC
 * does two different things about them: LOS is a link event and takes the ONU
 * state down the way the MAC's own loss-of-signal interrupt takes it - O2..O4
 * back to O1, O5 to O6, and O1/O6/O7 left as they are - while TX_FAULT leaves
 * the link up and is recorded as a DACT with an RDI.  A transceiver with its own
 * loss-of-light detector and a MAC that also has one is the intended case, not a
 * duplicate: either may report first and the second one has nothing left to do.
 * Everything else the transceiver can notice - the SFF-8472 thresholds, an
 * unusable APD calibration, the watchdog restarting the loop - it deals with
 * itself, or it comes out as a transmitter that is no longer worth trusting and
 * that is the same bit.
 */
#define PON_LASER_ALARM_LOS		BIT(0)
#define PON_LASER_ALARM_TX_FAULT	BIT(1)

/**
 * xpon_register_device() - announce an optical front end to the PON core
 * @set_traffic: run the transmitter, see pon_laser_set_traffic_fn()
 * @safe_circuit_reset: clear a latched fault, see pon_laser_safe_circuit_reset_fn()
 * @mode: the PON_LASER_MODE_* the front end came up in
 *
 * Implemented by the PON core.  The front end driver calls this at probe and
 * knows nothing about the MAC: its node in the device tree says where it sits
 * on the bus and which lines it drives, and nothing links the two to each
 * other.
 *
 * This is what starts the pairing, and it is not an error that the core has
 * not probed yet - the core is normally loaded first and is sitting there
 * waiting for exactly this call.  The core takes these addresses and starts
 * what it had held off for want of an optical path; if it had not probed yet
 * it takes them when it does.  Both have to stay valid until
 * xpon_unregister_device().
 *
 * @mode is the PON_LASER_MODE_* the front end came up in, read out of its
 * own calibration before it has been told anything.
 *
 * Returns 0, or a negative errno.
 */
int xpon_register_device(pon_laser_set_traffic_fn set_traffic,
			 pon_laser_safe_circuit_reset_fn safe_circuit_reset,
			 u8 mode);

/**
 * xpon_unregister_device() - detach an optical front end
 *
 * Implemented by the PON core.  After this returns the core has stopped
 * reaching the front end and put its addresses away, so a driver that is
 * shutting down calls this before it frees anything the core could still
 * touch.
 *
 * The MAC is taken out of service before this returns, and synchronously: a
 * dying gasp goes out while the link is still up to carry it, then the queue
 * and the carrier go down, then the interface itself is unregistered and pon0 is
 * no longer there.  A MAC with no front end has no optical path and nothing left
 * to bring up, so it stops rather than being left idle.
 *
 * This is undone by xpon_register_device(), which brings the MAC back up with
 * pon0 in it again.  The core stays loaded and its device stays bound either
 * way, so a front end can come and go as often as it likes.
 */
void xpon_unregister_device(void);

/**
 * xpon_alarm() - a transceiver reporting a fault up to its MAC
 * @alarm: mask of PON_LASER_ALARM_*
 *
 * The way back up, and nothing is passed: there is one MAC and one front end,
 * so the core knows who to tell and the caller does not have to name itself.
 * Not a callback in @ops - the MAC is the core and declares it here, and the
 * transceiver calls it as soon as it sees a fault, so the MAC acts without
 * polling.  A transceiver with nothing to report never calls it.
 *
 * A no-op when no MAC has probed.  Once a front end has unregistered nothing
 * calls here, since it is the one doing the calling.
 */
void xpon_alarm(u32 alarm);

/**
 * xpon_burst_ctrl() - hold the upstream burst generator still
 * @enable: false to stop new bursts, true to let them run again
 *
 * A transceiver writes its bias and modulation DAC codes over I2C, which takes
 * long enough for the MAC to start an upstream burst in the middle of it.  The
 * code has to hold still across that burst, so the transceiver stops the
 * generator, writes, and starts it again.  Without this a burst goes out with
 * a modulation level it was never given.
 *
 * No handle, for the same reason as xpon_alarm(): one front end, one MAC.
 * Pair the calls; @enable false with no matching true leaves the upstream
 * quiet.  Safe to call from the 1 s work with the front end's own lock held,
 * and cheap: it is one register.  See the locking note at the top.
 */
void xpon_burst_ctrl(bool enable);

#endif /* _UAPI_LINUX_PON_LASER_H */
