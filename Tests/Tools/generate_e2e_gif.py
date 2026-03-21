#!/usr/bin/env python3

import argparse
import math
import pathlib
import re
import subprocess
import sys


LOG_PATTERN = re.compile(r"^\[(?P<seconds>\d+\.\d{3})s\] \[(?P<name>[^\]]+)\] (?P<message>.*)$")


def read_lines(log_path: pathlib.Path):
    entries = []
    for raw_line in log_path.read_text(encoding="utf-8").splitlines():
        match = LOG_PATTERN.match(raw_line.strip())
        if not match:
            continue
        rendered_line = f"[{match.group('seconds')}s] [{match.group('name')}] {match.group('message')}"
        entries.append((float(match.group("seconds")), rendered_line))
    return entries


def ass_time(seconds: float) -> str:
    total_cs = max(0, int(round(seconds * 100)))
    hours = total_cs // 360000
    minutes = (total_cs % 360000) // 6000
    secs = (total_cs % 6000) // 100
    centis = total_cs % 100
    return f"{hours}:{minutes:02d}:{secs:02d}.{centis:02d}"


def escape_ass(text: str) -> str:
    return text.replace("\\", r"\\").replace("{", r"\{").replace("}", r"\}")


def write_ass(entries, output_path: pathlib.Path, duration: float, max_lines: int):
    header = """[Script Info]
ScriptType: v4.00+
PlayResX: 860
PlayResY: 720

[V4+ Styles]
Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding
Style: Default,Menlo,19,&H00E8E8E8,&H000000FF,&H00101010,&H80101010,0,0,0,0,100,100,0,0,1,1,0,7,18,18,18,1

[Events]
Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text
"""

    lines = [header]
    if not entries:
        entries = [(0.0, "No E2E logs captured")]

    grouped_entries = []
    for seconds, line in entries:
        if grouped_entries and math.isclose(grouped_entries[-1][0], seconds, abs_tol=1e-6):
            grouped_entries[-1][1].append(line)
        else:
            grouped_entries.append((seconds, [line]))

    seen_lines = []
    for index, (start, new_lines) in enumerate(grouped_entries):
        end = duration if index == len(grouped_entries) - 1 else max(start + 0.05, grouped_entries[index + 1][0])
        seen_lines.extend(new_lines)
        window = seen_lines[-max_lines:]
        text = r"\N".join(escape_ass(line) for line in window)
        lines.append(
            f"Dialogue: 0,{ass_time(start)},{ass_time(end)},Default,,0,0,0,,{text}"
        )

    output_path.write_text("\n".join(lines), encoding="utf-8")


def probe_duration(video_path: pathlib.Path) -> float:
    command = [
        "ffprobe",
        "-v",
        "error",
        "-show_entries",
        "format=duration",
        "-of",
        "default=noprint_wrappers=1:nokey=1",
        str(video_path),
    ]
    result = subprocess.run(command, check=True, capture_output=True, text=True)
    return float(result.stdout.strip())


def run(command):
    subprocess.run(command, check=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--frames-dir", required=True)
    parser.add_argument("--log-file", required=True)
    parser.add_argument("--output-gif", required=True)
    parser.add_argument("--fps", type=int, default=12)
    parser.add_argument("--game-width", type=int, default=600)
    parser.add_argument("--panel-width", type=int, default=860)
    parser.add_argument("--height", type=int, default=720)
    parser.add_argument("--max-lines", type=int, default=20)
    args = parser.parse_args()

    frames_dir = pathlib.Path(args.frames_dir)
    log_file = pathlib.Path(args.log_file)
    output_gif = pathlib.Path(args.output_gif)
    working_dir = output_gif.parent
    working_dir.mkdir(parents=True, exist_ok=True)

    gameplay_mp4 = working_dir / "gameplay.mp4"
    panel_mp4 = working_dir / "logs.mp4"
    ass_file = working_dir / "logs.ass"

    run([
        "ffmpeg",
        "-y",
        "-framerate",
        str(args.fps),
        "-i",
        str(frames_dir / "frame-%05d.png"),
        "-vf",
        f"scale={args.game_width}:{args.height}:force_original_aspect_ratio=decrease,pad={args.game_width}:{args.height}:(ow-iw)/2:(oh-ih)/2:color=0x101820",
        "-pix_fmt",
        "yuv420p",
        str(gameplay_mp4),
    ])

    duration = probe_duration(gameplay_mp4)
    minimum_duration = 4.0
    if duration < minimum_duration:
        padded_mp4 = working_dir / "gameplay-padded.mp4"
        run([
            "ffmpeg",
            "-y",
            "-i",
            str(gameplay_mp4),
            "-vf",
            f"tpad=stop_mode=clone:stop_duration={minimum_duration - duration:.3f}",
            "-pix_fmt",
            "yuv420p",
            str(padded_mp4),
        ])
        gameplay_mp4 = padded_mp4
        duration = probe_duration(gameplay_mp4)

    entries = read_lines(log_file)
    write_ass(entries, ass_file, duration, args.max_lines)

    run([
        "ffmpeg",
        "-y",
        "-f",
        "lavfi",
        "-i",
        f"color=c=0x14161b:s={args.panel_width}x{args.height}:d={duration:.3f}",
        "-vf",
        f"subtitles={ass_file}",
        "-pix_fmt",
        "yuv420p",
        str(panel_mp4),
    ])

    stack_filter = f"[0:v][1:v]hstack=inputs=2,fps={args.fps},split[v1][v2];[v1]palettegen=stats_mode=diff[p];[v2][p]paletteuse=dither=sierra2_4a"
    run([
        "ffmpeg",
        "-y",
        "-i",
        str(gameplay_mp4),
        "-i",
        str(panel_mp4),
        "-lavfi",
        stack_filter,
        str(output_gif),
    ])

    print(output_gif)


if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as exc:
        print(f"Command failed with exit code {exc.returncode}: {exc.cmd}", file=sys.stderr)
        sys.exit(exc.returncode)