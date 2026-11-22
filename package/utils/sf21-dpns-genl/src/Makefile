GENL_FILES = main.c utils.c genl.c l2.c nat.c vlan.c router.c common.c tmu.c mcast.c acl.c
GENL_CFLAGS = $(CFLAGS) -DCONFIG_SIFLOWER_GENL_TMU -DCONFIG_SIFLOWER_GENL_MCAST -DCONFIG_SIFLOWER_GENL_ACL
sf_genl_user: $(patsubst %.c,%.o,$(GENL_FILES))
	$(CC) -o $@ $^ $(LDFLAGS)
%.o: %.c
	$(CC) $(GENL_CFLAGS) -c -o $@ $<
clean:
	rm -f *.o sf_genl_user
