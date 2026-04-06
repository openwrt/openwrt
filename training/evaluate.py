#!/usr/bin/env python3
"""
Evaluate a trained model and generate visualization plots.

Usage:
    python evaluate.py --model model/classifier.joblib --input features.csv --output plots/
"""

import argparse
import os
import sys

import joblib
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import confusion_matrix, classification_report, roc_curve, auc
from sklearn.preprocessing import label_binarize

FEATURE_NAMES = [
    "flow_duration_sec", "total_fwd_packets", "total_bwd_packets",
    "total_fwd_bytes", "total_bwd_bytes", "fwd_bwd_bytes_ratio",
    "avg_packet_size", "std_packet_size", "avg_iat_usec",
    "std_iat_usec", "min_packet_size", "max_packet_size",
    "packets_per_second", "bytes_per_second", "fwd_pkt_ratio",
    "avg_fwd_pkt_size", "avg_bwd_pkt_size", "tcp_flags_or",
    "syn_count", "protocol",
]

CLASS_NAMES = [
    "unknown", "video", "gaming", "social",
    "browsing", "download", "voip", "other",
]


def plot_confusion_matrix(y_true, y_pred, output_dir):
    present = sorted(set(y_true) | set(y_pred))
    names = [CLASS_NAMES[i] if i < len(CLASS_NAMES) else f"c{i}" for i in present]

    cm = confusion_matrix(y_true, y_pred, labels=present)
    cm_norm = cm.astype(float) / cm.sum(axis=1, keepdims=True)

    fig, axes = plt.subplots(1, 2, figsize=(16, 6))

    sns.heatmap(cm, annot=True, fmt="d", cmap="Blues",
                xticklabels=names, yticklabels=names, ax=axes[0])
    axes[0].set_title("Confusion Matrix (Counts)")
    axes[0].set_ylabel("True")
    axes[0].set_xlabel("Predicted")

    sns.heatmap(cm_norm, annot=True, fmt=".2f", cmap="Blues",
                xticklabels=names, yticklabels=names, ax=axes[1])
    axes[1].set_title("Confusion Matrix (Normalized)")
    axes[1].set_ylabel("True")
    axes[1].set_xlabel("Predicted")

    plt.tight_layout()
    path = os.path.join(output_dir, "confusion_matrix.png")
    plt.savefig(path, dpi=150)
    plt.close()
    print(f"Saved {path}")


def plot_feature_importance(model, output_dir):
    importance = model.feature_importances_
    indices = np.argsort(importance)

    fig, ax = plt.subplots(figsize=(10, 8))
    ax.barh(range(len(indices)), importance[indices], color="steelblue")
    ax.set_yticks(range(len(indices)))
    ax.set_yticklabels([FEATURE_NAMES[i] for i in indices])
    ax.set_xlabel("Feature Importance")
    ax.set_title("XGBoost Feature Importance")
    plt.tight_layout()

    path = os.path.join(output_dir, "feature_importance.png")
    plt.savefig(path, dpi=150)
    plt.close()
    print(f"Saved {path}")


def plot_class_distribution(y, output_dir):
    unique, counts = np.unique(y, return_counts=True)
    names = [CLASS_NAMES[i] if i < len(CLASS_NAMES) else f"c{i}" for i in unique]

    fig, ax = plt.subplots(figsize=(10, 6))
    colors = sns.color_palette("Set2", len(unique))
    bars = ax.bar(names, counts, color=colors)

    for bar, count in zip(bars, counts):
        ax.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 5,
                str(count), ha="center", va="bottom", fontsize=10)

    ax.set_xlabel("Traffic Class")
    ax.set_ylabel("Number of Flows")
    ax.set_title("Dataset Class Distribution")
    plt.tight_layout()

    path = os.path.join(output_dir, "class_distribution.png")
    plt.savefig(path, dpi=150)
    plt.close()
    print(f"Saved {path}")


def plot_per_class_metrics(y_true, y_pred, output_dir):
    present = sorted(set(y_true) | set(y_pred))
    names = [CLASS_NAMES[i] if i < len(CLASS_NAMES) else f"c{i}" for i in present]

    report = classification_report(y_true, y_pred, labels=present,
                                   target_names=names, output_dict=True,
                                   zero_division=0)

    metrics = ["precision", "recall", "f1-score"]
    data = {m: [report[n][m] for n in names] for m in metrics}

    x = np.arange(len(names))
    width = 0.25

    fig, ax = plt.subplots(figsize=(12, 6))
    for i, metric in enumerate(metrics):
        ax.bar(x + i * width, data[metric], width, label=metric.capitalize())

    ax.set_xticks(x + width)
    ax.set_xticklabels(names, rotation=15)
    ax.set_ylim(0, 1.1)
    ax.set_ylabel("Score")
    ax.set_title("Per-Class Precision / Recall / F1")
    ax.legend()
    plt.tight_layout()

    path = os.path.join(output_dir, "per_class_metrics.png")
    plt.savefig(path, dpi=150)
    plt.close()
    print(f"Saved {path}")


def main():
    parser = argparse.ArgumentParser(description="Evaluate and visualize model")
    parser.add_argument("--model", required=True, help="Path to classifier.joblib")
    parser.add_argument("--input", required=True, help="Input features CSV")
    parser.add_argument("--output", default="plots", help="Output directory for plots")
    args = parser.parse_args()

    os.makedirs(args.output, exist_ok=True)

    print("Loading model and data...")
    model = joblib.load(args.model)
    df = pd.read_csv(args.input)

    X = df[FEATURE_NAMES].values.astype(np.float32)
    X = np.nan_to_num(X, nan=0.0, posinf=1e10, neginf=-1e10)
    y = df["label_id"].values.astype(np.int32)

    y_pred = model.predict(X)

    print("\nGenerating plots...")
    plot_confusion_matrix(y, y_pred, args.output)
    plot_feature_importance(model, args.output)
    plot_class_distribution(y, args.output)
    plot_per_class_metrics(y, y_pred, args.output)

    print(f"\nAll plots saved to {args.output}/")


if __name__ == "__main__":
    main()
