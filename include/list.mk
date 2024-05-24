# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2022 OpenWrt.org

ifneq ($(__list_inc),1)
__list_inc=1
endif

# set this non-empty for verbose dump of each $(call List/AddUniq)
LIST_DEBUG?=

# List/AddUniq(listvar, items)
#  adds $(items) to listvar
define List/AddUniq
  $(if $(LIST_DEBUG),
    $(if $(filter-out 0 1,$(words $(strip $(2)))),
      $(foreach item,$(strip $(2)),$(info List/AddUniq,$(strip $(1)),$(item))),
      $(if $(filter $(strip $(2)),$($(strip $(1)))),,$(info eval $(strip $(1)) += $(strip $(2))))
    )
  )
  $(if $(filter-out 0 1,$(words $(strip $(2)))),
    $(foreach item,$(strip $(2)),$(call List/AddUniq,$(strip $(1)),$(item))),
    $(if $(filter $(strip $(2)),$($(strip $(1)))),,$(eval $(strip $(1)) += $(strip $(2))))
  )
  $(if $(LIST_DEBUG),
    $(if $(filter $(strip $(1)),$(curdir)-y),
      $(if $(filter $($(curdir)-y),$($(curdir)-n)),
        $(info y-prunes-n eval $(curdir)-n:=$(strip $(filter-out $($(curdir)-y),$($(curdir)-n))))
      )
    )
  )
  $(if $(filter $(strip $(1)),$(curdir)-y),
    $(if $(filter $($(curdir)-y),$($(curdir)-n)),
      $(eval $(curdir)-n:=$(strip $(filter-out $($(curdir)-y),$($(curdir)-n))))
    )
  )
endef

# List/AddUniqIf(condition, listvar, items)
#  if non-empty condition, adds $(items) to $(listvar)-y
#  if     empty condition, adds $(items) to $(listvar)-n
define List/AddUniqIf
  $(call List/AddUniq,$(strip $(2))$(if $(strip $(1)),-y,-n),$(strip $(3)))
endef

# List/DepAddUniq(action, package, deps)
#  adds $(curdir)/dep/action to $(curdir)/package/action
define List/DepAddUniq
  $(foreach item,$(strip $(3)),
    $(call List/AddUniq,
      $(curdir)/$(strip $(2))/$(strip $(1)),
      $(curdir)/$(item)/$(strip $(1))
    )
  )
endef
