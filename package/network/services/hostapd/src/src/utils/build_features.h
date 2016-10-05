#ifndef BUILD_FEATURES_H
#define BUILD_FEATURES_H

static inline int has_feature(const char *feat)
{
#ifdef IEEE8021X_EAPOL
	if (!strcmp(feat, "eap"))
		return 1;
#endif
#ifdef IEEE80211N
	if (!strcmp(feat, "11n"))
		return 1;
#endif
	return 0;
}

#endif /* BUILD_FEATURES_H */
