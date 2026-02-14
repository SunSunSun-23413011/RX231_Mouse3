#!/usr/bin/env python3
# midi2sound_macrotime.py
#
# MIDI -> SOUND_T array (Sound.h macro time style)
#
# Features:
# - Parse Sound.h to get note macros, time macros, and special marks (RST_/RPT_/STP_)
# - Notes that do not exist in Sound.h are treated as REST (RST_) (no numeric tone counts are output)
# - Polyphonic reduction: highest/lowest among notes that exist in Sound.h (after transpose)
# - Two parts (instrument/hand) can be merged into one speaker:
#     * If hands are explicitly left/right, left is primary and right is fallback
#     * (left rest -> right note)
#     * Otherwise part1 is primary and part2 is fallback
#     * --prefer is used only to choose one note inside each part when polyphonic
# - "Hand split" inside the same channel: left/right decided by pitch threshold (--hand-split)
# - REST-only time processing:
#     * --rest-div can shorten rests (1/2/4/8)
#     * subtract TIT_ from ALL rests by default (--rest-sub-tit 1)
#     * minimum REST duration is TIT_ (unless it becomes 0 after subtraction, then it is omitted)
# - Tempo:
#     * If --bpm is omitted, use the first MIDI set_tempo if present; otherwise 120
#     * Durations are converted to "tempo=120 note-value macros" by scaling (120 / bpm)
# - TIME expressions are emitted using macros: T00_, T02_, T04_, T08_, T16_, T32_, TIT_
#   (minimum emitted unit is TIT_)
#
# Requirements:
#   pip install mido
#
# Example:
#   python midi2sound_macrotime.py Coconut_Mall_1.mid --sound-h Sound.h --name ccnt1 \
#     --channel 0 --hand right --channel2 0 --hand2 left --hand-split 60 \
#     --prefer highest --rest-div 8 --rest-sub-tit 1 --out ccnt1.c --encoding cp932

import argparse
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

import mido
import re
import sys


# -------------------- Sound.h parsing --------------------

_PITCH_BASE = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}

def _read_text_guess_encoding(path: str) -> str:
    for enc in ("utf-8", "cp932", "shift_jis", "utf-8-sig", "latin-1"):
        try:
            with open(path, "r", encoding=enc) as f:
                return f.read()
        except Exception:
            continue
    with open(path, "rb") as f:
        return f.read().decode("utf-8", errors="replace")

def _parse_define_value(line: str) -> Optional[int]:
    # Match: #define NAME ( 1234UL) ... or #define NAME 1234 ...
    m = re.search(r"#define\s+\w+\s+\(?\s*(0x[0-9A-Fa-f]+|\d+)", line)
    if not m:
        return None
    s = m.group(1)
    return int(s, 16) if s.lower().startswith("0x") else int(s, 10)

def _parse_comment_pitch(line: str) -> Optional[int]:
    # Look for "(C#4)" or "(D5)" inside comments
    m = re.search(r"\(([A-G])(#?)(-?\d+)\)", line)
    if not m:
        return None
    letter = m.group(1)
    sharp = m.group(2) == "#"
    octave = int(m.group(3))
    semitone = _PITCH_BASE[letter] + (1 if sharp else 0)
    midi = (octave + 1) * 12 + semitone
    if 0 <= midi <= 127:
        return midi
    return None

@dataclass(frozen=True)
class SoundHInfo:
    note_midi_to_macro: Dict[int, str]
    time_macros: Dict[str, int]
    stp_name: str
    rpt_name: str
    rst_name: str

def parse_sound_h(path: str) -> SoundHInfo:
    text = _read_text_guess_encoding(path)
    lines = text.splitlines()

    time_macros: Dict[str, int] = {}
    note_midi_to_macro: Dict[int, str] = {}

    stp_name = "STP_"
    rpt_name = "RPT_"
    rst_name = "RST_"

    for ln in lines:
        s = ln.strip()
        if not s.startswith("#define"):
            continue

        mname = re.match(r"#define\s+(\w+)", s)
        if not mname:
            continue
        name = mname.group(1)

        val = _parse_define_value(s)
        if val is None:
            continue

        if name in ("STP_", "RPT_", "RST_"):
            if name == "STP_":
                stp_name = name
            elif name == "RPT_":
                rpt_name = name
            elif name == "RST_":
                rst_name = name
            continue

        if name.startswith("T") or name == "TIT_":
            time_macros[name] = val
            continue

        # Note macro: try to map by comment pitch (preferred)
        midi = _parse_comment_pitch(s)
        if midi is not None:
            # Keep first occurrence if duplicates
            note_midi_to_macro.setdefault(midi, name)

    # Minimal required time macros
    required = ["T00_", "T02_", "T04_", "T08_", "T16_", "T32_", "TIT_"]
    missing = [k for k in required if k not in time_macros]
    if missing:
        raise SystemExit(f"error: Sound.h is missing time macro(s): {', '.join(missing)}")

    if not note_midi_to_macro:
        raise SystemExit("error: No note macros with (C4) style comments were found in Sound.h.")

    return SoundHInfo(
        note_midi_to_macro=note_midi_to_macro,
        time_macros=time_macros,
        stp_name=stp_name,
        rpt_name=rpt_name,
        rst_name=rst_name,
    )


# -------------------- Time tokenization (units = 1/64 note, minimum = TIT_) --------------------

@dataclass(frozen=True)
class Tok:
    units64: int
    expr: str

def build_tokens(info: SoundHInfo) -> List[Tok]:
    # Only emit down to TIT_ (minimum requested)
    toks: List[Tok] = []
    # Whole/half/quarter/eighth/sixteenth/thirty-second + TIT_
    toks.append(Tok(64, "T00_"))
    toks.append(Tok(48, "T02_ + (T02_/2)"))
    toks.append(Tok(32, "T02_"))
    toks.append(Tok(24, "T04_ + (T04_/2)"))
    toks.append(Tok(16, "T04_"))
    toks.append(Tok(12, "T08_ + (T08_/2)"))
    toks.append(Tok(8,  "T08_"))
    toks.append(Tok(6,  "T16_ + (T16_/2)"))
    toks.append(Tok(4,  "T16_"))
    toks.append(Tok(2,  "T32_"))
    toks.append(Tok(1,  "TIT_"))
    return toks

def best_decompose_units64(units64: int, tokens: List[Tok]) -> List[Tok]:
    # Exact decomposition minimizing token count
    INF = 10**9
    dp_cost = [INF] * (units64 + 1)
    dp_prev: List[Optional[Tuple[int, int]]] = [None] * (units64 + 1)  # (prev_i, token_index)
    dp_cost[0] = 0

    for i in range(units64 + 1):
        if dp_cost[i] == INF:
            continue
        for ti, tok in enumerate(tokens):
            j = i + tok.units64
            if j <= units64 and dp_cost[i] + 1 < dp_cost[j]:
                dp_cost[j] = dp_cost[i] + 1
                dp_prev[j] = (i, ti)

    if dp_cost[units64] == INF:
        # fallback: all TIT_
        tit = next(t for t in tokens if t.units64 == 1)
        return [tit] * units64

    out: List[Tok] = []
    cur = units64
    while cur > 0:
        prev = dp_prev[cur]
        if prev is None:
            break
        pi, ti = prev
        out.append(tokens[ti])
        cur = pi
    out.reverse()
    return out

def units64_to_expr(units64: int, tokens: List[Tok]) -> str:
    toks = best_decompose_units64(units64, tokens)
    return " + ".join(t.expr for t in toks)

def split_long_duration(note_str: str, units64: int, tokens: List[Tok]) -> List[Tuple[str, str]]:
    # Keep each chunk <= 64 (<= T00_) for readability
    out: List[Tuple[str, str]] = []
    remaining = units64
    while remaining > 0:
        chunk = min(64, remaining)
        out.append((note_str, units64_to_expr(chunk, tokens)))
        remaining -= chunk
    return out


# -------------------- MIDI processing --------------------

@dataclass(frozen=True)
class PartCfg:
    channel: Optional[int]      # None = all channels
    hand: str                  # "all" | "left" | "right"
    transpose: int

def _hand_of_note(note: int, split: int) -> str:
    return "left" if note < split else "right"

def _note_exists_in_soundh(info: SoundHInfo, midi_note: int) -> bool:
    return midi_note in info.note_midi_to_macro

def pick_note_from_active(
    info: SoundHInfo,
    active_notes: set,
    prefer: str,
    transpose: int
) -> Optional[int]:
    # Return transposed midi note that exists in Sound.h, else None
    if not active_notes:
        return None
    candidates: List[int] = []
    for raw in active_notes:
        nn = raw + transpose
        if 0 <= nn <= 127 and _note_exists_in_soundh(info, nn):
            candidates.append(nn)
    if not candidates:
        return None
    return max(candidates) if prefer == "highest" else min(candidates)

def merge_primary_with_fallback(primary: Optional[int], fallback: Optional[int]) -> Optional[int]:
    # Keep part1 while it has a note; use part2 only during part1 rests.
    return primary if primary is not None else fallback

def merge_left_priority(
    part1: PartCfg, n1: Optional[int],
    part2: PartCfg, n2: Optional[int]
) -> Optional[int]:
    # If two hands are explicitly split, always prefer left; use right only on left rests.
    if part1.hand == "left" and part2.hand == "right":
        return n1 if n1 is not None else n2
    if part1.hand == "right" and part2.hand == "left":
        return n2 if n2 is not None else n1
    return merge_primary_with_fallback(n1, n2)

def first_tempo_bpm(mid: mido.MidiFile) -> Optional[float]:
    for tr in mid.tracks:
        for msg in tr:
            if msg.type == "set_tempo":
                try:
                    return float(mido.tempo2bpm(msg.tempo))
                except Exception:
                    return None
    return None

def extract_segments(
    mid: mido.MidiFile,
    info: SoundHInfo,
    part1: PartCfg,
    part2: Optional[PartCfg],
    prefer: str,
    hand_split: int
) -> List[Tuple[Optional[int], int]]:
    merged = mido.merge_tracks(mid.tracks)

    active1: set = set()
    active2: set = set()

    cur_note: Optional[int] = None
    out: List[Tuple[Optional[int], int]] = []

    def emit(dt_ticks: int) -> None:
        nonlocal out, cur_note
        if dt_ticks <= 0:
            return
        if out and out[-1][0] == cur_note:
            out[-1] = (out[-1][0], out[-1][1] + dt_ticks)
        else:
            out.append((cur_note, dt_ticks))

    def accept_event_for_part(msg, cfg: PartCfg) -> bool:
        if cfg.channel is not None and getattr(msg, "channel", None) != cfg.channel:
            return False
        if cfg.hand == "all":
            return True
        if not hasattr(msg, "note"):
            return False
        h = _hand_of_note(msg.note, hand_split)
        return (h == cfg.hand)

    def recompute_current() -> None:
        nonlocal cur_note
        n1 = pick_note_from_active(info, active1, prefer, part1.transpose)
        if part2 is None:
            cur_note = n1
            return
        n2 = pick_note_from_active(info, active2, prefer, part2.transpose)
        cur_note = merge_left_priority(part1, n1, part2, n2)

    # Initial note is REST
    recompute_current()

    for msg in merged:
        if msg.time:
            emit(msg.time)

        if msg.type in ("note_on", "note_off"):
            is_on = (msg.type == "note_on" and msg.velocity > 0)
            is_off = (msg.type == "note_off") or (msg.type == "note_on" and msg.velocity == 0)

            if accept_event_for_part(msg, part1):
                if is_on:
                    active1.add(msg.note)
                elif is_off:
                    active1.discard(msg.note)

            if part2 is not None and accept_event_for_part(msg, part2):
                if is_on:
                    active2.add(msg.note)
                elif is_off:
                    active2.discard(msg.note)

            recompute_current()

    return out


# -------------------- Tick -> macro units (1/64 note base, scaled to tempo=120 macros) --------------------

def ticks_to_units64_scaled(dt_ticks: int, tpq: int, bpm: float) -> int:
    # musical units64 (1/64 note) = dt_ticks * 16 / tpq (since 1 beat = quarter = 16 units64)
    musical = (dt_ticks * 16.0) / float(tpq)
    scale = 120.0 / float(bpm)  # convert to "tempo=120 macros" time
    u = int(round(musical * scale))
    return max(1, u)


# -------------------- Main --------------------

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("midi", help="input MIDI file (.mid)")
    ap.add_argument("--sound-h", default="Sound.h", help="path to Sound.h to parse")
    ap.add_argument("--name", default="Melody_MIDI", help="C array name")

    ap.add_argument("--channel", type=int, default=None, help="part1 MIDI channel 0-15 (omit = all)")
    ap.add_argument("--hand", choices=["all", "left", "right"], default="all", help="part1 hand filter")
    ap.add_argument("--transpose", type=int, default=0, help="part1 transpose (semitones)")

    ap.add_argument("--channel2", type=int, default=None,
                    help="part2 MIDI channel 0-15 (fallback source; omit = disabled)")
    ap.add_argument("--hand2", choices=["all", "left", "right"], default="all", help="part2 hand filter")
    ap.add_argument("--transpose2", type=int, default=0, help="part2 transpose (semitones)")

    ap.add_argument("--hand-split", type=int, default=60, help="hand split threshold (MIDI note), default C4=60")

    ap.add_argument("--prefer", choices=["highest", "lowest"], default="highest",
                    help="note selection policy inside each part when polyphonic")

    ap.add_argument("--bpm", type=float, default=None,
                    help="source BPM. omit = use first MIDI set_tempo if present, else 120")

    ap.add_argument("--rest-div", type=int, default=1, choices=[1, 2, 4, 8],
                    help="shorten REST duration only (1=normal, 2=half, 4=quarter, 8=eighth)")
    ap.add_argument("--rest-sub-tit", type=int, default=1,
                    help="subtract TIT_ units from ALL rests (default 1). can be 0 to disable")
    ap.add_argument("--min-rest", type=int, default=1,
                    help="minimum REST units64 when outputting a rest (default 1=TIT_)")
    ap.add_argument("--min-note", type=int, default=2,
                    help="minimum NOTE units64 (default 2=T32_)")

    ap.add_argument("--loop", type=int, default=0, help="1: end with RPT_, 0: end with STP_")

    ap.add_argument("--out", default=None, help="output .c file path (omit = stdout)")
    ap.add_argument("--encoding", default="cp932", help="output encoding when --out is used")
    ap.add_argument("--errors", default="strict", choices=["strict", "replace", "ignore"],
                    help="encoding error handling when writing file (only for --out)")

    args = ap.parse_args()

    info = parse_sound_h(args.sound_h)
    tokens = build_tokens(info)

    mid = mido.MidiFile(args.midi)
    tpq = mid.ticks_per_beat

    bpm = args.bpm
    if bpm is None:
        bpm0 = first_tempo_bpm(mid)
        bpm = bpm0 if bpm0 is not None else 120.0
    if bpm <= 0:
        raise SystemExit("error: bpm must be > 0")

    part1 = PartCfg(channel=args.channel, hand=args.hand, transpose=args.transpose)
    part2 = None
    if args.channel2 is not None:
        part2 = PartCfg(channel=args.channel2, hand=args.hand2, transpose=args.transpose2)

    segments = extract_segments(
        mid=mid,
        info=info,
        part1=part1,
        part2=part2,
        prefer=args.prefer,
        hand_split=args.hand_split,
    )

    entries: List[Tuple[str, str]] = []

    for note_midi, dt_ticks in segments:
        units64 = ticks_to_units64_scaled(dt_ticks, tpq, bpm)

        if note_midi is None:
            # REST only processing
            if args.rest_div > 1:
                units64 = int(round(units64 / float(args.rest_div)))
                units64 = max(0, units64)

            if args.rest_sub_tit > 0:
                units64 = max(0, units64 - args.rest_sub_tit)

            if units64 <= 0:
                continue  # omit zero-length rests

            units64 = max(args.min_rest, units64)
            note_str = info.rst_name

        else:
            # NOTE (already transposed and guaranteed to exist in Sound.h)
            units64 = max(args.min_note, units64)
            note_str = info.note_midi_to_macro[note_midi]

        entries.extend(split_long_duration(note_str, units64, tokens))

    end_mark = info.rpt_name if args.loop else info.stp_name

    if args.out:
        f = open(args.out, "w", encoding=args.encoding, errors=args.errors, newline="\r\n")
        def w(s: str = "") -> None:
            f.write(s + "\r\n")
    else:
        f = None
        def w(s: str = "") -> None:
            sys.stdout.write(s + "\n")

    w("/* Auto-generated by midi2sound_macrotime.py */")
    w("/* TIME is emitted as macro expressions (T04_, T08_+(T08_/2), ..., TIT_) from Sound.h */")
    w(f"/* Source BPM used for scaling: {bpm:.3f} */")
    w(f"SOUND_T {args.name}[] = {{")
    for nstr, texpr in entries:
        w(f"    {{{nstr}, {texpr}}},")
    w(f"    {{{end_mark}, T00_}},")
    w("};")

    if f is not None:
        f.close()

if __name__ == "__main__":
    main()
