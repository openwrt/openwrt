#!/usr/bin/env python3
"""
Export a trained XGBoost model to C code via treelite.

The generated C code can be compiled directly into the traffic-classifier
daemon — no ONNX runtime or Python needed on the target device.

Usage:
    python export_treelite.py --model model/classifier.xgb --output ../package/network/services/traffic-classifier/src/
"""

import argparse
import json
import os
import sys
import textwrap

import treelite
import xgboost as xgb


CLASS_NAMES = [
    "unknown", "video", "gaming", "social",
    "browsing", "download", "voip", "other",
]


def export_treelite_c(model_path, output_dir):
    """Load XGBoost model, compile to C via treelite."""
    print(f"Loading model from {model_path}...")
    tl_model = treelite.Model.from_xgboost(xgb.Booster(model_file=model_path))

    os.makedirs(output_dir, exist_ok=True)

    toolchain = "gcc"
    print(f"Compiling model to C (toolchain: {toolchain})...")

    treelite.compile(
        tl_model,
        dirpath=output_dir,
        params={"parallel_comp": 1},
        compiler="ast_native",
        verbose=True,
    )

    print(f"\nTreelite C code written to {output_dir}")
    print("Files generated:")
    for f in sorted(os.listdir(output_dir)):
        fpath = os.path.join(output_dir, f)
        size = os.path.getsize(fpath)
        print(f"  {f} ({size:,} bytes)")


def export_manual_c(model_path, output_path):
    """
    Fallback: export XGBoost model as a hand-written C decision tree.
    Works when treelite compilation is not available or too complex.
    Generates a single .c file with predict() function.
    """
    print(f"Loading model from {model_path}...")
    booster = xgb.Booster(model_file=model_path)

    dump = booster.get_dump(dump_format="json")
    num_trees = len(dump)

    config = json.loads(booster.save_config())
    num_classes = int(config.get("learner", {}).get(
        "learner_model_param", {}).get("num_class", 8))

    print(f"  Trees: {num_trees}")
    print(f"  Classes: {num_classes}")
    print(f"  Trees per class: {num_trees // num_classes}")

    lines = []
    lines.append('#include "model_predict.h"')
    lines.append("#include <math.h>")
    lines.append("#include <float.h>")
    lines.append("")

    for tree_idx, tree_json_str in enumerate(dump):
        tree = json.loads(tree_json_str)
        lines.append(f"static float tree_{tree_idx}(const float *f)")
        lines.append("{")
        _emit_tree_node(tree, lines, depth=1)
        lines.append("}")
        lines.append("")

    lines.append(f"void model_predict(const float *features, int num_features,")
    lines.append(f"                   int *out_class, float *out_confidence)")
    lines.append("{")
    lines.append(f"    float scores[{num_classes}] = {{0}};")
    lines.append("")

    for tree_idx in range(num_trees):
        class_idx = tree_idx % num_classes
        lines.append(f"    scores[{class_idx}] += tree_{tree_idx}(features);")

    lines.append("")
    lines.append("    /* softmax */")
    lines.append(f"    float max_score = scores[0];")
    lines.append(f"    for (int i = 1; i < {num_classes}; i++)")
    lines.append("        if (scores[i] > max_score) max_score = scores[i];")
    lines.append("")
    lines.append("    float sum_exp = 0;")
    lines.append(f"    for (int i = 0; i < {num_classes}; i++) {{")
    lines.append("        scores[i] = expf(scores[i] - max_score);")
    lines.append("        sum_exp += scores[i];")
    lines.append("    }")
    lines.append("")
    lines.append("    int best = 0;")
    lines.append("    float best_prob = 0;")
    lines.append(f"    for (int i = 0; i < {num_classes}; i++) {{")
    lines.append("        scores[i] /= sum_exp;")
    lines.append("        if (scores[i] > best_prob) {")
    lines.append("            best_prob = scores[i];")
    lines.append("            best = i;")
    lines.append("        }")
    lines.append("    }")
    lines.append("")
    lines.append("    *out_class = best;")
    lines.append("    *out_confidence = best_prob;")
    lines.append("}")
    lines.append("")

    with open(output_path, "w") as f:
        f.write("\n".join(lines))

    header_path = output_path.replace(".c", ".h")
    with open(header_path, "w") as f:
        f.write("#ifndef MODEL_PREDICT_H\n")
        f.write("#define MODEL_PREDICT_H\n\n")
        f.write("void model_predict(const float *features, int num_features,\n")
        f.write("                   int *out_class, float *out_confidence);\n\n")
        f.write("#endif\n")

    c_size = os.path.getsize(output_path)
    print(f"\nGenerated {output_path} ({c_size:,} bytes, {num_trees} trees)")
    print(f"Generated {header_path}")


def _emit_tree_node(node, lines, depth):
    """Recursively emit C code for a single tree node."""
    indent = "    " * depth

    if "leaf" in node:
        lines.append(f"{indent}return {node['leaf']:.8f}f;")
        return

    feature_idx = node["split"]
    threshold = node["split_condition"]
    yes_child = node.get("yes", 0)
    no_child = node.get("no", 0)

    children = {child["nodeid"]: child for child in node.get("children", [])}

    lines.append(f"{indent}if (f[{feature_idx}] < {threshold:.8f}f) {{")
    if yes_child in children:
        _emit_tree_node(children[yes_child], lines, depth + 1)
    else:
        lines.append(f"{indent}    return 0.0f;")
    lines.append(f"{indent}}} else {{")
    if no_child in children:
        _emit_tree_node(children[no_child], lines, depth + 1)
    else:
        lines.append(f"{indent}    return 0.0f;")
    lines.append(f"{indent}}}")


def main():
    parser = argparse.ArgumentParser(
        description="Export XGBoost model to C code for embedded inference"
    )
    parser.add_argument("--model", required=True, help="Path to classifier.xgb")
    parser.add_argument(
        "--output", default="model_c/",
        help="Output directory for C files (default: model_c/)"
    )
    parser.add_argument(
        "--method", choices=["treelite", "manual"], default="manual",
        help="Export method: 'treelite' (library) or 'manual' (self-contained C). "
             "Default: manual"
    )
    args = parser.parse_args()

    if not os.path.exists(args.model):
        print(f"Error: model file not found: {args.model}", file=sys.stderr)
        sys.exit(1)

    if args.method == "treelite":
        export_treelite_c(args.model, args.output)
    else:
        os.makedirs(args.output, exist_ok=True)
        output_c = os.path.join(args.output, "model_predict.c")
        export_manual_c(args.model, output_c)

    print("\nNext steps:")
    print("  1. Copy generated .c/.h files to the traffic-classifier src/ directory")
    print("  2. Update classifier.c to call model_predict() instead of heuristics")
    print("  3. Rebuild the OpenWRT package")


if __name__ == "__main__":
    main()
