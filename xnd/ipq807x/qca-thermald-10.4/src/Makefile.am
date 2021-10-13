AM_CFLAGS = -Wall \
		-Wundef \
		-Wstrict-prototypes \
		-Wno-trigraphs \
		-Werror

AM_CPPFLAGS = -DENABLE_TSENS_INTERRUPT \
	      -DENABLE_MODEM_MITIGATION \
	      -DSENSORS_8960 \
	      -DCONFIG_FILE_DEFAULT='"/etc/thermald.conf"' \
              $(QMIF_CFLAGS)

thermald_SOURCES = \
		   thermal.c \
		   thermal_config.c \
		   thermal_monitor.c \
		   thermal_actions.c \
		   thermal_util.c \
		   adc-sensor.c \
		   tsens-sensor.c \
		   sensors-8960.c \
		   modem_mitigation_qmi.c \
		   thermal_mitigation_device_service_v01.c

if USE_GLIB
thermald_CFLAGS = $(AM_CFLAGS) -DUSE_GLIB @GLIB_CFLAGS@
thermald_LDFLAGS = -lpthread -lrt @GLIB_LIBS@ $(QMIF_LIBS)
else
thermald_LDFLAGS = -lpthread -lrt $(QMIF_LIBS)
endif

bin_PROGRAMS = thermald
sysconf_DATA = thermald-9x15.conf
install-data-hook:
	mv $(DESTDIR)$(sysconfdir)/$(sysconf_DATA) $(DESTDIR)$(sysconfdir)/thermald.conf
