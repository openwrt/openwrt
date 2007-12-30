#include <sound/driver.h>
#include <sound/core.h>
#include <sound/info.h>
#include <sound/control.h>
#include <sound/ac97_codec.h>

#include <asm/olpc.h>
#include "cs5535audio.h"

/*
 * OLPC has an additional feature on top of the regular AD1888 codec features.
 * It has an Analog Input mode that is switched into (after disabling the
 * High Pass Filter) via GPIO.  It is only supported on B2 and later models.
 */

int olpc_ai_enable(struct snd_ac97 *ac97, u8 val)
{
	int err;

	/*
	 * update the High Pass Filter (via AC97_AD_TEST2), and then set
	 * Analog Input mode through a GPIO.
	 */

	if (val) {
		err = snd_ac97_update_bits(ac97, AC97_AD_TEST2,
				1<<AC97_AD_HPFD_SHIFT, 1<<AC97_AD_HPFD_SHIFT);
		geode_gpio_set(OLPC_GPIO_MIC_AC, GPIO_OUTPUT_VAL);
	}
	else {
		err = snd_ac97_update_bits(ac97, AC97_AD_TEST2,
				1<<AC97_AD_HPFD_SHIFT, 0);
		geode_gpio_clear(OLPC_GPIO_MIC_AC, GPIO_OUTPUT_VAL);
	}
	if (err < 0)
		snd_printk(KERN_ERR "Error updating AD_TEST2: %d\n", err);

	return err;
}
EXPORT_SYMBOL_GPL(olpc_ai_enable);

static int snd_cs5535audio_ai_info(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int snd_cs5535audio_ai_get(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = geode_gpio_isset(OLPC_GPIO_MIC_AC,
			GPIO_OUTPUT_VAL);
	return 0;
}

static int snd_cs5535audio_ai_put(struct snd_kcontrol *kcontrol,
		struct snd_ctl_elem_value *ucontrol)
{
	struct cs5535audio *cs5535au = snd_kcontrol_chip(kcontrol);
	struct snd_ac97 *ac97 = cs5535au->ac97;

	olpc_ai_enable(ac97, ucontrol->value.integer.value[0]);

	return 1;
}

static struct snd_kcontrol_new snd_cs5535audio_controls __devinitdata =
{
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "DC Mode Enable",
	.info = snd_cs5535audio_ai_info,
	.get = snd_cs5535audio_ai_get,
	.put = snd_cs5535audio_ai_put,
	.private_value = 0
};

void __devinit olpc_prequirks(struct snd_card *card,
		struct snd_ac97_template *ac97)
{
	/* Bail if this isn't an OLPC platform */
	if (!machine_is_olpc())
		return;

	/* If on an OLPC B3 or higher, invert EAPD. */
	if (olpc_rev_after(OLPC_REV_B2))
		ac97->scaps |= AC97_SCAP_INV_EAPD;
}

int __devinit olpc_quirks(struct snd_card *card, struct snd_ac97 *ac97)
{
	struct snd_ctl_elem_id elem;

	/* Bail if this isn't an OLPC platform */
	if (!machine_is_olpc())
		return 0;

	/* drop the original ad1888 HPF control */
	memset(&elem, 0, sizeof(elem));
	elem.iface = SNDRV_CTL_ELEM_IFACE_MIXER;
	strcpy(elem.name, "High Pass Filter Enable");
	snd_ctl_remove_id(card, &elem);

	/* add the override for OLPC's HPF */
	return snd_ctl_add(card, snd_ctl_new1(&snd_cs5535audio_controls,
			ac97->private_data));
}
