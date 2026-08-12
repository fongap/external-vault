#!/usr/bin/env python3
"""
ADFilter build script.
Parses adfilter.txt header and generates stats.json for dynamic badges.
"""

import json
import re
import sys
from pathlib import Path
from datetime import datetime, timezone


def parse_header(filter_file: Path) -> dict:
    """Parse Adblock Plus 2.0 header for metadata."""
    stats = {
        "total_rules": 0,
        "network_filters": 0,
        "element_hiding": 0,
        "exceptions": 0,
        "version": "unknown",
        "updated": "unknown",
        "expires": "unknown",
    }

    header_pattern = re.compile(r'^!\s*(Title|Version|Expires|Count|Updated|Homepage):\s*(.+)$')
    rule_counts = {"network": 0, "elemhide": 0, "exception": 0}

    with filter_file.open('r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.rstrip('\n')

            # Header parsing
            if line.startswith('!'):
                m = header_pattern.match(line)
                if m:
                    key, val = m.group(1), m.group(2).strip()
                    if key == 'Count':
                        try:
                            stats['total_rules'] = int(val.replace(',', ''))
                        except ValueError:
                            pass
                    elif key == 'Version':
                        stats['version'] = val
                    elif key == 'Updated':
                        stats['updated'] = val
                    elif key == 'Expires':
                        stats['expires'] = val
                continue

            # Skip comments and empty lines
            if not line or line.startswith('['):
                continue

            # Rule classification
            if line.startswith('@@'):
                rule_counts['exception'] += 1
            elif '##' in line or '#@#' in line:
                rule_counts['elemhide'] += 1
            else:
                rule_counts['network'] += 1

    stats['network_filters'] = rule_counts['network']
    stats['element_hiding'] = rule_counts['elemhide']
    stats['exceptions'] = rule_counts['exception']

    # If header Count missing, compute from actual rules
    if stats['total_rules'] == 0:
        stats['total_rules'] = sum(rule_counts.values())

    return stats


def generate_stats_json(stats: dict, output_path: Path):
    """Generate stats.json for shields.io endpoint badge."""
    payload = {
        "schemaVersion": 1,
        "label": "rules",
        "message": f"{stats['total_rules']:,}",
        "color": "blue",
        "labelColor": "2d2d2d",
    }
    output_path.write_text(json.dumps(payload, separators=(',', ':')))


def generate_detailed_json(stats: dict, output_path: Path):
    """Generate detailed stats.json for human consumption."""
    detailed = {
        "generated_at": datetime.now(timezone.utc).isoformat().replace('+00:00', 'Z'),
        "source_file": "adfilter.txt",
        "version": stats['version'],
        "updated": stats['updated'],
        "expires": stats['expires'],
        "counts": {
            "total": stats['total_rules'],
            "network_filters": stats['network_filters'],
            "element_hiding": stats['element_hiding'],
            "exceptions": stats['exceptions'],
        }
    }
    output_path.write_text(json.dumps(detailed, indent=2, ensure_ascii=False))


def main():
    project_root = Path(__file__).parent
    filter_file = project_root / 'adfilter.txt'

    if not filter_file.exists():
        print(f"Error: {filter_file} not found", file=sys.stderr)
        sys.exit(1)

    print(f"Parsing {filter_file}...")
    stats = parse_header(filter_file)

    print(f"Total rules: {stats['total_rules']:,}")
    print(f"  Network filters: {stats['network_filters']:,}")
    print(f"  Element hiding: {stats['element_hiding']:,}")
    print(f"  Exceptions: {stats['exceptions']:,}")
    print(f"Version: {stats['version']}")
    print(f"Updated: {stats['updated']}")

    stats_json = project_root / 'stats.json'
    generate_stats_json(stats, stats_json)
    print(f"Generated {stats_json}")

    detailed_json = project_root / 'stats.detailed.json'
    generate_detailed_json(stats, detailed_json)
    print(f"Generated {detailed_json}")


if __name__ == '__main__':
    main()