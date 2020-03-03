#!/bin/bash
set -eu

NEXT="${1:-master}"

cd "$(dirname $0)"
git-chglog --next-tag "$NEXT" > CHANGELOG.raw.md
git diff CHANGELOG.raw.md
