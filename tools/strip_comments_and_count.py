#!/usr/bin/env python3
import os
import sys

root = os.path.join(os.path.dirname(__file__), '..', 'KursovayaGUI')
paths = []
for dirpath, dirnames, filenames in os.walk(root):
    for f in filenames:
        if f.endswith('.cpp') or f.endswith('.h') or f.endswith('.c') or f.endswith('.hpp'):
            paths.append(os.path.join(dirpath, f))

total_lines = 0
processed = 0
errors = []


def strip_comments(text):
    out = []
    i = 0
    n = len(text)
    in_string = False
    in_char = False
    in_line = False
    in_block = False
    esc = False
    while i < n:
        c = text[i]
        nc = text[i + 1] if i + 1 < n else ''
        if in_line:
            if c == '\n':
                in_line = False
                out.append(c)
            i += 1
            continue
        if in_block:
            if c == '*' and nc == '/':
                in_block = False
                i += 2
            else:
                i += 1
            continue
        if in_string:
            out.append(c)
            if c == '"' and not esc:
                in_string = False
            esc = (c == '\\' and not esc)
            i += 1
            continue
        if in_char:
            out.append(c)
            if c == "'" and not esc:
                in_char = False
            esc = (c == '\\' and not esc)
            i += 1
            continue
        # not in any
        if c == '/' and nc == '/':
            in_line = True
            i += 2
            continue
        if c == '/' and nc == '*':
            in_block = True
            i += 2
            continue
        if c == '"':
            in_string = True
            out.append(c)
            esc = False
            i += 1
            continue
        if c == "'":
            in_char = True
            out.append(c)
            esc = False
            i += 1
            continue
        out.append(c)
        i += 1
    return ''.join(out)


for p in paths:
    try:
        with open(p, 'r', encoding='utf-8') as f:
            txt = f.read()
    except Exception:
        try:
            with open(p, 'r', encoding='cp1251') as f:
                txt = f.read()
        except Exception as e:
            errors.append((p, str(e)))
            continue
    stripped = strip_comments(txt)
    lines = stripped.splitlines()
    total_lines += len(lines)
    try:
        with open(p, 'w', encoding='utf-8') as f:
            f.write('\n'.join(lines) + ('\n' if stripped.endswith('\n') else ''))
    except Exception as e:
        errors.append((p, str(e)))
        continue
    processed += 1

print(f'Files processed: {processed}')
print(f'Total lines after stripping comments: {total_lines}')
if errors:
    print('Errors:')
    for p, e in errors:
        print(p, e)

