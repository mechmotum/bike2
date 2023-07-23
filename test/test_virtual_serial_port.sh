#!/usr/bin/env bash

set -euo pipefail

# https://bazel.build/reference/test-encyclopedia#test-interaction-filesystem
TEST_TMPDIR="${TEST_TEMPDIR:-.}"

dev="$TEST_TMPDIR/virt0"
log="$TEST_TMPDIR/out"
bin=$1

rm -f "$log"

socat PTY,link="$dev",raw,echo=0 OPEN:"$log",creat=1,excl=1 &
socat_pid=$!

while [ ! -e "$dev" ]; do
    sleep 0.1
done

"$bin" "$dev"

kill "$socat_pid"

grep -q "hello, world!" "$log"
