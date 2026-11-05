// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON - the optical front end: registry, binding and traffic channel
 *
 * Implements the core half of <linux/pon_laser.h>.  A transceiver driver hands
 * over two function addresses here; the core keeps them for as long as the
 * front end is registered and calls nothing at all once it is not.
 *
 * Everything about the front end is in this one file, in three parts:
 *
 *   - the registry state and the four exported entry points below,
 *   - the binding, an7552_laser_attached()/an7552_laser_detached(): what the
 *     core takes on and gives back when the two meet and part,
 *   - the traffic channel, an7552_phy_set_traffic() and its helpers: the mode
 *     the front end is told about and whether user traffic is flowing.
 *
 * None of that is the SerDes transmit side (an7552_phy_tx.c writes the burst
 * shaping registers) nor PHY bring-up (an7552_phy_main.c maps the windows);
 * the line the front end shares with the SerDes is the only thing that ties
 * the three together, and it is what this file owns.
 *
 * The interface is declared there and names no part, so a build without an
 * optical front end simply never registers one.
 *
 * Four symbols leave this module, all called by the transceiver driver and
 * none by the kernel: xpon_register_device() and xpon_unregister_device() to
 * attach and detach, and xpon_alarm() and xpon_burst_ctrl() to call back up.
 * The transceiver's own functions stay private to it - the dependency runs
 * from the leaf device up to the core, the way a wireless driver depends on
 * cfg80211.
 *
 * Nothing links the two sides in the device tree: each node describes only
 * its own device.  They find each other by calling in, which is also why
 * neither of them looks the other up.  There is one MAC and one front end on
 * this hardware, so each side is a single pointer rather than a list, and
 * whichever loads second is what brings the pair together.
 *
 *   - the MAC announces itself when it has probed (an7552_laser_core_ready,
 *     internal), and waits.  Its probe succeeds either way; what waits is the
 *     part that needs an optical path.
 *   - the front end announces itself when it loads, and that is the moment
 *     the MAC picks it up and starts.
 *
 * Both orders occur, so the second one to arrive does the binding.
 */

#include <linux/device.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>

#include <linux/pon_laser.h>

#include "an7552_mac.h"
#include "an7552_phy.h"

/*
 * Guards the three pointers.  Each is written once, at its owner's probe or
 * registration, and read on every call the other side makes; the lock is here
 * so that a front end appearing at the same moment the MAC is going away cannot
 * half-bind.
 *
 * Lock order.  It is held across the calls the front end makes up into the
 * MAC, so it sits in the middle of this chain:
 *
 *   en7571 priv->lock -> laser_lock -> x->reg_lock
 *       (the DDMI tick, xpon_burst_ctrl() -> an7552_mac_burst_ctrl())
 *   laser_lock -> x->lock
 *       (xpon_alarm() -> an7552_mac_laser_fault())
 *   x->lock -> tx_reg_lock -> the regulator core
 *       (an7552_trans_power_switch())
 *
 * The chain is acyclic only because the MAC never takes an en7571 lock while
 * it holds x->lock.  en7571_tx_set(), which the regulator reaches, only drives
 * the GPIOs and does not take priv->lock, so the third edge stops
 * there; the debugfs tx_fault_reset action runs under state_lock, not x->lock,
 * and an7552_phy_set_traffic() is only ever called with x->lock dropped.  A
 * new caller that reaches the front end's set_traffic() or
 * safe_circuit_reset() with x->lock held closes the cycle.
 */
static DEFINE_MUTEX(laser_lock);

/* The core, once it has probed.  NULL until then and again after it is gone. */
static struct an7552_mac *xpon_mac;

/*
 * The front end, once it has registered.  There is only ever one, so this is
 * what it came to the core with and nothing else; NULL is how "no front end"
 * says itself, and the MAC side keeps the same two fields NULL for as long as
 * these are.
 */
static pon_laser_set_traffic_fn xpon_set_traffic;
static pon_laser_safe_circuit_reset_fn xpon_safe_circuit_reset;
static u8 xpon_front_end_mode;

/* ---- binding: the core taking a front end on and putting it away ---- */
/* an7552_laser_mode_of(), the mode mapping; with the traffic channel below */
static u32 an7552_laser_mode_of(u32 mode);


/*
 * Everything that only makes sense with an optical front end present.
 *
 * Nothing in the device tree connects the two sides: the EN7571's own node
 * says where it sits on I2C and which lines it drives, and the xPON node says
 * what it is.  The front end driver announces itself with
 * xpon_register_device() and this core picks it up, which is the whole
 * binding - the handle both sides pass to each other afterwards is the front
 * end's own.
 *
 * So there is nothing here to look up, only to wait for, and waiting is not a
 * probe failure.  This core probes either way; what it holds off is everything
 * that needs an optical path, and an7552_laser_attached() takes it on when a
 * front end shows up.
 *
 * The transceiver pin polarities are deliberately not here.  They are board
 * wiring of this node rather than anything the front end owns, and
 * Mode_Config_7552() derives rg_tdc_los_inv and rg_ben_inv from them before the
 * SerDes settles into a link, so they are read at probe
 * (an7552_phy_probe()) and a link that came up before the front end arrived
 * does not have to be restarted over them.
 */
static int an7552_laser_setup(struct an7552_phy *priv)
{
	int ret;

	/*
	 * The transmitter line is the same device seen as a regulator, which is
	 * what the en7571 driver registers it as.  Reference counting and the
	 * regulator debugfs come with that, and no front end entry point is
	 * needed to drive the line.
	 *
	 * Only the regulator, and only here: asked for at probe this lookup
	 * would defer against the EN7571 (laser-supply names a device whose
	 * driver has not loaded yet), and a platform device that waits for its
	 * optical front end would leave no pon0, no netlink and nothing for
	 * userspace to report the absence with.  Here the front end is by
	 * definition already there, so the lookup cannot defer and a failure is
	 * a real one.
	 *
	 * Deliberately not devm: this reference has to be given back when the
	 * front end detaches, not when this platform device goes away.  The
	 * front end is a module of its own and names itself as the regulator's
	 * owner, and _regulator_get() takes a reference on that module for as
	 * long as this one is held (core.c: try_module_get(rdev->owner)).
	 * Held across a detach, it is what keeps en7571 loaded for good, and
	 * regulator_unregister() on the front end's way out finds open_count
	 * still set and warns.  an7552_laser_detached() gives it back.
	 */
	if (!priv->tx_rdev) {
		priv->tx_rdev = regulator_get_optional(priv->dev, "laser");
		if (IS_ERR(priv->tx_rdev)) {
			ret = PTR_ERR(priv->tx_rdev);
			priv->tx_rdev = NULL;
			return dev_err_probe(priv->dev, ret,
					     "optical front end regulator\n");
		}
	}

	return 0;
}

/*
 * Bind a front end that announced itself.
 *
 * Called by the registry whenever the two meet, whichever side loaded second:
 * from an7552_laser_core_ready() if the front end got here first, and from
 * xpon_register_device() if it did not.  Idempotent, because either path may
 * reach it and neither knows what the other did.
 *
 * Everything the probe could not do without a front end, done here: the
 * transmitter regulator, which is the one thing that cannot be had earlier.
 * devm_regulator_get_optional() does not swallow -EPROBE_DEFER - the regulator
 * core hands back anything that is not -ENODEV, get_type notwithstanding - and
 * this core probes before the EN7571 driver is loaded, so asking at probe would
 * defer the whole platform device until an optical front end appeared.  Here it
 * is by definition already there.
 *
 * Nothing else is left over, and in particular the SerDes is not restarted.
 * an7552_mac_open() refuses to start the MAC while no front end is registered,
 * so mac->started cannot be true at this point - never on a first attach, and
 * after a detach an7552_mac_down() has already cleared it.  That is what makes
 * the regulator sufficient on its own: there is no transmitter power to restore
 * and no link to rebuild.
 */
static void an7552_laser_attached(struct an7552_mac *mac,
			   pon_laser_set_traffic_fn set_traffic,
			   pon_laser_safe_circuit_reset_fn safe_circuit_reset,
			   u8 mode)
{
	struct an7552_phy *priv;
	int ret;

	if (!mac || !mac->phy)
		return;

	priv = mac->phy;

	/* the platform device itself is gone; there is nothing left to bring up */
	if (an7552_mac_is_gone(mac)) {
		dev_err(priv->dev, "optical front end arrived after the MAC "
				   "was given up\n");
		return;
	}

	/* shutdown has already given the netdev back; there is none to put back */
	if (!READ_ONCE(mac->ndev)) {
		dev_err(priv->dev, "optical front end arrived before the MAC "
				   "registered pon0\n");
		return;
	}

	if (priv->laser_set_traffic)
		return;			/* already bound */

	ret = an7552_laser_setup(priv);
	if (ret) {
		dev_err(priv->dev, "optical front end: %d\n", ret);
		return;
	}

	/*
	 * Before the MAC comes up: an7552_mac_up() registers pon0, and from that
	 * moment a userspace that had been waiting for it can open it and drive
	 * the transmitter.  The addresses have to be in place first, or that
	 * would be a call into a front end that was never told anything.
	 */
	WRITE_ONCE(priv->laser_set_traffic, set_traffic);
	WRITE_ONCE(priv->laser_safe_circuit_reset, safe_circuit_reset);

	/*
	 * Not an error to disagree: the part runs in the mode its calibration
	 * names until the stack hands one over, and it does so before the
	 * transmitter is ever enabled.  Worth saying out loud, because the two
	 * come from different places - this one out of the part's flash, the
	 * other out of the configuration - and nothing else would report it.
	 */
	if (mode != an7552_laser_mode_of(mac->mode))
		dev_info(priv->dev, "front end came up in mode %u, stack runs in %u\n",
			 mode, mac->mode);

	/*
	 * A previous front end may have taken the MAC down, which took pon0 out
	 * of the kernel with it.  Putting both back is what makes this
	 * registration start the machine again rather than only supply its
	 * addresses.
	 */
	if (an7552_mac_is_down(mac)) {
		ret = an7552_mac_up(mac);
		if (ret) {
			dev_err(priv->dev, "MAC restart: %d\n", ret);
			WRITE_ONCE(priv->laser_set_traffic, NULL);
			WRITE_ONCE(priv->laser_safe_circuit_reset, NULL);
			return;
		}
	}

	dev_info(priv->dev, "optical front end attached\n");
}

/*
 * Put a front end away, which takes the MAC out of service with it.
 *
 * A MAC with no front end has no optical path, so there is nothing it could be
 * asked to do that would work: pon0 leaves the kernel and the MAC stops being
 * functional, rather than sitting there taking commands it cannot carry out.
 * That is an7552_mac_down()'s whole job - dying gasp, link down, interface
 * unregistered - and it is reversible, so a front end that registers again
 * brings both back.  It is synchronous, so by the time the front end driver has
 * freed its structures the MAC is no longer reaching for them.
 *
 * The addresses go to NULL around it, not after: everything the teardown below
 * runs through them, so they have to still be there while it happens and gone
 * once it has, and a thread arriving in between finds an address the driver is
 * about to free.  Keeping them through the teardown is what lets the transmitter
 * actually be powered down.
 *
 * The core itself stays probed and stays loaded, as does the platform device -
 * an7552_mac_shutdown() has not run, so the workqueue, the interrupts and the
 * netdev allocation are all still there for the next registration to use.
 */
static void an7552_laser_detached(struct an7552_mac *mac)
{
	struct an7552_phy *priv;

	if (!mac || !mac->phy)
		return;

	priv = mac->phy;

	an7552_mac_down(mac);

	WRITE_ONCE(priv->laser_set_traffic, NULL);
	WRITE_ONCE(priv->laser_safe_circuit_reset, NULL);

	/*
	 * laser_work is on system_wq, which the teardown above does not flush: a
	 * run that read the address before it went to NULL could still be on its
	 * way into the front end's module text.  The pointers are NULL, so a
	 * run queued after this finds nothing to call.
	 */
	cancel_work_sync(&priv->laser_work);

	/*
	 * The transmitter line, last, once nothing here can reach the front end
	 * any more.  This is the one reference this module holds on it, and the
	 * front end is a module: giving it back is what lets en7571 be unloaded
	 * at all.  Left held, regulator_unregister() over on that side finds the
	 * open_count still set and warns, and the module is never free.
	 * an7552_tx_reg_put() disables the line before dropping the reference.
	 */
	an7552_tx_reg_put(priv);

	dev_info(priv->dev, "optical front end detached\n");
}

/*
 * Announce the core.  Called by the MAC once its probe has set up everything
 * the front end will need, and only then: attaching takes the regulator core
 * and the PHY lock, and neither is in a state to be taken from in the middle
 * of a probe.
 *
 * A front end that registered first is picked up here, which is the order that
 * happens when the core is loaded second.
 */
void an7552_laser_core_ready(struct an7552_mac *mac)
{
	pon_laser_set_traffic_fn set_traffic;
	pon_laser_safe_circuit_reset_fn safe_circuit_reset;
	u8 mode;
	bool attached;

	if (!mac)
		return;

	/*
	 * Take copies under the lock, not just a flag: xpon_unregister_device()
	 * clears both addresses under this same lock, so reading them after the
	 * unlock would be a read of what the front end is about to free.
	 */
	scoped_guard(mutex, &laser_lock) {
		xpon_mac = mac;
		set_traffic = xpon_set_traffic;
		safe_circuit_reset = xpon_safe_circuit_reset;
		mode = xpon_front_end_mode;
		attached = set_traffic != NULL;
	}

	if (attached)
		an7552_laser_attached(mac, set_traffic, safe_circuit_reset,
				      mode);
}

/*
 * The core going away, as the platform device is removed does.
 *
 * The pointer is cleared here and nowhere else, which is what stops
 * xpon_alarm() and xpon_burst_ctrl() from reaching a struct an7552_mac that
 * devm is about to free: both take this same lock for the whole of their call
 * into the MAC, so once this returns there is no call in flight and no new one
 * can start.
 *
 * The addresses are left alone - they are the front end's, and a front end
 * that is still registered has not given them up.  It will find no core on its
 * next xpon_register_device() bind attempt and simply not be picked up.
 */
void an7552_laser_core_gone(struct an7552_mac *mac)
{
	if (!mac)
		return;

	guard(mutex)(&laser_lock);
	if (xpon_mac == mac)
		xpon_mac = NULL;
}

int xpon_register_device(pon_laser_set_traffic_fn set_traffic,
			 pon_laser_safe_circuit_reset_fn safe_circuit_reset,
			 u8 mode)
{
	struct an7552_mac *waiting;
	int ret = 0;

	if (!set_traffic || !safe_circuit_reset)
		return -EINVAL;

	scoped_guard(mutex, &laser_lock) {
		if (xpon_set_traffic) {
			/* one front end is all the hardware has */
			ret = -EBUSY;
		} else {
			xpon_set_traffic = set_traffic;
			xpon_safe_circuit_reset = safe_circuit_reset;
			xpon_front_end_mode = mode;
		}
		waiting = ret ? NULL : xpon_mac;
	}

	if (ret)
		return ret;

	/*
	 * A core that probed first is waiting for exactly this, and it has to be
	 * told outside the lock: picking the front end up takes the regulator
	 * core and the PHY lock, and holding ours across that is a lock order
	 * nobody has to survive.
	 */
	if (waiting)
		an7552_laser_attached(waiting, set_traffic,
				      safe_circuit_reset, mode);

	return 0;
}
EXPORT_SYMBOL_GPL(xpon_register_device);

/*
 * Drop the front end.  The core stays announced - it is still probed and
 * still loaded - so a driver that unbinds and comes back finds it waiting,
 * exactly as it would have on first registration.  The MAC is told to let go of
 * the addresses before they are cleared here, because what it has to stop is
 * whatever was running through them.
 */
void xpon_unregister_device(void)
{
	struct an7552_mac *mac;

	scoped_guard(mutex, &laser_lock) {
		if (!xpon_set_traffic)
			return;
		xpon_set_traffic = NULL;
		xpon_safe_circuit_reset = NULL;
		mac = xpon_mac;
	}

	if (mac)
		an7552_laser_detached(mac);
}
EXPORT_SYMBOL_GPL(xpon_unregister_device);

/* ---- traffic channel: the mode the front end runs in, and whether user
 * traffic flows ---- */

/* work_mt7571_safe_circuit_reset, queued by phy_trans_power_switch(1) */
void an7552_laser_work(struct work_struct *work)
{
	struct an7552_phy *priv = container_of(work, struct an7552_phy,
						    laser_work);
	pon_laser_safe_circuit_reset_fn ops =
		READ_ONCE(priv->laser_safe_circuit_reset);

	if (ops)
		ops();
}

/*
 * What the front end is told: the mode this PHY runs in while traffic flows,
 * or PON_LASER_MODE_OFF when it does not.
 *
 * One value covers both, because the front end has to act on the mode and on
 * the traffic together - a mode it is told about while the transmitter is meant
 * to be off is not a request to start it.  The core imposes the mapping, so
 * this is the one place the SerDes mode numbers become PON_LASER_MODE_* ones.
 *
 * The argument is the uapi mode number rather than enum an7552_xpon_mode, and
 * that is deliberate on both sides: this PHY admits only GPON and GPON_SYM
 * (an7552_phy_main.c), while the front end is a part that also knows EPON and
 * maps it on its own.  Comparing the two it does accept keeps the number the
 * core hands over the axis, and leaves the mapping table whole for a mode that
 * only the laser ever has to name.
 */
static u32 an7552_laser_mode_of(u32 mode)
{
	switch (mode) {
	case GPON_MODE_GPON_SYM:
		return PON_LASER_MODE_GPON_SYM;
	case GPON_MODE_EPON:
		return PON_LASER_MODE_EPON;
	default:
		return PON_LASER_MODE_GPON;
	}
}

static u32 an7552_laser_mode(struct an7552_phy *priv)
{
	if (!READ_ONCE(priv->traffic_up))
		return PON_LASER_MODE_OFF;

	return an7552_laser_mode_of(priv->mode);
}

/*
 * Hand the front end a mode on its own, with no traffic attached to it.
 *
 * A front end runs in whatever mode its own calibration says until it is told
 * otherwise, and a link that has not been started yet is the only moment the
 * core holds the mode the stack asked for while the transmitter is still off.
 * Leaving the mode to ride in with the traffic would have the first telling
 * arrive at O5, by which point the transmitter has been on since O2 and every
 * upstream burst sent in between went out in the wrong mode.
 *
 * Called with no front end is not an error - there is nothing to tell yet, and
 * the mode is delivered again when one attaches.
 */
int an7552_phy_laser_mode(struct an7552_phy *priv, enum an7552_xpon_mode mode)
{
	pon_laser_set_traffic_fn fn = READ_ONCE(priv->laser_set_traffic);

	if (!fn)
		return 0;

	return fn(an7552_laser_mode_of(mode));
}

/*
 * phy_set_traffic_status(): gpPhyPriv+0x58 bit 2, read by the EN7571 loops
 *
 * A front end that refuses the value it is handed is reported rather than
 * dropped: being told about a mode it cannot take while its transmitter is
 * running is the one answer that says the two sides have drifted apart.
 */
int an7552_phy_set_traffic(struct an7552_phy *priv, bool up)
{
	pon_laser_set_traffic_fn fn = READ_ONCE(priv->laser_set_traffic);

	WRITE_ONCE(priv->traffic_up, up);
	if (!fn)
		return 0;

	return fn(an7552_laser_mode(priv));
}

/*
 * The way a transceiver reports faults up to the MAC.
 *
 * @alarm is a mask and more than one bit may be set in a single call, which is
 * why it is handed on whole rather than taken apart here: the MAC wants to see
 * that both happened.  A no-op when no MAC has probed, once the front end has
 * unregistered, and once the core has gone (an7552_laser_core_gone()).
 *
 * The lock is held for the whole of the call into the MAC, not only for the
 * read of the pointer: that is what lets an7552_laser_core_gone() hand the core
 * back knowing that no call is in flight.  Both sides are process context - the
 * front end calls out of its own work - so a mutex is the right tool here and
 * not just a spinlock over the pointer.
 */
void xpon_alarm(u32 alarm)
{
	struct an7552_mac *mac;

	guard(mutex)(&laser_lock);
	mac = xpon_mac;
	if (mac && !an7552_mac_is_gone(mac))
		an7552_mac_laser_fault(mac, alarm);
}
EXPORT_SYMBOL_GPL(xpon_alarm);

/*
 * The way a transceiver asks for the burst generator to stand still across a
 * DAC write.  See xpon_burst_ctrl() in the header for why.
 */
void xpon_burst_ctrl(bool enable)
{
	struct an7552_mac *mac;

	guard(mutex)(&laser_lock);
	mac = xpon_mac;
	if (mac && !an7552_mac_is_gone(mac))
		an7552_mac_burst_ctrl(mac, enable);
}
EXPORT_SYMBOL_GPL(xpon_burst_ctrl);
