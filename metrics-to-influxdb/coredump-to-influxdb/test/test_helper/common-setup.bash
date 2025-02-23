#!/usr/bin/env bash

_common_setup() {
    load 'test_helper/bats-mock/stub'
    load 'test_helper/bats-assert/load'
    load 'test_helper/bats-support/load'
    load '../functions.sh'
    DIR="$(cd "$(dirname "$BATS_TEST_FILENAME")/.." >/dev/null 2>&1 && pwd)"
    PATH="$DIR/..:$PATH"
}
