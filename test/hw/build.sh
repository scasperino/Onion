#!/bin/bash
# Build the recent-list test for the Miyoo with all /mnt/SDCARD paths moved to /tmp/sdtest
set -e
rm -rf /tmp/hwsrc && mkdir -p /tmp/hwsrc && cp -r src include /tmp/hwsrc/
grep -rl "/mnt/SDCARD" /tmp/hwsrc/src/common | xargs sed -i 's#/mnt/SDCARD#/tmp/sdtest#g'
S=/tmp/hwsrc
FILES="test/hw/t.c $S/src/common/utils/file.c $S/src/common/utils/str.c $S/src/common/utils/log.c $S/include/cjson/cJSON.c"
FLAGS="-I$S/src/common -I$S/include -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -w -g -O1"
mkdir -p out
$CC $FLAGS -static -o out/recent_test $FILES -lm
if $CC $FLAGS -fsanitize=address,undefined -fno-sanitize=alignment -static-libasan -static-libubsan -o out/recent_test_asan $FILES -lm 2>/tmp/asan.err; then echo "asan build ok"; else echo "asan build failed:"; tail -5 /tmp/asan.err; fi
ls -la out
# "before" build against unpatched main
git fetch -q --depth 1 origin main && rm -rf /tmp/hwmain && mkdir -p /tmp/hwmain && git --work-tree=/tmp/hwmain checkout FETCH_HEAD -- src include && git reset -q
grep -rl "/mnt/SDCARD" /tmp/hwmain/src/common | xargs sed -i 's#/mnt/SDCARD#/tmp/sdtest#g'
M=/tmp/hwmain
$CC -I$M/src/common -I$M/include -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -w -g -O1 -fsanitize=address,undefined -fno-sanitize=alignment -static-libasan -static-libubsan -o out/before_asan test/hw/t_before.c $M/src/common/utils/file.c $M/src/common/utils/str.c $M/src/common/utils/log.c $M/include/cjson/cJSON.c -lm
ls -la out
