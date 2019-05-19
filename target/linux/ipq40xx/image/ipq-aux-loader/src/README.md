# qcom-ipq-aux-loader

This auxiliary bootloader was created to be able to run OpenWRT on devices of the Mikrotik company with is based on IPQ-4XXX and IPQ-806X series processors.
The thing is that a RouterBOOT is able to load only programs in ELF format and a OpenWRT kernel has a format of uImage Legacy or uImage FIT.

This bootloader with a small fixes can potentially be used on any ARM system(V5 and higher). Dependence from hardware here is minimal.
