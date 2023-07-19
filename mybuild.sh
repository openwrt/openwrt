#!/bin/bash
set -e
./scripts/feeds update -a
./scripts/feeds install -a
