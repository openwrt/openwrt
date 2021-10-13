
ifeq (linux, $(OS))
  ifeq (KSLIB, $(MODULE_TYPE))
    ifeq (TRUE, $(KERNEL_MODE))
      COMPONENTS = HSL SAL INIT UTIL REF SHELIB
      ifeq (TRUE, $(FAL))
        COMPONENTS += FAL ADPT
      endif
    else
      COMPONENTS = HSL SAL INIT REF
    endif

    ifeq (TRUE, $(UK_IF))
      COMPONENTS += API
    endif
  endif
  
  ifeq (USLIB, $(MODULE_TYPE))
    ifneq (TRUE, $(KERNEL_MODE))
      COMPONENTS = HSL SAL INIT UTIL REF
      ifeq (TRUE, $(FAL))
        COMPONENTS += FAL ADPT
      endif
    else
      COMPONENTS = UK_IF SAL
    endif

    ifeq (TRUE, $(UK_IF))
      COMPONENTS += API
    endif
  endif

  ifeq (SHELL, $(MODULE_TYPE))
    COMPONENTS = SHELL
  endif
endif
