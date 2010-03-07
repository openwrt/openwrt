/*
 * Copyright (C) 2009, Yauhen Kharuzhy <jekhor@gmail.com>
 *  OpenInkpot project
 * Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/jack.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>

#include "../codecs/jzcodec.h"
#include "jz4740-pcm.h"
#include "jz4740-i2s.h"

#include <asm/mach-jz4740/board-n516.h>

enum {
	N516_SPEAKER_AUTO = 0,
	N516_SPEAKER_OFF = 1,
	N516_SPEAKER_ON = 2,
};

static int n516_speaker_mode;
static struct snd_soc_codec *n516_codec;
static struct work_struct n516_headphone_work;

static void n516_ext_control(void)
{
	if (!n516_codec)
		return;

	switch (n516_speaker_mode) {
	case N516_SPEAKER_ON:
		snd_soc_dapm_enable_pin(n516_codec, "Speaker");
		break;
	case N516_SPEAKER_OFF:
		snd_soc_dapm_disable_pin(n516_codec, "Speaker");
	    break;
	case N516_SPEAKER_AUTO:
		if (snd_soc_dapm_get_pin_status(n516_codec, "Headphone"))
			snd_soc_dapm_disable_pin(n516_codec, "Speaker");
		else
			snd_soc_dapm_enable_pin(n516_codec, "Speaker");
		break;
	default:
		break;
	}

    /* signal a DAPM event */
    snd_soc_dapm_sync(n516_codec);
}

static int n516_speaker_event(struct snd_soc_dapm_widget *widget,
			     struct snd_kcontrol *ctrl, int event)
{
	int on = !SND_SOC_DAPM_EVENT_OFF(event);

	gpio_set_value(GPIO_SPEAKER_ENABLE, on);

	return 0;
}

static void n516_headphone_event_work(struct work_struct *work)
{
	n516_ext_control();
}

static int n516_headphone_event(struct snd_soc_dapm_widget *widget,
			     struct snd_kcontrol *ctrl, int event)
{
	/* We can't call soc_dapm_sync from a event handler */
	if (event & (SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD))
		schedule_work(&n516_headphone_work);
	return 0;
}

static const struct snd_soc_dapm_widget n516_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", n516_speaker_event),
	SND_SOC_DAPM_HP("Headphone", n516_headphone_event),
	SND_SOC_DAPM_MIC("Mic", NULL),
};

static const struct snd_soc_dapm_route n516_routes[] = {
	{"Mic", NULL, "MIC"},
	{"Speaker", NULL, "LOUT"},
	{"Speaker", NULL, "ROUT"},
	{"Headphone", NULL, "LOUT"},
	{"Headphone", NULL, "ROUT"},
};

static const char *n516_speaker_modes[] = {"Auto", "Off", "On"};
static const struct soc_enum n516_speaker_mode_enum =
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(n516_speaker_modes), n516_speaker_modes);

static int n516_get_speaker_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = n516_speaker_mode;
	return 0;
}

static int n516_set_speaker_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	if (n516_speaker_mode == ucontrol->value.integer.value[0])
		return 0;

	n516_speaker_mode = ucontrol->value.integer.value[0];
	n516_ext_control();
	return 1;
}

static const struct snd_kcontrol_new n516_controls[] = {
	SOC_ENUM_EXT("Speaker Function", n516_speaker_mode_enum,
		n516_get_speaker_mode, n516_set_speaker_mode),
};

#define N516_DAIFMT (SND_SOC_DAIFMT_I2S | \
			SND_SOC_DAIFMT_NB_NF | \
			SND_SOC_DAIFMT_CBM_CFM)

static int n516_codec_init(struct snd_soc_codec *codec)
{
	int ret;
	struct snd_soc_dai *cpu_dai = codec->socdev->card->dai_link->cpu_dai;
	struct snd_soc_dai *codec_dai = codec->socdev->card->dai_link->codec_dai;

	n516_codec = codec;

	snd_soc_dapm_nc_pin(codec, "LIN");
	snd_soc_dapm_nc_pin(codec, "RIN");

	ret = snd_soc_dai_set_fmt(codec_dai, N516_DAIFMT);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set codec dai format: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_fmt(cpu_dai, N516_DAIFMT);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cpu dai format: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_sysclk(codec_dai, JZCODEC_SYSCLK, 111,
		SND_SOC_CLOCK_IN);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set codec dai sysclk: %d\n", ret);
		return ret;
	}

	ret = snd_soc_add_controls(codec, n516_controls,
		ARRAY_SIZE(n516_controls));
	if (ret) {
		dev_err(codec->dev, "Failed to add controls: %d\n", ret);
		return ret;
	}


	ret = snd_soc_dapm_new_controls(codec, n516_widgets,
		ARRAY_SIZE(n516_widgets));
	if (ret) {
		dev_err(codec->dev, "Failed to add dapm controls: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dapm_add_routes(codec, n516_routes, ARRAY_SIZE(n516_routes));
	if (ret) {
		dev_err(codec->dev, "Failed to add dapm routes: %d\n", ret);
		return ret;
	}

	snd_soc_dapm_sync(codec);

	return 0;
}

static struct snd_soc_dai_link n516_dai = {
	.name = "jz-codec",
	.stream_name = "JZCODEC",
	.cpu_dai = &jz4740_i2s_dai,
	.codec_dai = &jz_codec_dai,
	.init = n516_codec_init,
};

static struct snd_soc_card n516_card = {
	.name = "N516",
	.dai_link = &n516_dai,
	.num_links = 1,
	.platform = &jz4740_soc_platform,
};

static struct snd_soc_device n516_snd_devdata = {
	.card = &n516_card,
	.codec_dev = &soc_codec_dev_jzcodec,
};

static struct platform_device *n516_snd_device;

static struct snd_soc_jack n516_hp_jack;

static struct snd_soc_jack_pin n516_hp_pin = {
	.pin = "Headphone",
	.mask = SND_JACK_HEADPHONE,
};

static struct snd_soc_jack_gpio n516_hp_gpio = {
	.gpio = GPIO_HPHONE_DETECT,
	.name = "Headphone detect",
	.report = SND_JACK_HEADPHONE,
	.debounce_time = 100,
};

static int __init n516_add_headphone_jack(void)
{
	int ret;

	ret = snd_soc_jack_new(&n516_card, "Headphone jack",
		SND_JACK_HEADPHONE, &n516_hp_jack);
	if (ret)
		return ret;

	ret = snd_soc_jack_add_pins(&n516_hp_jack, 1, &n516_hp_pin);
	if (ret)
		return ret;

	ret = snd_soc_jack_add_gpios(&n516_hp_jack, 1, &n516_hp_gpio);

	return ret;
}

static int __init n516_init(void)
{
	int ret;

	n516_snd_device = platform_device_alloc("soc-audio", -1);

	if (!n516_snd_device)
		return -ENOMEM;

	ret = gpio_request(GPIO_SPEAKER_ENABLE, "Speaker enable");
	if (ret) {
		pr_err("n516 snd: Failed to request SPEAKER_ENABLE GPIO(%d): %d\n",
				GPIO_SPEAKER_ENABLE, ret);
		goto err_device_put;
	}

	gpio_direction_output(GPIO_SPEAKER_ENABLE, 0);
	INIT_WORK(&n516_headphone_work, n516_headphone_event_work);

	platform_set_drvdata(n516_snd_device, &n516_snd_devdata);
	n516_snd_devdata.dev = &n516_snd_device->dev;
	ret = platform_device_add(n516_snd_device);
	if (ret) {
		pr_err("n516 snd: Failed to add snd soc device: %d\n", ret);
		goto err_unset_pdata;
	}

	ret = n516_add_headphone_jack();
	/* We can live without it, so just print a warning */
	if (ret)
		pr_warning("n516 snd: Failed to initalise headphone jack: %d\n", ret);

	return 0;

err_unset_pdata:
	platform_set_drvdata(n516_snd_device, NULL);
/*err_gpio_free_speaker:*/
	gpio_free(GPIO_SPEAKER_ENABLE);
err_device_put:
	platform_device_put(n516_snd_device);

	return ret;
}
module_init(n516_init);

static void __exit n516_exit(void)
{
	snd_soc_jack_free_gpios(&n516_hp_jack, 1, &n516_hp_gpio);
	gpio_free(GPIO_SPEAKER_ENABLE);
	platform_device_unregister(n516_snd_device);
}
module_exit(n516_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("ALSA SoC N516 Audio support");
MODULE_LICENSE("GPL v2");
