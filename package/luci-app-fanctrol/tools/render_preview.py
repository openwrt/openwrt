#!/usr/bin/env python3
"""Render the LuCI template as a self-contained local responsive preview."""

from __future__ import annotations

import ast
import re
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT.parent / "_responsive_preview"


def translations() -> dict[str, str]:
    result: dict[str, str] = {}
    source = (ROOT / "i18n-src/fancontrol.zh-cn.po").read_text(encoding="utf-8")
    for block in re.split(r"\n\s*\n", source):
        msgid = re.search(r'^msgid\s+(".*")$', block, re.MULTILINE)
        msgstr = re.search(r'^msgstr\s+(".*")$', block, re.MULTILINE)
        if msgid and msgstr:
            result[ast.literal_eval(msgid.group(1))] = ast.literal_eval(msgstr.group(1))
    return result


def render_view() -> str:
    mapping = translations()
    view = (ROOT / "luasrc/view/fancontrol/main.htm").read_text(encoding="utf-8")
    view = view.replace("<%+header%>", "").replace("<%+footer%>", "")
    view = re.sub(r'<link rel="stylesheet"[^>]+fancontrol\.css[^>]*>\s*', "", view, count=1)
    view = re.sub(r"<%:(.*?)%>", lambda match: mapping.get(match.group(1), match.group(1)), view)
    if "<%" in view:
        remaining = re.findall(r"<%.*?%>", view)
        raise RuntimeError(f"unexpanded LuCI template expression remains: {remaining}")
    return view


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(
        ROOT / "htdocs/luci-static/resources/view/fancontrol.css",
        OUTPUT / "fancontrol.css",
    )
    view = render_view()
    html = f"""<!doctype html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>风扇控制响应式预览</title>
<link rel="stylesheet" href="fancontrol.css">
<style>
html {{ background: #f2f5f8; }}
body {{ margin: 0; color: #172033; background: #f2f5f8; font-family: "Segoe UI", "Microsoft YaHei", sans-serif; }}
.preview-sidebar {{ position: fixed; inset: 0 auto 0 0; width: 240px; padding: 28px 20px; color: #fff; background: linear-gradient(180deg, #263b80, #4466d9); box-sizing: border-box; }}
.preview-sidebar strong {{ display: block; font-size: 22px; margin-bottom: 28px; }}
.preview-sidebar span {{ display: block; margin: 12px 0; opacity: .86; }}
.preview-sidebar button {{ min-height: 44px; margin-top: 24px; padding: 8px 12px; color: #263b80; border: 0; border-radius: 7px; background: #fff; cursor: pointer; }}
.preview-main {{ min-width: 0; margin-left: 240px; padding: 22px; }}
.sidebar-collapsed .preview-sidebar {{ width: 60px; padding-inline: 8px; }}
.sidebar-collapsed .preview-sidebar strong,
.sidebar-collapsed .preview-sidebar span {{ display: none; }}
.sidebar-collapsed .preview-sidebar button {{ width: 44px; padding: 4px; font-size: 0; }}
.sidebar-collapsed .preview-sidebar button::after {{ content: "☰"; font-size: 22px; }}
.sidebar-collapsed .preview-main {{ margin-left: 60px; }}
.preview-main > h2 {{ margin: 0; padding: 20px; border-radius: 8px; background: #fff; box-shadow: 0 6px 20px rgba(45,69,102,.08); }}
.preview-main > h2 a {{ color: #1d315f; text-decoration: none; }}
.cbi-map-descr {{ padding: 18px 20px 4px; color: #314267; }}
@media (max-width: 900px) {{
  .preview-sidebar {{ display: none; }}
  .preview-main {{ margin-left: 0; padding: 12px; }}
}}
@media (max-width: 500px) {{
  .preview-main {{ padding: 6px; }}
  .preview-main > h2 {{ padding: 14px; font-size: 20px; }}
  .cbi-map-descr {{ padding: 12px 14px 2px; font-size: 13px; }}
}}
</style>
<script>
(function () {{
  var curves = {{
    quiet: '20:20,25:20,30:20,35:20,40:20,45:20,50:20,55:20,60:20,65:20,70:20,75:20,80:20,85:20,90:20,95:20,100:20,105:20,110:20',
    balanced: '20:20,25:20,30:20,35:20,40:20,45:40,50:59,55:79,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100',
    performance: '20:100,25:100,30:100,35:100,40:100,45:100,50:100,55:100,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
  }};
  window.fetch = function (input, options) {{
    var url = new URL(String(input), window.location.href);
    var action = url.searchParams.get('act');
    var client = url.searchParams.get('client') || '';
    var data = {{ result: 'ok' }};
    if (action === 'claim_ui') {{
      localStorage.setItem('fancontrolPreviewOwner', client);
    }} else if (!client || localStorage.getItem('fancontrolPreviewOwner') !== client) {{
      data = {{ result: 'taken_over' }};
    }} else if (action === 'get_config') {{
      data = {{ enable: '1', mode: 1, temp_sources: 'cpu', curve_silent: curves.quiet, curve_balanced: curves.balanced, curve_performance: curves.performance }};
    }} else if (action === 'get_curve') {{
      data = {{ curve: curves.balanced }};
    }} else if (action === 'get_status') {{
      data = {{
        temp: '53.3', speed: 78, rpm: '--', control_source: 'cpu',
        sources: [
          {{ id: 'cpu', state: 'ok', temp: 53.3 }},
          {{ id: 'phy', state: 'ok', temp: 30.0 }},
          {{ id: 'nvme1', state: 'missing', temp: null }},
          {{ id: 'nvme2', state: 'missing', temp: null }}
        ]
      }};
    }} else if (action === 'set_config') {{
      var configBody = options && options.body;
      data = {{ result: 'ok', temp_sources: configBody && configBody.get ? configBody.get('temp_sources') : 'cpu' }};
    }} else if (action === 'set_curve') {{
      var body = options && options.body;
      var preset = body && body.get ? body.get('preset') : null;
      var curve = body && body.get ? body.get('curve') : curves.balanced;
      if (preset) curves[preset] = curve;
      data = {{ result: 'ok', curve: curve, mode: preset === 'quiet' ? 0 : (preset === 'performance' ? 2 : 1) }};
    }}
    return Promise.resolve({{ ok: true, json: function () {{ return Promise.resolve(data); }} }});
  }};
  document.addEventListener('DOMContentLoaded', function () {{
    document.getElementById('toggleSidebar').addEventListener('click', function () {{
      document.body.classList.toggle('sidebar-collapsed');
    }});
  }});
}})();
</script>
</head>
<body>
<aside class="preview-sidebar"><strong>OpenWrt</strong><span>状态</span><span>系统</span><span>服务</span><span>网络</span><button id="toggleSidebar" type="button" aria-label="切换侧栏">切换侧栏</button></aside>
<main class="preview-main">
{view}
</main>
</body>
</html>
"""
    (OUTPUT / "index.html").write_text(html, encoding="utf-8", newline="\n")
    print(OUTPUT / "index.html")


if __name__ == "__main__":
    main()
