#!/bin/bash
# clayer file [v=index] [a=index] [sub=subtitle_file]

project_path="$(dirname "$0")"
media_file=$1

shift 1
for args in "$@"
do
        eval "$args"
done

set -x

video=${v:-0}
audio=${a:-0}
subtitle=${sub:-}

(
	"$project_path/decoder/decoder" "$media_file" -v $video &
	"$project_path/decoder/decoder" "$media_file" -a $audio | "$project_path/resampler/resampler" -f pack:flt32:maxbit32 -lmax5.1 | "$project_path/speaker/speaker"
) | "$project_path/screen/screen"
