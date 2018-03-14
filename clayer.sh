#!/bin/bash
# clayer file [v=index] [a=index] [sub=subtitle_file] [p=position(0,0)] [s=size(1920x1080)] [vf=video_flag(full|opengl|borderless)]

project_path="$(dirname "$0")"
media_file=$1

shift 1
for args in "$@"
do
        eval "$args"
done

video=${v:-0}
audio=${a:-0}
subtitle=${sub:-}

position=${p:-}
if [ "$position" != "" ]; then
	position="-p $position"
fi

size=${s:-}
if [ "$size" != "" ]; then
	size="-s $size"
fi

video_flag=${vf:-}
if [ "$video_flag" != "" ]; then
	video_flag="-f $video_flag"
fi

set -x
(
	"$project_path/decoder/decoder" "$media_file" -v $video &
	"$project_path/decoder/decoder" "$media_file" -a $audio | "$project_path/resampler/resampler" -f pack:flt32:maxbit32 -l stereo | "$project_path/speaker/speaker" -d 1
) | "$project_path/screen/screen" $position $size $video_flag

