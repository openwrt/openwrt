// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG (DSP Group)
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bits.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <sound/soc.h>
#include <sound/tlv.h>

#define DAIF_AFEDIGC1		5
#define DAIF_AFEDIGC2		6
#define DAIF_AMPOUTTHD1		10
#define DAIF_AMPOUTTHD2		11
#define DAIF_AFEAMPEN		14
#define DAIF_AFECFG1		15
#define DAIF_AFECFG2		16
#define DAIF_AFEIN1		17
#define DAIF_AFEOUTGC0		18
#define DAIF_AFEOUTGC1		19
#define DAIF_AFESIGC1		20
#define DAIF_AFESIGC2		21
#define DAIF_AMPOUTSEL1		22
#define DAIF_LQSD_CFG1		23
#define DAIF_LQSD_CFG2		24
#define DAIF_AMPOUTSEL2		34
#define DAIF_DAC0GC0		38
#define DAIF_DACMUTE		40
#define DAIF_AFEMIC0_SHORT	81
#define DAIF_AFEMIC1_SHORT	82
#define DAIF_DCLS_ANA_CTRL	106
#define DAIF_AFEICTRL1		148
#define DAIF_AFEICTRL2		150

#define DCLS_ANA_CTRL_SUP_MODE	BIT(6)

static const char * const dvf9a_input_sel[] = {
	"SINGIN0", "SINGIN1", "DIFFIN0", "DIFFIN1",
	"SINGIN0-SINGIN1", "DAC0", "MUTE-VREF", "MUTE-FLOAT"
};

static const char * const dvf9a_input_sel1[] = {
	"SINGIN0", "SINGIN1", "DIFFIN0", "DIFFIN1",
	"SINGIN0-SINGIN1", "VREF-OFFSET", "MUTE-VREF", "MUTE-FLOAT"
};

static const char * const dvf9a_output_sel[] = {
	"DAC0", "DAC1", "DAC0_INV", "DAC1_INV", "SINGIN0",
	"DIFFIN0", "DIFFIN1", "MUTE"
};

static const char * const dvf9a_output_sel1[] = {
	"DAC0", "DAC1", "DAC0_INV", "DAC1_INV", "SINGIN1",
	"DIFFIN0_INV", "DIFFIN1_INV", "MUTE"
};

static const char * const dvf9a_mute_type[] = {
	"NO_MUTE", "MUTE1", "MUTE2", "MUTE3"
};

static const char * const dvf9a_lat_type[] = {
	"FALLING_EDGE", "LOW_PHASE", "RISING_EDGE", "HIGH_PHASE"
};

static const char * const dvf9a_amp_current[] = {
	"50%", "66%", "100%", "130%"
};

static const char * const dvf9a_adc_current[] = {
	"22%", "26%", "40%", "52%", "32%", "40%", "133%", "30%",
	"36%", "66%", "103%", "36%", "45%", "100%", "200%"
};

static const char * const dvf9a_vref_level[] = {
	"1.5V", "1.65V", "1.0V", "1.8V", "1.2V", "AUX_MUX",
	"VCCAMPOUT/2", "VCCA/2"
};

static const char * const dvf9a_out_amp_ref[] = {
	"GLOBAL_AFE_REF", "VCCAMPOUT/2"
};

static const char * const dvf9a_switch[] = {
	"off", "on"
};

static const char * const dvf9a_dac_curr_scale[] = {
	"-6dB", "-3dB", "-2dB", "-1dB", "0dB", "+1dB", "+2dB", "+3dB"
};

static SOC_ENUM_SINGLE_DECL(dvf9a_in_mux0, DAIF_AFEIN1, 0, dvf9a_input_sel);
static SOC_ENUM_SINGLE_DECL(dvf9a_in_mux1, DAIF_AFEIN1, 4, dvf9a_input_sel1);
static SOC_ENUM_SINGLE_DECL(dvf9a_out_mux0, DAIF_AMPOUTSEL1, 0,
			    dvf9a_output_sel);
static SOC_ENUM_SINGLE_DECL(dvf9a_out_mux1, DAIF_AMPOUTSEL1, 4,
			    dvf9a_output_sel1);
static SOC_ENUM_SINGLE_DECL(dvf9a_out_mux2, DAIF_AMPOUTSEL2, 0,
			    dvf9a_output_sel);
static SOC_ENUM_SINGLE_DECL(dvf9a_out_mux3, DAIF_AMPOUTSEL2, 4,
			    dvf9a_output_sel1);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac0_mute, DAIF_DACMUTE, 0, dvf9a_mute_type);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac1_mute, DAIF_DACMUTE, 2, dvf9a_mute_type);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac_lat, DAIF_LQSD_CFG2, 3, dvf9a_lat_type);
static SOC_ENUM_SINGLE_DECL(dvf9a_adc_lat, DAIF_LQSD_CFG2, 5, dvf9a_lat_type);
static SOC_ENUM_SINGLE_DECL(dvf9a_diffin0_i, DAIF_AFEICTRL1, 0,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_diffin1_i, DAIF_AFEICTRL1, 2,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_singin0_i, DAIF_AFEICTRL1, 4,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_singin1_i, DAIF_AFEICTRL1, 6,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout0_i, DAIF_AFEICTRL1, 8,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout1_i, DAIF_AFEICTRL1, 10,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout2_i, DAIF_AFEICTRL1, 12,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout3_i, DAIF_AFEICTRL1, 14,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac0_i, DAIF_AFEICTRL2, 12,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac1_i, DAIF_AFEICTRL2, 14,
			    dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_adc0_i, DAIF_AFEICTRL2, 0,
			    dvf9a_adc_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_adc1_i, DAIF_AFEICTRL2, 4,
			    dvf9a_adc_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_vref_lvl, DAIF_AFECFG1, 4, dvf9a_vref_level);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout_vref, DAIF_AFECFG2, 0,
			    dvf9a_out_amp_ref);
static SOC_ENUM_SINGLE_DECL(dvf9a_vref_aux, DAIF_AFECFG2, 3, dvf9a_switch);
static SOC_ENUM_SINGLE_DECL(dvf9a_vref_i, DAIF_AFECFG2, 2, dvf9a_amp_current);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout0_att, DAIF_AFEOUTGC0, 3, dvf9a_switch);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout1_att, DAIF_AFEOUTGC0, 7, dvf9a_switch);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout2_att, DAIF_AFEOUTGC1, 3, dvf9a_switch);
static SOC_ENUM_SINGLE_DECL(dvf9a_ampout3_att, DAIF_AFEOUTGC1, 7, dvf9a_switch);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac0_scale, DAIF_DAC0GC0, 0,
			    dvf9a_dac_curr_scale);
static SOC_ENUM_SINGLE_DECL(dvf9a_dac1_scale, DAIF_DAC0GC0, 4,
			    dvf9a_dac_curr_scale);

static const DECLARE_TLV_DB_SCALE(dvf9a_input_tlv, -400, 200, 0);

static const struct snd_kcontrol_new dvf9a_controls[] = {
	SOC_SINGLE_TLV("DIFFIN0 Gain", DAIF_AFEDIGC1, 0, 28, 0,
		       dvf9a_input_tlv),
	SOC_SINGLE_TLV("DIFFIN1 Gain", DAIF_AFEDIGC2, 0, 28, 0,
		       dvf9a_input_tlv),
	SOC_SINGLE_TLV("SINGIN0 Gain", DAIF_AFESIGC1, 0, 28, 0,
		       dvf9a_input_tlv),
	SOC_SINGLE_TLV("SINGIN1 Gain", DAIF_AFESIGC2, 0, 28, 0,
		       dvf9a_input_tlv),

	SOC_SINGLE("Output0 Gain", DAIF_AFEOUTGC0, 0, 6, 0),
	SOC_SINGLE("Output1 Gain", DAIF_AFEOUTGC0, 4, 6, 0),
	SOC_SINGLE("Output2 Gain", DAIF_AFEOUTGC1, 0, 6, 0),
	SOC_SINGLE("Output3 Gain", DAIF_AFEOUTGC1, 4, 6, 0),

	SOC_ENUM("Input Mux0", dvf9a_in_mux0),
	SOC_ENUM("Input Mux1", dvf9a_in_mux1),
	SOC_ENUM("Output Mux0", dvf9a_out_mux0),
	SOC_ENUM("Output Mux1", dvf9a_out_mux1),
	SOC_ENUM("Output Mux2", dvf9a_out_mux2),
	SOC_ENUM("Output Mux3", dvf9a_out_mux3),

	SOC_ENUM("DAC0 MuteType", dvf9a_dac0_mute),
	SOC_ENUM("DAC1 MuteType", dvf9a_dac1_mute),
	SOC_ENUM("DIFFIN0_ICTRL", dvf9a_diffin0_i),
	SOC_ENUM("DIFFIN1_ICTRL", dvf9a_diffin1_i),
	SOC_ENUM("SINGIN0_ICTRL", dvf9a_singin0_i),
	SOC_ENUM("SINGIN1_ICTRL", dvf9a_singin1_i),
	SOC_ENUM("AMPOUT0_ICTRL", dvf9a_ampout0_i),
	SOC_ENUM("AMPOUT1_ICTRL", dvf9a_ampout1_i),
	SOC_ENUM("AMPOUT2_ICTRL", dvf9a_ampout2_i),
	SOC_ENUM("AMPOUT3_ICTRL", dvf9a_ampout3_i),
	SOC_ENUM("DAC0_ICTRL", dvf9a_dac0_i),
	SOC_ENUM("DAC1_ICTRL", dvf9a_dac1_i),
	SOC_ENUM("ADC0_ICTRL", dvf9a_adc0_i),
	SOC_ENUM("ADC1_ICTRL", dvf9a_adc1_i),
	SOC_ENUM("VREF_LVL", dvf9a_vref_lvl),
	SOC_ENUM("AMPOUT_VREF_SEL", dvf9a_ampout_vref),
	SOC_ENUM("VREF_TO_AUX_EN", dvf9a_vref_aux),
	SOC_ENUM("VREF_ICTRL", dvf9a_vref_i),
	SOC_ENUM("AMPOUT0_30DBATT", dvf9a_ampout0_att),
	SOC_ENUM("AMPOUT1_30DBATT", dvf9a_ampout1_att),
	SOC_ENUM("AMPOUT2_30DBATT", dvf9a_ampout2_att),
	SOC_ENUM("AMPOUT3_30DBATT", dvf9a_ampout3_att),
	SOC_ENUM("DAC0_CURRENT_SCALE", dvf9a_dac0_scale),
	SOC_ENUM("DAC1_CURRENT_SCALE", dvf9a_dac1_scale),
	SOC_ENUM("DAC_LATENCY", dvf9a_dac_lat),
	SOC_ENUM("ADC_LATENCY", dvf9a_adc_lat),

	SOC_SINGLE("VREF_EN", DAIF_AFECFG1, 7, 1, 0),
	SOC_SINGLE("DCLASS_EN", DAIF_DCLS_ANA_CTRL, 0, 1, 0),
	SOC_SINGLE("ULTRA_WB_EN", DAIF_LQSD_CFG2, 2, 1, 0),
	SOC_SINGLE("AFE_CLK_SRC_CRYSTAL_OSC", DAIF_LQSD_CFG2, 1, 1, 0),
	SOC_SINGLE("AFE_SAMPLING_864K", DAIF_LQSD_CFG2, 0, 1, 0),
	SOC_SINGLE("MIC0_SHORT_WIN_SIZE", DAIF_AFEMIC0_SHORT, 0, 3, 0),
	SOC_SINGLE("MIC0_SHORT_DET_EN", DAIF_AFEMIC0_SHORT, 2, 1, 0),
	SOC_SINGLE("MIC0_SHORT_DET_PROTECTION_EN", DAIF_AFEMIC0_SHORT, 3, 1,
		   0),
	SOC_SINGLE("MIC1_SHORT_WIN_SIZE", DAIF_AFEMIC1_SHORT, 0, 3, 0),
	SOC_SINGLE("MIC1_SHORT_DET_EN", DAIF_AFEMIC1_SHORT, 2, 1, 0),
	SOC_SINGLE("MIC1_SHORT_DET_PROTECTION_EN", DAIF_AFEMIC1_SHORT, 3, 1,
		   0),
	SOC_SINGLE("AMPOUT0_LOW_THD", DAIF_AMPOUTTHD1, 0, 0xf, 0),
	SOC_SINGLE("AMPOUT1_LOW_THD", DAIF_AMPOUTTHD1, 4, 0xf, 0),
	SOC_SINGLE("AMPOUT2_LOW_THD", DAIF_AMPOUTTHD2, 0, 0xf, 0),
	SOC_SINGLE("AMPOUT3_LOW_THD", DAIF_AMPOUTTHD2, 4, 0xf, 0),
	SOC_SINGLE("MICPWR0_EN", DAIF_AFECFG1, 1, 1, 0),
	SOC_SINGLE("MICPWR1_EN", DAIF_AFECFG1, 3, 1, 0),
	SOC_SINGLE("MICPWR0_CURR_LIM_EN", DAIF_AFECFG1, 0, 1, 0),
	SOC_SINGLE("MICPWR1_CURR_LIM_EN", DAIF_AFECFG1, 2, 1, 0),
	SOC_SINGLE("AFE_DITHER_ON", DAIF_LQSD_CFG1, 0, 1, 0),
	SOC_SINGLE("MUTE_REQ_INT_EN", DAIF_LQSD_CFG1, 1, 1, 0),
	SOC_SINGLE("MUTE_STAT_EN", DAIF_LQSD_CFG1, 2, 1, 0),
	SOC_SINGLE("ADC2_EN", DAIF_LQSD_CFG1, 3, 1, 0),
	SOC_SINGLE("DAC0_EN", DAIF_LQSD_CFG1, 4, 1, 0),
	SOC_SINGLE("DAC1_EN", DAIF_LQSD_CFG1, 5, 1, 0),
	SOC_SINGLE("ADC0_EN", DAIF_LQSD_CFG1, 6, 1, 0),
	SOC_SINGLE("ADC1_EN", DAIF_LQSD_CFG1, 7, 1, 0),
	SOC_SINGLE("AMPOUT0_EN", DAIF_AFEAMPEN, 0, 1, 0),
	SOC_SINGLE("AMPOUT1_EN", DAIF_AFEAMPEN, 1, 1, 0),
	SOC_SINGLE("AMPOUT2_EN", DAIF_AFEAMPEN, 2, 1, 0),
	SOC_SINGLE("AMPOUT3_EN", DAIF_AFEAMPEN, 3, 1, 0),
	SOC_SINGLE("SINGIN0_EN", DAIF_AFEAMPEN, 4, 1, 0),
	SOC_SINGLE("SINGIN1_EN", DAIF_AFEAMPEN, 5, 1, 0),
	SOC_SINGLE("DIFFIN0_EN", DAIF_AFEAMPEN, 6, 1, 0),
	SOC_SINGLE("DIFFIN1_EN", DAIF_AFEAMPEN, 7, 1, 0),
};

static const struct snd_kcontrol_new dvf9a_sup_mode_control =
	SOC_SINGLE("DCLASS_SUP_MODE", DAIF_DCLS_ANA_CTRL, 6, 1, 0);

static int dvf9a_codec_probe(struct snd_soc_component *component)
{
	struct device *dev = component->dev;
	struct regmap *regmap = dev_get_regmap(dev->parent, NULL);

	if (!regmap)
		return -ENODEV;

	snd_soc_component_init_regmap(component, regmap);

	if (of_property_read_bool(dev->of_node, "dspg,filterless"))
		return snd_soc_component_update_bits(component,
						     DAIF_DCLS_ANA_CTRL,
						     DCLS_ANA_CTRL_SUP_MODE,
						     0) < 0 ? -EIO : 0;

	return snd_soc_add_component_controls(component,
					      &dvf9a_sup_mode_control, 1);
}

static const struct snd_soc_component_driver dvf9a_codec_driver = {
	.name = "dvf9a-codec",
	.probe = dvf9a_codec_probe,
	.controls = dvf9a_controls,
	.num_controls = ARRAY_SIZE(dvf9a_controls),
};

static int dvf9a_codec_platform_probe(struct platform_device *pdev)
{
	return devm_snd_soc_register_component(&pdev->dev, &dvf9a_codec_driver,
					       NULL, 0);
}

static const struct of_device_id dvf9a_codec_of_match[] = {
	{ .compatible = "dspg,dvf9a-codec" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf9a_codec_of_match);

static struct platform_driver dvf9a_codec_platform_driver = {
	.probe = dvf9a_codec_platform_probe,
	.driver = {
		.name = "dvf9a-codec",
		.of_match_table = dvf9a_codec_of_match,
	},
};
module_platform_driver(dvf9a_codec_platform_driver);

MODULE_DESCRIPTION("DSPG DVF9A audio front end controls");
MODULE_LICENSE("GPL");
