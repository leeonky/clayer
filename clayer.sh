#!/bin/bash
# clayer file [v=index] [a=index] [sub=subtitle_file] [p=position(0,0)] [s=size(1920x1080)] [vf=video_flag(full|opengl|borderless|highdpi)]

project_path="$(dirname "$0")"
media_file=$1

DECODER_BIN="$project_path/decoder/cl_decoder"
SUBTITLE_BIN="$project_path/subtitle/cl_subtitle"
SCREEN_BIN="$project_path/screen/cl_screen"
RESAMPLER_BIN="$project_path/resampler/cl_resampler"
RESCALER_BIN="$project_path/rescaler/cl_rescaler"
SPEEKER_BIN="$project_path/speaker/cl_speaker"
CONTROLLER_BIN="$project_path/controller/cl_controller"
TERMINAL_BIN="$project_path/cl_terminal"

new_sub_file=$(mktemp -u /tmp/cl_sub.srt.XXXXXX)
audio_control_queue=$(mktemp -u /tmp/cl_ab.XXXXXX)
video_control_queue=$(mktemp -u /tmp/cl_vb.XXXXXX)
clock_control_queue=$(mktemp -u /tmp/cl_cb.XXXXXX)

mkfifo "$clock_control_queue"

trap "{ rm -f $new_sub_file $clock_control_queue; }" EXIT

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

if [ "$subtitle" == "" ]; then
	srt_name="${media_file%.*}.srt"
	if [ -f "$srt_name" ]; then
		subtitle="$srt_name"
	fi
fi

if [ "$subtitle" != "" ]; then
	encoding=$(file -b --mime-encoding "$subtitle")
	if [ $encoding != 'utf-8' ]; then
		encoding=${encoding/%le/}
		encoding=${encoding/%be/}
		iconv -f $encoding -t utf-8 "$subtitle" > "$new_sub_file"
		#sed -i '1s/^\xEF\xBB\xBF//' "$new_sub_file"
		subtitle="$new_sub_file"
	fi
fi

function play_audio() {
	"$DECODER_BIN" "$media_file" -a $audio |
	"$RESAMPLER_BIN" -f pack:flt32:maxbit32 -l stereo |
	"$SPEEKER_BIN" -d 1
}

function play_audio_with_controller() {
	(
	play_audio | tee >( grep --line-buffered 'CLOCK' > "$clock_control_queue" ) | grep -v --line-buffered 'CLOCK' &
	cat
	) | "$CONTROLLER_BIN"
}

function wrapper_video_with_subtitle() {
	if [ "$subtitle" == "" ]; then
		"$DECODER_BIN" "$media_file" -v $video | "$RESCALER_BIN" -m yuv420p10le -f yuv420p
	else
		"$DECODER_BIN" "$media_file" -v $video | "$RESCALER_BIN" -m yuv420p10le -f yuv420p | "$SUBTITLE_BIN" -f "$project_path/wqy-zenhei.ttc" "$subtitle"
	fi
}

function play_video() {
	(
	wrapper_video_with_subtitle &
	cat $clock_control_queue
	) | "$SCREEN_BIN" $position $size $video_flag
}

function play_video_with_controller() {
	(
	play_video &
	cat
	) | "$CONTROLLER_BIN"
}

set -x
"$TERMINAL_BIN" | tee >( play_video_with_controller ) | play_audio_with_controller

