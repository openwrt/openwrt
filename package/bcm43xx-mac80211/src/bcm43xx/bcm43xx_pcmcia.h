#ifndef BCM43xx_PCMCIA_H_
#define BCM43xx_PCMCIA_H_

#ifdef CONFIG_BCM43XX_MAC80211_PCMCIA

int bcm43xx_pcmcia_init(void);
void bcm43xx_pcmcia_exit(void);

#else /* CONFIG_BCM43XX_MAC80211_PCMCIA */

static inline
int bcm43xx_pcmcia_init(void)
{
	return 0;
}
static inline
void bcm43xx_pcmcia_exit(void)
{
}

#endif /* CONFIG_BCM43XX_MAC80211_PCMCIA */
#endif /* BCM43xx_PCMCIA_H_ */
