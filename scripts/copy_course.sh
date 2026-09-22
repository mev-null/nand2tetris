#!/bin/sh
# Copy the course's projects/NN directories into this repository's projects/NN-<name> layout.
# The hardware chapters keep the course files next to the .hdl files they test; the
# software chapters keep them under course/, apart from the implementation.
# usage: scripts/copy_course.sh /tmp/n2t-projects/projects
# cp -n never overwrites, so authored files stay untouched and re-running is safe.
set -eu

src=${1:?usage: $0 COURSE_PROJECTS_DIR}
root=$(cd "$(dirname "$0")/.." && pwd)

dest_of() {
  case $1 in
    01) echo 01-boolean-logic ;;
    02) echo 02-boolean-arithmetic ;;
    03) echo 03-memory ;;
    04) echo 04-machine-language ;;
    05) echo 05-computer-architecture ;;
    06) echo 06-assembler/course ;;
    07 | 08) echo 07-08-vm-translator/course ;;
    09) echo 09-high-level-language/course ;;
    # both chapters ship a different Square/, so each keeps its own directory
    10 | 11) echo "10-11-compiler/course/$1" ;;
    12) echo 12-operating-system/course ;;
    # 00 and 13 hold no course projects
    *) echo "" ;;
  esac
}

for dir in "$src"/*/; do
  chapter=$(basename "$dir")
  dest=$(dest_of "$chapter")
  [ -n "$dest" ] || continue
  mkdir -p "$root/projects/$dest"
  # macOS cp -n exits 1 when it skips a file that already exists, which is the point here
  if [ "$chapter" = 12 ]; then
    # The OS skeletons at the top of 12 are tracked as authored files; only the tests go
    # to course/.
    for test_dir in "$dir"*/; do
      cp -Rn "${test_dir%/}" "$root/projects/$dest/" || true
    done
  else
    cp -Rn "$dir". "$root/projects/$dest/" || true
  fi
done
