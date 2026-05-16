# ci:target:x86:64 is going to trigger CI target check jobs for x86/64 target

name: Build check target specified in labels
on:
  pull_request:
    types:
      - labeled

jobs:
  build-target-label:
    name: Build target from defined label
    permissions:
      contents: read
      packages: read
      actions: write
    uses: openwrt/actions-shared-workflows/.github/workflows/label-target.yml@main
