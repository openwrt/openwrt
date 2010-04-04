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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc-dapm.h>
#include <sound/soc.h>

#define JZ_REG_CODEC_1 0x0
#define JZ_REG_CODEC_2 0x1

#define JZ_CODEC_1_LINE_ENABLE BIT(29)
#define JZ_CODEC_1_MIC_ENABLE BIT(28)
#define JZ_CODEC_1_SW1_ENABLE BIT(27)
#define JZ_CODEC_1_ADC_ENABLE BIT(26)
#define JZ_CODEC_1_SW2_ENABLE BIT(25)
#define JZ_CODEC_1_DAC_ENABLE BIT(24)
#define JZ_CODEC_1_VREF_DISABLE BIT(20)
#define JZ_CODEC_1_VREF_AMP_DISABLE BIT(19)
#define JZ_CODEC_1_VREF_PULL_DOWN BIT(18)
#define JZ_CODEC_1_VREF_LOW_CURRENT BIT(17)
#define JZ_CODEC_1_VREF_HIGH_CURRENT BIT(16)
#define JZ_CODEC_1_HEADPHONE_DISABLE BIT(14)
#define JZ_CODEC_1_HEADPHONE_AMP_CHANGE_ANY BIT(13)
#define JZ_CODEC_1_HEADPHONE_CHANGE BIT(12)
#define JZ_CODEC_1_HEADPHONE_PULL_DOWN_M BIT(11)
#define JZ_CODEC_1_HEADPHONE_PULL_DOWN_R BIT(10)
#define JZ_CODEC_1_HEADPHONE_POWER_DOWN_M BIT(9)
#define JZ_CODEC_1_HEADPHONE_POWER_DOWN BIT(8)
#define JZ_CODEC_1_SUSPEND BIT(1)
#define JZ_CODEC_1_RESET BIT(0)

#define JZ_CODEC_1_LINE_ENABLE_OFFSET 29
#define JZ_CODEC_1_MIC_ENABLE_OFFSET 28
#define JZ_CODEC_1_SW1_ENABLE_OFFSET 27
#define JZ_CODEC_1_ADC_ENABLE_OFFSET 26
#define JZ_CODEC_1_SW2_ENABLE_OFFSET 25
#define JZ_CODEC_1_DAC_ENABLE_OFFSET 24
#define JZ_CODEC_1_HEADPHONE_DISABLE_OFFSET 14
#define JZ_CODEC_1_HEADPHONE_POWER_DOWN_OFFSET 8

#define JZ_CODEC_2_INPUT_VOLUME_MASK		0x1f0000
#define JZ_CODEC_2_SAMPLE_RATE_MASK			0x000f00
#define JZ_CODEC_2_MIC_BOOST_GAIN_MASK		0x000030
#define JZ_CODEC_2_HEADPHONE_VOLUME_MASK	0x000003

#define JZ_CODEC_2_INPUT_VOLUME_OFFSET		16
#define JZ_CODEC_2_SAMPLE_RATE_OFFSET		 8
#define JZ_CODEC_2_MIC_BOOST_GAIN_OFFSET	 4
#define JZ_CODEC_2_HEADPHONE_VOLUME_OFFSET	 0

struct jz_codec {
	void __iomem *base;
	struct resource *mem;

	uint32_t reg_cache[2];
	struct snd_soc_codec codec;
};

inline static struct jz_codec *codec_to_jz(struct snd_soc_codec *codec)
{
	return container_of(codec, struct jz_codec, codec);
}

static unsigned int jz_codec_read(struct snd_soc_codec *codec, unsigned int reg)
{
	struct jz_codec *jz_codec = codec_to_jz(codec);
	return readl(jz_codec->base + (reg << 2));
}

static int jz_codec_write(struct snd_soc_codec *codec, unsigned int reg,
unsigned int val)
{
	struct jz_codec *jz_codec = codec_to_jz(codec);
	jz_codec->reg_cache[reg] = val;

	writel(val, jz_codec->base + (reg << 2));
	return 0;
}

static const struct snd_kcontrol_new jz_codec_controls[] = {
	SOC_SINGLE("Master Playback Volume", JZ_REG_CODEC_2,
			JZ_CODEC_2_HEADPHONE_VOLUME_OFFSET, 3, 0),
	SOC_SINGLE("Capture Volume", JZ_REG_CODEC_2,
			JZ_CODEC_2_INPUT_VOLUME_OFFSET, 31, 0),
	SOC_SINGLE("Master Playback Switch", JZ_REG_CODEC_1,
			JZ_CODEC_1_HEADPHONE_DISABLE_OFFSET, 1, 1),
	SOC_SINGLE("Mic Capture Volume", JZ_REG_CODEC_2,
			JZ_CODEC_2_MIC_BOOST_GAIN_OFFSET, 3, 0),
};

static const struct snd_kcontrol_new jz_codec_output_controls[] = {
	SOC_DAPM_SINGLE("Bypass Switch", JZ_REG_CODEC_1,
			JZ_CODEC_1_SW1_ENABLE_OFFSET, 1, 0),
	SOC_DAPM_SINGLE("DAC Switch", JZ_REG_CODEC_1,
			JZ_CODEC_1_SW2_ENABLE_OFFSET, 1, 0),
};

static const struct snd_kcontrol_new jz_codec_input_controls[] =
{
	SOC_DAPM_SINGLE("Line Capture Switch", JZ_REG_CODEC_1,
			JZ_CODEC_1_LINE_ENABLE_OFFSET, 1, 0),
	SOC_DAPM_SINGLE("Mic Capture Switch", JZ_REG_CODEC_1,
			JZ_CODEC_1_MIC_ENABLE_OFFSET, 1, 0),
};

static const struct snd_soc_dapm_widget jz_codec_dapm_widgets[] = {
	SND_SOC_DAPM_ADC("ADC", "Capture", JZ_REG_CODEC_1,
			JZ_CODEC_1_ADC_ENABLE_OFFSET, 0),
	SND_SOC_DAPM_DAC("DAC", "Playback", JZ_REG_CODEC_1,
			JZ_CODEC_1_DAC_ENABLE_OFFSET, 0),

	SND_SOC_DAPM_MIXER("Output Mixer", JZ_REG_CODEC_1,
			JZ_CODEC_1_HEADPHONE_POWER_DOWN_OFFSET, 1,
			jz_codec_output_controls,
			ARRAY_SIZE(jz_codec_output_controls)),

	SND_SOC_DAPM_MIXER_NAMED_CTL("Input Mixer", SND_SOC_NOPM, 0, 0,
			jz_codec_input_controls,
			ARRAY_SIZE(jz_codec_input_controls)),
	SND_SOC_DAPM_MIXER("Line Input", SND_SOC_NOPM, 0, 0, NULL, 0),

	SND_SOC_DAPM_OUTPUT("LOUT"),
	SND_SOC_DAPM_OUTPUT("ROUT"),

	SND_SOC_DAPM_INPUT("MIC"),
	SND_SOC_DAPM_INPUT("LIN"),
	SND_SOC_DAPM_INPUT("RIN"),
};

static const struct snd_soc_dapm_route jz_codec_dapm_routes[] = {

	{"Line Input", NULL, "LIN"},
	{"Line Input", NULL, "RIN"},

	{"Input Mixer", "Line Capture Switch", "Line Input"},
	{"Input Mixer", "Mic Capture Switch", "MIC"},

	{"ADC", NULL, "Input Mixer"},

	{"Output Mixer", "Bypass Switch", "Input Mixer"},
	{"Output Mixer", "DAC Switch", "DAC"},

	{"LOUT", NULL, "Output Mixer"},
	{"ROUT", NULL, "Output Mixer"},
};

static int jz_codec_hw_params(struct snd_pcm_substream *substream, struct
snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	uint32_t val;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S18_3LE:
		break;
	default:
		return -EINVAL;
		break;
	}

	switch (params_rate(params)) {
	case 8000:
		val = 0;
		break;
	case 11025:
		val = 1;
		break;
	case 12000:
		val = 2;
		break;
	case 16000:
		val = 3;
		break;
	case 22050:
		val = 4;
		break;
	case 24000:
		val = 5;
		break;
	case 32000:
		val = 6;
		break;
	case 44100:
		val = 7;
		break;
	case 48000:
		val = 8;
		break;
	default:
		return -EINVAL;
	}

	val <<= JZ_CODEC_2_SAMPLE_RATE_OFFSET;

	snd_soc_update_bits(codec, JZ_REG_CODEC_2,
				JZ_CODEC_2_SAMPLE_RATE_MASK, val);

	return 0;
}

static int jz_codec_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int jz_codec_set_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
	return 0;
}


static struct snd_soc_dai_ops jz_codec_dai_ops = {
	.hw_params = jz_codec_hw_params,
	.set_fmt = jz_codec_set_fmt,
/*	.set_clkdiv = jz_codec_set_clkdiv,*/
	.set_sysclk = jz_codec_set_sysclk,
};

struct snd_soc_dai jz_codec_dai = {
	.name = "jz-codec",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_44100,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_8000_44100,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.ops = &jz_codec_dai_ops,
	.symmetric_rates = 1,
};
EXPORT_SYMBOL_GPL(jz_codec_dai);

static int jz_codec_set_bias_level(struct snd_soc_codec *codec,
				enum snd_soc_bias_level level)
{

	if (codec->bias_level == SND_SOC_BIAS_OFF && level != SND_SOC_BIAS_OFF) {
		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
				JZ_CODEC_1_RESET, JZ_CODEC_1_RESET);
		udelay(2);

		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
					JZ_CODEC_1_SUSPEND | JZ_CODEC_1_RESET, 0);
	}
	switch (level) {
	case SND_SOC_BIAS_ON:
		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
					JZ_CODEC_1_VREF_DISABLE | JZ_CODEC_1_VREF_AMP_DISABLE |
					JZ_CODEC_1_HEADPHONE_POWER_DOWN_M |
					JZ_CODEC_1_VREF_LOW_CURRENT | JZ_CODEC_1_VREF_HIGH_CURRENT,
				0);
		break;
	case SND_SOC_BIAS_PREPARE:
		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
					JZ_CODEC_1_VREF_LOW_CURRENT | JZ_CODEC_1_VREF_HIGH_CURRENT,
					JZ_CODEC_1_VREF_LOW_CURRENT | JZ_CODEC_1_VREF_HIGH_CURRENT);
		break;
	case SND_SOC_BIAS_STANDBY:
		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
					JZ_CODEC_1_VREF_DISABLE | JZ_CODEC_1_VREF_AMP_DISABLE,
					JZ_CODEC_1_VREF_DISABLE | JZ_CODEC_1_VREF_AMP_DISABLE);
		break;
	case SND_SOC_BIAS_OFF:
		snd_soc_update_bits(codec, JZ_REG_CODEC_1,
					JZ_CODEC_1_SUSPEND, JZ_CODEC_1_SUSPEND);
		break;
	}
	codec->bias_level = level;

	return 0;
}


static struct snd_soc_codec *jz_codec_codec;

static int jz_codec_dev_probe(struct platform_device *pdev)
{
	int ret;
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = jz_codec_codec;

	BUG_ON(!codec);

	socdev->card->codec = codec;

	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create pcms: %d\n", ret);
		goto err;
	}
	snd_soc_add_controls(codec, jz_codec_controls,
	ARRAY_SIZE(jz_codec_controls));

	snd_soc_dapm_new_controls(codec, jz_codec_dapm_widgets,
	ARRAY_SIZE(jz_codec_dapm_widgets));

	snd_soc_dapm_add_routes(codec, jz_codec_dapm_routes,
	ARRAY_SIZE(jz_codec_dapm_routes));

	snd_soc_dapm_new_widgets(codec);

	ret = snd_soc_init_card(socdev);

	if (ret) {
		dev_err(&pdev->dev, "Failed to register card\n");
		goto err;
	}

	return 0;

err:
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

	return ret;
}

static int jz_codec_dev_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

	return 0;
}

struct snd_soc_codec_device soc_codec_dev_jzcodec = {
	.probe = jz_codec_dev_probe,
	.remove = jz_codec_dev_remove,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_jzcodec);

static int __devinit jz_codec_probe(struct platform_device *pdev)
{
	int ret;
	struct jz_codec *jz_codec;
	struct snd_soc_codec *codec;

	jz_codec = kzalloc(sizeof(*jz_codec), GFP_KERNEL);

	if (!jz_codec)
		return -ENOMEM;

	jz_codec->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!jz_codec->mem) {
		dev_err(&pdev->dev, "Failed to get mmio memory resource\n");
		ret = -ENOENT;
		goto err_free_jz_codec;
	}

	jz_codec->mem = request_mem_region(jz_codec->mem->start,
				resource_size(jz_codec->mem), pdev->name);

	if (!jz_codec->mem) {
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		ret = -EBUSY;
		goto err_free_jz_codec;
	}

	jz_codec->base = ioremap(jz_codec->mem->start, resource_size(jz_codec->mem));

	if (!jz_codec->base) {
		dev_err(&pdev->dev, "Failed to ioremap mmio memory\n");
		ret = -EBUSY;
		goto err_release_mem_region;
	}

	jz_codec_dai.dev = &pdev->dev;

	codec = &jz_codec->codec;

	codec->dev		= &pdev->dev;
	codec->name		= "jz-codec";
	codec->owner		= THIS_MODULE;

	codec->read		= jz_codec_read;
	codec->write		= jz_codec_write;
	codec->set_bias_level	= jz_codec_set_bias_level;
	codec->bias_level	= SND_SOC_BIAS_OFF;

	codec->dai		= &jz_codec_dai;
	codec->num_dai		= 1;

	codec->reg_cache	= jz_codec->reg_cache;
	codec->reg_cache_size	= 2;

	codec->private_data	= jz_codec;

	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);

	jz_codec_codec = codec;

	snd_soc_update_bits(codec, JZ_REG_CODEC_1,
				JZ_CODEC_1_SW2_ENABLE, JZ_CODEC_1_SW2_ENABLE);


	platform_set_drvdata(pdev, jz_codec);
	ret = snd_soc_register_codec(codec);

	if (ret) {
		dev_err(&pdev->dev, "Failed to register codec\n");
		goto err_iounmap;
	}

	ret = snd_soc_register_dai(&jz_codec_dai);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register codec dai\n");
		goto err_unregister_codec;
	}

	jz_codec_set_bias_level (codec, SND_SOC_BIAS_STANDBY);

	return 0;
err_unregister_codec:
	snd_soc_unregister_codec(codec);
err_iounmap:
	iounmap(jz_codec->base);
err_release_mem_region:
	release_mem_region(jz_codec->mem->start, resource_size(jz_codec->mem));
err_free_jz_codec:
	kfree(jz_codec);

	return ret;
}

static int __devexit jz_codec_remove(struct platform_device *pdev)
{
	struct jz_codec *jz_codec = platform_get_drvdata(pdev);

	snd_soc_unregister_dai(&jz_codec_dai);
	snd_soc_unregister_codec(&jz_codec->codec);

	iounmap(jz_codec->base);
	release_mem_region(jz_codec->mem->start, resource_size(jz_codec->mem));

	platform_set_drvdata(pdev, NULL);
	kfree(jz_codec);

	return 0;
}

static struct platform_driver jz_codec_driver = {
	.probe = jz_codec_probe,
	.remove = __devexit_p(jz_codec_remove),
	.driver = {
		.name = "jz4740-codec",
		.owner = THIS_MODULE,
	},
};

static int __init jz_codec_init(void)
{
	return platform_driver_register(&jz_codec_driver);
}
module_init(jz_codec_init);

static void __exit jz_codec_exit(void)
{
	platform_driver_unregister(&jz_codec_driver);
}
module_exit(jz_codec_exit);

MODULE_DESCRIPTION("JZ4720/JZ4740 SoC internal codec driver");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:jz-codec");
