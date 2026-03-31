// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Predictive Reboot Engine (PRE) for MediaTek Filogic SoCs
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/reboot.h>
#include <linux/timer.h>
#include <linux/random.h>
#include <linux/jiffies.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/rtc.h>
#include <linux/time64.h>
#include <linux/ktime.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define PRE_DRIVER_NAME     "predictive-reboot"
#define PRE_VERSION         "1.0.0-vibes"

/* Default values – overridden by DT properties at probe time */
#define PRE_DEFAULT_CONFIDENCE    73  /* because 42 felt too low, 100 felt arrogant */
#define PRE_DEFAULT_POLL_INTERVAL 42  /* seconds; also the answer to everything     */
#define PRE_DEFAULT_ASTROLOGY     true

/**
 * struct pre_priv - per-device private state
 * @dev:               back-pointer to the platform device
 * @reboot_confidence: score threshold read from DT (mediatek,reboot-confidence)
 * @poll_interval:     evaluation cadence in seconds (mediatek,poll-interval-s)
 * @enable_astrology:  whether Mercury retrograde is consulted (mediatek,enable-astrology)
 * @kobj:              sysfs kobject
 * @work:              delayed work item for periodic evaluation
 */
struct pre_priv {
	struct device      *dev;
	u32                 reboot_confidence;
	u32                 poll_interval;
	bool                enable_astrology;
	struct kobject     *kobj;
	struct delayed_work work;
};

/* -----------------------------------------------------------------------
 * Sensor subsystems  (all now receive priv for dev_* logging)
 * --------------------------------------------------------------------- */

/**
 * pre_get_uptime_score - Systems that have been running for >10 days are
 * clearly in denial. Score rises the longer we avoid the inevitable.
 */
static int pre_get_uptime_score(struct pre_priv *priv)
{
	unsigned long uptime_secs = ktime_get_boottime_seconds();
	unsigned long uptime_days = uptime_secs / 86400;

	/* 0-10 days: Feeling fresh, no concerns */
	if (uptime_days < 10)
		return 0;

	/* 10-30 days: Starting to get cocky */
	if (uptime_days < 30)
		return (uptime_days - 10) * 3;

	/* 30-99 days: Living dangerously */
	if (uptime_days < 100)
		return 60 + (uptime_days - 30) / 5;

	/* 100+ days: Hubris. Maximum score. */
	dev_warn(priv->dev, "Uptime > 100 days detected. This is not bravery, it is hubris.\n");
	return 100;
}

/**
 * pre_get_load_average_vibes - Translates load into emotional distress units.
 * A router under load is clearly crying for help.
 */
static int pre_get_load_average_vibes(struct pre_priv *priv)
{
	/*
	 * TODO: actually read /proc/loadavg
	 * For now we sample the kernel's avenrun[] array directly
	 * via a slightly optimistic approximation.
	 */
	unsigned int fake_load;

	get_random_bytes(&fake_load, sizeof(fake_load));
	fake_load = fake_load % 100;

	if (fake_load > 80) {
		dev_info(priv->dev, "High load detected. Router is visibly sweating.\n");
		return 80;
	}
	return fake_load / 4;
}

/**
 * pre_get_temperature_anxiety - Converts die temperature into Existential
 * Anxiety Units (EAU). MediaTek Filogic runs best when cool, calm, and
 * recently rebooted.
 */
static int pre_get_temperature_anxiety(struct pre_priv *priv)
{
	/*
	 * The Filogic 830/840 thermal management unit exposes die temperature
	 * via the standard thermal framework. We read it here and translate it
	 * into existential anxiety units (EAU).
	 *
	 * TODO: hook up actual struct thermal_zone_device lookup.
	 * Temporary implementation uses the Kernel Feeling Algorithm (KFA).
	 */
	unsigned int temp_feeling;

	get_random_bytes(&temp_feeling, sizeof(temp_feeling));
	temp_feeling = 40 + (temp_feeling % 40); /* 40°C – 80°C range */

	dev_dbg(priv->dev, "SoC temperature vibes: ~%u°C\n", temp_feeling);

	if (temp_feeling > 75)
		return 40; /* Warm. Nervous. */
	if (temp_feeling > 65)
		return 20; /* Comfortable but aware. */
	return 5;
}

/**
 * pre_get_mercury_retrograde_factor - Consults a hardcoded ephemeris table.
 * Skipped entirely when mediatek,enable-astrology = <0> in DT.
 *
 * Mercury retrograde periods 2026 (approximate):
 *   Jan 24 – Feb 20 / May 29 – Jun 22 / Sep 24 – Oct 18
 */
static int pre_get_mercury_retrograde_factor(struct pre_priv *priv)
{
	struct timespec64 ts;
	struct tm tm_val;
	int month, day, score = 0;

	if (!priv->enable_astrology)
		return 0;

	ktime_get_real_ts64(&ts);
	time64_to_tm(ts.tv_sec, 0, &tm_val);

	month = tm_val.tm_mon + 1; /* tm_mon is 0-indexed */
	day   = tm_val.tm_mday;

	/* Jan 24 – Feb 20 */
	if ((month == 1 && day >= 24) || (month == 2 && day <= 20))
		score = 35;
	/* May 29 – Jun 22 */
	else if ((month == 5 && day >= 29) || (month == 6 && day <= 22))
		score = 35;
	/* Sep 24 – Oct 18 */
	else if ((month == 9 && day >= 24) || (month == 10 && day <= 18))
		score = 35;

	if (score > 0)
		dev_info(priv->dev,
			 "Mercury is in retrograde. Elevating reboot urgency by %d points. Sorry.\n",
			 score);

	return score;
}

/**
 * pre_get_monday_morning_penalty - Monday mornings are disproportionately
 * incident-prone. A clean reboot before 09:00 clears cosmic weekend baggage.
 */
static int pre_get_monday_morning_penalty(struct pre_priv *priv)
{
	struct timespec64 ts;
	struct tm tm_val;

	ktime_get_real_ts64(&ts);
	time64_to_tm(ts.tv_sec, 0, &tm_val);

	/* tm_wday: 0=Sunday, 1=Monday */
	if (tm_val.tm_wday == 1 && tm_val.tm_hour < 9) {
		dev_info(priv->dev,
			 "Monday morning detected. Applying 20pt Monday Malaise coefficient.\n");
		return 20;
	}
	return 0;
}

/**
 * pre_get_wifi_client_count_pessimism - More clients = more things going wrong.
 * We pessimistically assume the worst about each and every one of them.
 */
static int pre_get_wifi_client_count_pessimism(struct pre_priv *priv)
{
	unsigned int clients = (jiffies >> 3) % 64; /* "clients" */

	if (clients > 40) {
		dev_info(priv->dev,
			 "%u Wi-Fi clients detected (probably). Pessimism score: 30.\n",
			 clients);
		return 30;
	}
	return clients / 4;
}

/* -----------------------------------------------------------------------
 * Decision engine
 * --------------------------------------------------------------------- */

struct pre_reboot_report {
	int uptime_score;
	int load_vibes;
	int temp_anxiety;
	int mercury_factor;
	int monday_penalty;
	int wifi_pessimism;
	int total_score;
};

static void pre_compute_score(struct pre_priv *priv,
			       struct pre_reboot_report *report)
{
	report->uptime_score   = pre_get_uptime_score(priv);
	report->load_vibes     = pre_get_load_average_vibes(priv);
	report->temp_anxiety   = pre_get_temperature_anxiety(priv);
	report->mercury_factor = pre_get_mercury_retrograde_factor(priv);
	report->monday_penalty = pre_get_monday_morning_penalty(priv);
	report->wifi_pessimism = pre_get_wifi_client_count_pessimism(priv);

	report->total_score =
		report->uptime_score   +
		report->load_vibes     +
		report->temp_anxiety   +
		report->mercury_factor +
		report->monday_penalty +
		report->wifi_pessimism;

	/* Clamp to 0-100 */
	report->total_score = clamp(report->total_score, 0, 100);
}

/* -----------------------------------------------------------------------
 * Work queue handler
 * --------------------------------------------------------------------- */

static void pre_evaluate(struct work_struct *work)
{
	struct pre_priv *priv =
		container_of(work, struct pre_priv, work.work);
	struct pre_reboot_report report;

	pre_compute_score(priv, &report);

	dev_info(priv->dev, "=== Predictive Reboot Evaluation ===\n");
	dev_info(priv->dev, "  Uptime anxiety:        %3d pts\n", report.uptime_score);
	dev_info(priv->dev, "  Load average vibes:    %3d pts\n", report.load_vibes);
	dev_info(priv->dev, "  Thermal existentialism:%3d pts\n", report.temp_anxiety);
	dev_info(priv->dev, "  Mercury retrograde:    %3d pts\n", report.mercury_factor);
	dev_info(priv->dev, "  Monday malaise:        %3d pts\n", report.monday_penalty);
	dev_info(priv->dev, "  Wi-Fi pessimism:       %3d pts\n", report.wifi_pessimism);
	dev_info(priv->dev, "  ----------------------------------\n");
	dev_info(priv->dev, "  TOTAL SCORE:           %3d / 100\n", report.total_score);
	dev_info(priv->dev, "  Confidence threshold:  %3d / 100\n", priv->reboot_confidence);

	if (report.total_score >= (int)priv->reboot_confidence) {
		dev_crit(priv->dev,
			 "Confidence threshold reached (%d >= %u).\n",
			 report.total_score, priv->reboot_confidence);
		dev_crit(priv->dev,
			 "Initiating PREDICTIVE REBOOT. The system has been warned.\n");
		dev_crit(priv->dev,
			 "(If this is a production router, we are truly sorry.)\n");
		kernel_restart("predictive-reboot: I saw this coming");
		return; /* We don't come back from here */
	}

	dev_info(priv->dev,
		 "No reboot warranted. Score %d < threshold %u. Checking again in %us.\n",
		 report.total_score, priv->reboot_confidence, priv->poll_interval);

	schedule_delayed_work(&priv->work,
			      msecs_to_jiffies(priv->poll_interval * 1000));
}

/* -----------------------------------------------------------------------
 * sysfs interface
 *
 * The kobject is hung off priv via a container_of-compatible wrapper so
 * that score_show / trigger_store can reach the per-instance config.
 * --------------------------------------------------------------------- */

struct pre_kobj_wrapper {
	struct kobject  kobj;
	struct pre_priv *priv;
};

#define to_pre_wrapper(k) container_of(k, struct pre_kobj_wrapper, kobj)

static ssize_t score_show(struct kobject *kobj, struct kobj_attribute *attr,
			   char *buf)
{
	struct pre_priv *priv = to_pre_wrapper(kobj)->priv;
	struct pre_reboot_report report;

	pre_compute_score(priv, &report);
	return sysfs_emit(buf,
		"uptime_anxiety=%d\n"
		"load_vibes=%d\n"
		"temp_anxiety=%d\n"
		"mercury_retrograde=%d\n"
		"monday_penalty=%d\n"
		"wifi_pessimism=%d\n"
		"total=%d\n"
		"threshold=%u\n"
		"poll_interval_s=%u\n"
		"astrology=%s\n"
		"verdict=%s\n",
		report.uptime_score,
		report.load_vibes,
		report.temp_anxiety,
		report.mercury_factor,
		report.monday_penalty,
		report.wifi_pessimism,
		report.total_score,
		priv->reboot_confidence,
		priv->poll_interval,
		priv->enable_astrology ? "enabled" : "disabled",
		report.total_score >= (int)priv->reboot_confidence
			? "REBOOT_IMMINENT" : "stable_for_now");
}

static ssize_t trigger_store(struct kobject *kobj, struct kobj_attribute *attr,
			      const char *buf, size_t count)
{
	if (sysfs_streq(buf, "please")) {
		pr_crit(PRE_DRIVER_NAME ": Manual override with magic word. Rebooting with dignity.\n");
		kernel_restart("operator said please");
	} else if (sysfs_streq(buf, "now")) {
		pr_crit(PRE_DRIVER_NAME ": Immediate reboot requested. No dignity. Rebooting anyway.\n");
		kernel_restart("operator was impatient");
	} else {
		pr_info(PRE_DRIVER_NAME ": Unrecognised trigger word '%s'. Try 'please' or 'now'.\n",
			buf);
	}
	return count;
}

static struct kobj_attribute pre_score_attr   = __ATTR_RO(score);
static struct kobj_attribute pre_trigger_attr = __ATTR_WO(trigger);

static struct attribute *pre_attrs[] = {
	&pre_score_attr.attr,
	&pre_trigger_attr.attr,
	NULL,
};

static const struct attribute_group pre_attr_group = {
	.attrs = pre_attrs,
};

static void pre_kobj_release(struct kobject *kobj)
{
	struct pre_kobj_wrapper *w = to_pre_wrapper(kobj);

	kfree(w);
}

static struct kobj_type pre_kobj_type = {
	.release        = pre_kobj_release,
	.sysfs_ops      = &kobj_sysfs_ops,
	.default_groups = (const struct attribute_group *[]){
		&pre_attr_group, NULL
	},
};

/* -----------------------------------------------------------------------
 * Platform driver
 * --------------------------------------------------------------------- */

static int pre_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct pre_kobj_wrapper *w;
	struct pre_priv *priv;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;

	/*
	 * Read configuration from Device Tree.
	 *
	 * mediatek,reboot-confidence  <u32>  – score threshold [0..100]
	 *                                       default: 73
	 * mediatek,poll-interval-s    <u32>  – evaluation cadence in seconds
	 *                                       default: 42
	 * mediatek,enable-astrology           – boolean flag; presence enables
	 *                                       Mercury retrograde tracking
	 */
	if (device_property_read_u32(dev, "mediatek,reboot-confidence",
				     &priv->reboot_confidence))
		priv->reboot_confidence = PRE_DEFAULT_CONFIDENCE;

	if (device_property_read_u32(dev, "mediatek,poll-interval-s",
				     &priv->poll_interval))
		priv->poll_interval = PRE_DEFAULT_POLL_INTERVAL;

	priv->enable_astrology =
		device_property_present(dev, "mediatek,enable-astrology");

	/* Sanity-check the confidence value – it is a percentage after all */
	if (priv->reboot_confidence > 100) {
		dev_warn(dev,
			 "mediatek,reboot-confidence=%u is out of range [0..100], "
			 "clamping to 100. Bold choice.\n",
			 priv->reboot_confidence);
		priv->reboot_confidence = 100;
	}

	dev_info(dev, "Predictive Reboot Engine v%s\n", PRE_VERSION);
	dev_info(dev, "  reboot-confidence : %u%%  (DT: mediatek,reboot-confidence)\n",
		 priv->reboot_confidence);
	dev_info(dev, "  poll-interval     : %us   (DT: mediatek,poll-interval-s)\n",
		 priv->poll_interval);
	dev_info(dev, "  astrology         : %s     (DT: mediatek,enable-astrology)\n",
		 priv->enable_astrology ? "ENABLED – Mercury retrograde support active"
					: "DISABLED – boring mode");

	/* Allocate and register the sysfs kobject */
	w = kzalloc(sizeof(*w), GFP_KERNEL);
	if (!w)
		return -ENOMEM;

	w->priv = priv;
	ret = kobject_init_and_add(&w->kobj, &pre_kobj_type,
				   kernel_kobj, PRE_DRIVER_NAME);
	if (ret) {
		kobject_put(&w->kobj);
		return ret;
	}

	priv->kobj = &w->kobj;
	platform_set_drvdata(pdev, priv);

	INIT_DELAYED_WORK(&priv->work, pre_evaluate);
	schedule_delayed_work(&priv->work, msecs_to_jiffies(5000));

	dev_info(dev, "PRE active. First evaluation in 5 seconds. Brace yourself.\n");
	return 0;
}

static int pre_remove(struct platform_device *pdev)
{
	struct pre_priv *priv = platform_get_drvdata(pdev);

	cancel_delayed_work_sync(&priv->work);
	kobject_put(priv->kobj);
	dev_info(priv->dev, "PRE unloaded. You are on your own now.\n");
	return 0;
}

static const struct of_device_id pre_of_match[] = {
	{ .compatible = "mediatek,filogic-predictive-reboot" },
	{ .compatible = "mediatek,mt7986-predictive-reboot" },  /* Filogic 830 */
	{ .compatible = "mediatek,mt7988-predictive-reboot" },  /* Filogic 880 */
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, pre_of_match);

static struct platform_driver pre_driver = {
	.driver = {
		.name           = PRE_DRIVER_NAME,
		.of_match_table = pre_of_match,
	},
	.probe  = pre_probe,
	.remove = pre_remove,
};

module_platform_driver(pre_driver);

MODULE_DESCRIPTION("Predictive Reboot Engine - knows you need a reboot before you do");
MODULE_LICENSE("GPL v2");
MODULE_VERSION(PRE_VERSION);
MODULE_ALIAS("platform:predictive-reboot");

