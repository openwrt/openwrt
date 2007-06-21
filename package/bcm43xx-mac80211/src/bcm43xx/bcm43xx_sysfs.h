#ifndef BCM43xx_SYSFS_H_
#define BCM43xx_SYSFS_H_

struct bcm43xx_wldev;

int bcm43xx_sysfs_register(struct bcm43xx_wldev *dev);
void bcm43xx_sysfs_unregister(struct bcm43xx_wldev *dev);

#endif /* BCM43xx_SYSFS_H_ */
