/*
 * Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/gpio.h>

#include "../codecs/jzcodec.h"
#include "jz4740-pcm.h"
#include "jz4740-i2s.h"

#define N526_AMP_EN_GPIO JZ_GPIO_PORTD(4)

static int n526_spk_event(struct snd_soc_dapm_widget *widget,
			     struct snd_kcontrol *ctrl, int event)
{
	gpio_set_value(N526_AMP_EN_GPIO, !SND_SOC_DAPM_EVENT_OFF(event));
	return 0;
}

static const struct snd_soc_dapm_widget n526_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", n526_spk_event),
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Mic", NULL),
};

static const struct snd_soc_dapm_route n526_routes[] = {
	{"Mic", NULL, "MIC"},
	{"Speaker", NULL, "LOUT"},
	{"Speaker", NULL, "ROUT"},
	{"Headphone", NULL, "LOUT"},
	{"Headphone", NULL, "ROUT"},
};

static const struct snd_kcontrol_new n526_controls[] = {
	SOC_DAPM_PIN_SWITCH("Speaker"),
};

#define N526_DAIFMT (SND_SOC_DAIFMT_I2S | \
			SND_SOC_DAIFMT_NB_NF | \
			SND_SOC_DAIFMT_CBM_CFM)

static int n526_codec_init(struct snd_soc_codec *codec)
{
	int ret;
	struct snd_soc_dai *cpu_dai = codec->socdev->card->dai_link->cpu_dai;
	struct snd_soc_dai *codec_dai = codec->socdev->card->dai_link->codec_dai;

	snd_soc_dapm_nc_pin(codec, "LIN");
	snd_soc_dapm_nc_pin(codec, "RIN");

	ret = snd_soc_dai_set_fmt(codec_dai, N526_DAIFMT);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set codec dai format: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_fmt(cpu_dai, N526_DAIFMT);
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

	snd_soc_dapm_new_controls(codec, n526_widgets, ARRAY_SIZE(n526_widgets));

	snd_soc_add_controls(codec, n526_controls,
				ARRAY_SIZE(n526_controls));

	snd_soc_dapm_add_routes(codec, n526_routes, ARRAY_SIZE(n526_routes));

	snd_soc_dapm_sync(codec);

	return 0;
}

static struct snd_soc_dai_link n526_dai = {
	.name = "jz-codec",
	.stream_name = "JZCODEC",
	.cpu_dai = &jz4740_i2s_dai,
	.codec_dai = &jz_codec_dai,
	.init = n526_codec_init,
};

static struct snd_soc_card n526 = {
	.name = "N526",
	.dai_link = &n526_dai,
	.num_links = 1,
	.platform = &jz4740_soc_platform,
};

static struct snd_soc_device n526_snd_devdata = {
	.card = &n526,
	.codec_dev = &soc_codec_dev_jzcodec,
};

static struct platform_device *n526_snd_device;

static int __init n526_init(void)
{
	int ret;

	n526_snd_device = platform_device_alloc("soc-audio", -1);

	if (!n526_snd_device)
		return -ENOMEM;

	ret = gpio_request(N526_AMP_EN_GPIO, "AMP");
	if (ret) {
		pr_err("n526 snd: Failed to request AMP GPIO(%d): %d\n",
				N526_AMP_EN_GPIO, ret);
		goto err_device_put;
	}

	gpio_direction_output(JZ_GPIO_PORTD(4), 0);

	platform_set_drvdata(n526_snd_device, &n526_snd_devdata);
	n526_snd_devdata.dev = &n526_snd_device->dev;
	ret = platform_device_add(n526_snd_device);
	if (ret) {
		pr_err("n526 snd: Failed to add snd soc device: %d\n", ret);
		goto err_unset_pdata;
	}

	 return 0;

err_unset_pdata:
	platform_set_drvdata(n526_snd_device, NULL);
	gpio_free(N526_AMP_EN_GPIO);
err_device_put:
	platform_device_put(n526_snd_device);

	return ret;
}
module_init(n526_init);

static void __exit n526_exit(void)
{
	gpio_free(N526_AMP_EN_GPIO);
	platform_device_unregister(n526_snd_device);
}
module_exit(n526_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("ALSA SoC N526 audio support");
MODULE_LICENSE("GPL v2");
