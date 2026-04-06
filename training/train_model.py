#!/usr/bin/env python3
"""
Train an XGBoost classifier on extracted flow features.

Reads the CSV produced by extract_features.py, trains a multi-class
XGBoost model, evaluates it, and saves the model for export.

Usage:
    python train_model.py --input features.csv --output model/
    python train_model.py --input features.csv --output model/ --test-split 0.2
"""

import argparse
import json
import os
import sys

import joblib
import numpy as np
import pandas as pd
import xgboost as xgb
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import (
    classification_report,
    confusion_matrix,
    accuracy_score,
    f1_score,
)
from sklearn.preprocessing import LabelEncoder

FEATURE_NAMES = [
    "flow_duration_sec",
    "total_fwd_packets",
    "total_bwd_packets",
    "total_fwd_bytes",
    "total_bwd_bytes",
    "fwd_bwd_bytes_ratio",
    "avg_packet_size",
    "std_packet_size",
    "avg_iat_usec",
    "std_iat_usec",
    "min_packet_size",
    "max_packet_size",
    "packets_per_second",
    "bytes_per_second",
    "fwd_pkt_ratio",
    "avg_fwd_pkt_size",
    "avg_bwd_pkt_size",
    "tcp_flags_or",
    "syn_count",
    "protocol",
]

CLASS_NAMES = [
    "unknown", "video", "gaming", "social",
    "browsing", "download", "voip", "other",
]


def load_data(csv_path):
    """Load feature CSV, handle missing values, return X, y."""
    df = pd.read_csv(csv_path)

    for col in FEATURE_NAMES:
        if col not in df.columns:
            print(f"Error: missing column '{col}' in {csv_path}", file=sys.stderr)
            sys.exit(1)

    X = df[FEATURE_NAMES].values.astype(np.float32)
    X = np.nan_to_num(X, nan=0.0, posinf=1e10, neginf=-1e10)

    if "label_id" in df.columns:
        y = df["label_id"].values.astype(np.int32)
    elif "label" in df.columns:
        le = LabelEncoder()
        le.fit(CLASS_NAMES)
        y = le.transform(df["label"].values)
    else:
        print("Error: CSV must have 'label' or 'label_id' column", file=sys.stderr)
        sys.exit(1)

    return X, y, df


def train(X_train, y_train, X_test, y_test, num_classes):
    """Train XGBoost and return the model."""
    model = xgb.XGBClassifier(
        n_estimators=100,
        max_depth=6,
        learning_rate=0.1,
        objective="multi:softprob",
        num_class=num_classes,
        eval_metric="mlogloss",
        use_label_encoder=False,
        tree_method="hist",
        random_state=42,
        n_jobs=-1,
    )

    model.fit(
        X_train, y_train,
        eval_set=[(X_test, y_test)],
        verbose=True,
    )

    return model


def evaluate(model, X_test, y_test):
    """Print classification metrics."""
    y_pred = model.predict(X_test)

    acc = accuracy_score(y_test, y_pred)
    f1 = f1_score(y_test, y_pred, average="weighted", zero_division=0)

    print("\n" + "=" * 60)
    print(f"Accuracy:    {acc:.4f}")
    print(f"F1 (weighted): {f1:.4f}")
    print("=" * 60)

    present_classes = sorted(set(y_test) | set(y_pred))
    target_names = [CLASS_NAMES[i] if i < len(CLASS_NAMES) else f"class_{i}"
                    for i in present_classes]

    print("\nClassification Report:")
    print(classification_report(
        y_test, y_pred,
        labels=present_classes,
        target_names=target_names,
        zero_division=0,
    ))

    print("Confusion Matrix:")
    cm = confusion_matrix(y_test, y_pred, labels=present_classes)
    header = "        " + " ".join(f"{n:>8s}" for n in target_names)
    print(header)
    for i, row in enumerate(cm):
        row_str = " ".join(f"{v:8d}" for v in row)
        print(f"{target_names[i]:>8s} {row_str}")

    return {"accuracy": acc, "f1_weighted": f1}


def print_feature_importance(model):
    """Print top features by importance."""
    importance = model.feature_importances_
    indices = np.argsort(importance)[::-1]

    print("\nFeature Importance (top 10):")
    print("-" * 40)
    for i in range(min(10, len(indices))):
        idx = indices[i]
        print(f"  {i+1:2d}. {FEATURE_NAMES[idx]:<25s} {importance[idx]:.4f}")


def save_model(model, output_dir, metrics):
    """Save XGBoost model in multiple formats."""
    os.makedirs(output_dir, exist_ok=True)

    xgb_path = os.path.join(output_dir, "classifier.xgb")
    model.save_model(xgb_path)
    print(f"\nSaved XGBoost model: {xgb_path}")

    json_path = os.path.join(output_dir, "classifier.json")
    model.save_model(json_path)
    print(f"Saved JSON model:   {json_path}")

    joblib_path = os.path.join(output_dir, "classifier.joblib")
    joblib.dump(model, joblib_path)
    print(f"Saved joblib model: {joblib_path}")

    meta = {
        "model_type": "xgboost",
        "version": "0.1.0",
        "n_estimators": model.n_estimators,
        "max_depth": model.max_depth,
        "num_features": len(FEATURE_NAMES),
        "num_classes": len(CLASS_NAMES),
        "features": FEATURE_NAMES,
        "classes": CLASS_NAMES,
        "metrics": metrics,
    }
    meta_path = os.path.join(output_dir, "model_meta.json")
    with open(meta_path, "w") as f:
        json.dump(meta, f, indent=2)
    print(f"Saved metadata:     {meta_path}")


def main():
    parser = argparse.ArgumentParser(description="Train XGBoost traffic classifier")
    parser.add_argument("--input", required=True, help="Input CSV from extract_features.py")
    parser.add_argument("--output", default="model", help="Output directory (default: model/)")
    parser.add_argument("--test-split", type=float, default=0.2, help="Test set fraction (default: 0.2)")
    parser.add_argument("--cv", type=int, default=0, help="Cross-validation folds (0 = skip)")
    args = parser.parse_args()

    print(f"Loading data from {args.input}...")
    X, y, df = load_data(args.input)
    print(f"  Total flows: {len(X)}")
    print(f"  Features:    {X.shape[1]}")

    unique, counts = np.unique(y, return_counts=True)
    print("\n  Class distribution:")
    for cls_id, count in zip(unique, counts):
        name = CLASS_NAMES[cls_id] if cls_id < len(CLASS_NAMES) else f"class_{cls_id}"
        print(f"    {name:>10s}: {count:6d} ({100*count/len(y):.1f}%)")

    num_classes = max(len(CLASS_NAMES), int(unique.max()) + 1)

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=args.test_split, random_state=42, stratify=y
    )
    print(f"\n  Train: {len(X_train)}, Test: {len(X_test)}")

    if args.cv > 0:
        print(f"\nRunning {args.cv}-fold cross-validation...")
        temp_model = xgb.XGBClassifier(
            n_estimators=100, max_depth=6, learning_rate=0.1,
            objective="multi:softprob", num_class=num_classes,
            use_label_encoder=False, tree_method="hist",
            random_state=42, n_jobs=-1,
        )
        cv_scores = cross_val_score(temp_model, X, y, cv=args.cv, scoring="accuracy")
        print(f"  CV Accuracy: {cv_scores.mean():.4f} (+/- {cv_scores.std()*2:.4f})")

    print("\nTraining XGBoost...")
    model = train(X_train, y_train, X_test, y_test, num_classes)

    metrics = evaluate(model, X_test, y_test)
    print_feature_importance(model)

    save_model(model, args.output, metrics)

    print("\nDone! Next steps:")
    print(f"  1. Review metrics above")
    print(f"  2. Export to C:  python export_treelite.py --model {args.output}/classifier.xgb")


if __name__ == "__main__":
    main()
