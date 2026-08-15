'use strict';
'require dom';
'require fs';
'require uci';
'require ui';
'require view';

var HELPER = '/usr/sbin/display-control';
var FRAME_REFRESH_MS = 100;

var SCREENS = [
	{ id: 1, title: _('屏幕 1 - 网络状态界面') },
	{ id: 2, title: _('屏幕 2 - 时间时钟界面') },
	{ id: 3, title: _('屏幕 3 - 流量统计界面') },
	{ id: 4, title: _('屏幕 4 - 硬件监控界面') }
];

return view.extend({
	load: function() {
		return Promise.all([
			L.resolveDefault(fs.exec(HELPER, [ 'status' ]), {}),
			uci.load('display')
		]);
	},

	addStyle: function() {
		if (document.getElementById('display-control-style'))
			return;

		document.head.appendChild(E('style', { id: 'display-control-style' }, [ `
			/* E87N_DESIRED_HEADER_V4: full white title card over the blue page band */
			.display-page-head {
				--display-track-bg: #d8e0eb;
				--display-line: rgba(115, 132, 158, .28);
				--display-thumb-bg: #ffffff;
				--display-blue: #2d86ff;
				position: relative;
				display: flex;
				align-items: center;
				justify-content: space-between;
				gap: 16px;
				min-height: 70px;
				margin: 0 -12px;
				padding: 10px 28px 14px 30px;
				background: linear-gradient(to bottom, #6074e5 0, #6074e5 34px, transparent 34px);
				overflow: visible;
			}

			.display-page-head::before {
				position: absolute;
				inset: 10px 12px 14px;
				background: #ffffff;
				border-radius: 3px;
				box-shadow: 0 2px 7px rgba(15, 23, 42, .12);
				content: '';
			}

			.display-page-head h2 {
				position: relative;
				z-index: 1;
				display: flex;
				align-items: center;
				min-height: 46px;
				margin: 0 !important;
				padding: 8px 0;
				border-radius: 0;
				background: transparent;
				box-shadow: none;
				color: #172554;
				font-weight: 700;
			}

			.display-master-switch {
				position: relative;
				z-index: 1;
				flex: 0 0 auto;
				margin: 0 0 0 auto;
			}

			.display-app {
				--display-bg: #07111f;
				--display-panel: #101d2f;
				--display-panel-2: #142238;
				--display-line: rgba(159, 177, 205, .18);
				--display-text: #e8edf7;
				--display-muted: #9aa8bd;
				--display-blue: #2d86ff;
				--display-cyan: #18d3c9;
				--display-green: #43d17f;
				--display-orange: #ff9a2f;
				--display-card-bg: linear-gradient(180deg, rgba(20, 34, 56, .94), rgba(13, 27, 46, .94));
				--display-card-shadow: 0 16px 42px rgba(0, 0, 0, .22), inset 0 1px 0 rgba(255, 255, 255, .04);
				--display-field-bg: rgba(3, 11, 24, .42);
				--display-soft-bg: rgba(255, 255, 255, .035);
				--display-track-bg: rgba(107, 127, 153, .32);
				--display-thumb-bg: #f5f8ff;
				--display-screen-bg: rgba(7, 17, 31, .62);
				--display-clock: #cbd6e7;
				--display-time-bg: rgba(30, 49, 76, .92);
				--display-time-border: rgba(75, 112, 158, .58);
				--display-time-menu-bg: #08243d;
				--display-time-menu-border: rgba(45, 134, 255, .38);
				--display-time-hover: rgba(45, 134, 255, .22);
				--display-time-active: #2d86ff;
				color: var(--display-text);
				background:
					radial-gradient(circle at 24% 0%, rgba(45, 134, 255, .16), transparent 32rem),
					linear-gradient(135deg, #07111f 0%, #0b1628 48%, #101729 100%);
				border-radius: 8px;
				padding: 12px;
				min-height: 0;
				min-width: 0;
				max-width: 100%;
			}

			.display-app * {
				box-sizing: border-box;
			}

			.display-grid {
				display: grid;
				grid-template-columns: minmax(0, 1.35fr) minmax(300px, .85fr);
				gap: 10px;
				align-items: start;
			}

			.display-left {
				display: grid;
				gap: 10px;
				container-type: inline-size;
				container-name: display-controls;
			}

			.display-two {
				display: grid;
				grid-template-columns: minmax(0, .85fr) minmax(0, 1.15fr);
				gap: 10px;
			}

			.display-card,
			.display-select {
				background: var(--display-card-bg);
				border: 1px solid var(--display-line);
				border-radius: 8px;
				box-shadow: var(--display-card-shadow);
			}

			.display-card {
				padding: 12px;
			}

			.display-head,
			.display-title,
			.display-row,
			.display-switch-label,
			.display-preview-foot,
			.display-stat {
				display: flex;
				align-items: center;
			}

			.display-head {
				justify-content: space-between;
				gap: 10px;
				margin-bottom: 10px;
			}

			.display-title {
				gap: 8px;
				font-size: 17px;
				font-weight: 700;
				color: var(--display-text);
			}

			.display-icon {
				width: 22px;
				height: 22px;
				color: #cbd6e7;
				flex: 0 0 auto;
			}

			.display-icon svg {
				display: block;
				width: 100%;
				height: 100%;
			}

			.display-mobile-percent {
				display: none;
			}

			.display-mobile-brightness-line {
				display: contents;
			}

			.display-row {
				gap: 10px;
				min-height: 34px;
			}

			.display-label {
				color: var(--display-muted);
				font-size: 14px;
				white-space: nowrap;
			}

			.display-percent {
				color: var(--display-blue);
				font-size: 24px;
				font-weight: 800;
				min-width: 68px;
			}

			.display-slider {
				-webkit-appearance: none !important;
				appearance: none;
				flex: 1 1 auto;
				width: 100%;
				height: 24px;
				margin: 0;
				padding: 0 !important;
				border: 0 !important;
				border-radius: 0 !important;
				background: transparent !important;
				box-shadow: none !important;
				outline: none !important;
				position: relative;
				z-index: 2;
			}

			.display-slider::-webkit-slider-runnable-track {
				height: 24px;
				border: 0 !important;
				background: transparent !important;
				box-shadow: none !important;
			}

			.display-slider::-webkit-slider-thumb {
				-webkit-appearance: none !important;
				appearance: none !important;
				width: 18px;
				height: 18px;
				margin-top: 3px;
				border: 0;
				border-radius: 50%;
				background: var(--display-thumb-bg);
				box-shadow: 0 3px 10px rgba(0, 0, 0, .38), 0 0 0 1px rgba(255, 255, 255, .72);
				cursor: pointer;
				position: relative;
				z-index: 3;
			}

			.display-slider::-moz-range-track {
				height: 24px;
				border: 0 !important;
				background: transparent !important;
				box-shadow: none !important;
			}

			.display-slider::-moz-range-progress {
				height: 0;
				background: transparent;
			}

			.display-slider::-moz-range-thumb {
				width: 18px;
				height: 18px;
				border: 0;
				border-radius: 50%;
				background: var(--display-thumb-bg);
				box-shadow: 0 3px 10px rgba(0, 0, 0, .38), 0 0 0 1px rgba(255, 255, 255, .72);
				cursor: pointer;
			}

			.display-range {
				--value: 72%;
				--thumb: 18px;
				--track: 8px;
				position: relative;
				flex: 1 1 auto;
				height: 24px;
				min-width: 120px;
			}

			.display-range:before,
			.display-range:after {
				content: "";
				position: absolute;
				left: calc(var(--thumb) / 2);
				right: calc(var(--thumb) / 2);
				top: 50%;
				height: var(--track);
				border-radius: 999px;
				transform: translateY(-50%);
			}

			.display-range:before {
				background: var(--display-track-bg);
				box-shadow: none;
			}

			.display-range:after {
				width: calc((100% - var(--thumb)) * var(--value-num, .72));
				right: auto;
				background: var(--display-blue);
				box-shadow: none;
			}

			.display-switch-label {
				gap: 10px;
				color: var(--display-text);
				font-weight: 600;
				font-size: 14px;
				white-space: nowrap;
			}

			.display-switch-text {
				display: inline-flex;
				align-items: center;
				gap: 7px;
				min-width: 0;
				color: var(--display-text);
				font-weight: 700;
				font-size: 14px;
				white-space: nowrap;
			}

			.display-switch input {
				position: absolute;
				opacity: 0;
				pointer-events: none;
			}

			.display-switch span {
				display: inline-block;
				width: 40px;
				height: 22px;
				border-radius: 999px;
				background: var(--display-track-bg);
				border: 1px solid var(--display-line);
				position: relative;
				vertical-align: middle;
				transition: background .2s ease;
			}

			.display-switch span:before {
				content: "";
				position: absolute;
				width: 16px;
				height: 16px;
				left: 3px;
				top: 2px;
				border-radius: 50%;
				background: var(--display-thumb-bg);
				box-shadow: 0 2px 8px rgba(0, 0, 0, .35);
				transition: transform .2s ease;
			}

			.display-switch input:checked + span {
				background: var(--display-blue);
			}

			.display-switch input:checked + span:before {
				transform: translateX(17px);
			}

			.display-time-grid {
				display: grid;
				gap: 6px;
			}

			.display-time-row {
				display: block;
			}

			.display-power-row,
			.display-schedule-row {
				display: grid;
				gap: 8px;
				align-items: center;
				background: var(--display-soft-bg);
				border: 1px solid var(--display-line);
				border-radius: 8px;
				min-height: 42px;
				padding: 5px 8px;
			}

			.display-power-row {
				grid-template-columns: minmax(92px, .9fr) minmax(112px, .72fr) auto;
			}

			.display-power-row .display-icon {
				width: 18px;
				height: 18px;
				color: var(--display-clock);
			}

			.display-time-trigger {
				-webkit-appearance: none;
				appearance: none;
				width: 100%;
				height: 30px;
				border-radius: 8px;
				border: 1px solid var(--display-time-border) !important;
				background: var(--display-time-bg) !important;
				color: var(--display-text);
				display: flex;
				align-items: center;
				justify-content: space-between;
				gap: 10px;
				padding: 0 9px 0 11px;
				font-size: 14px;
				font-weight: 700;
				line-height: 1;
				cursor: pointer;
				box-shadow: none !important;
				outline: none;
				transition: background .15s ease, border-color .15s ease, box-shadow .15s ease;
			}

			.display-time-trigger:hover,
			.display-time-field.is-open .display-time-trigger {
				border-color: rgba(45, 134, 255, .72) !important;
				background: rgba(45, 134, 255, .18) !important;
				box-shadow: 0 0 0 2px rgba(45, 134, 255, .12) !important;
			}

			.display-time-field {
				display: block;
				position: relative;
				width: 100%;
			}

			.display-time-clock {
				width: 18px;
				height: 18px;
				color: var(--display-clock);
				pointer-events: none;
				flex: 0 0 auto;
			}

			.display-time-clock .display-icon {
				width: 18px;
				height: 18px;
				color: currentColor;
			}

			.display-time-picker {
				position: absolute;
				left: 0;
				top: calc(100% + 6px);
				z-index: 80;
				display: none;
				grid-template-columns: repeat(2, 54px);
				gap: 6px;
				width: 126px;
				padding: 7px;
				border: 1px solid var(--display-time-menu-border);
				border-radius: 10px;
				background: var(--display-time-menu-bg);
				box-shadow: 0 18px 42px rgba(0, 0, 0, .4), 0 0 0 1px rgba(255, 255, 255, .035) inset;
			}

			.display-time-field.is-open {
				z-index: 90;
			}

			.display-time-field.is-open .display-time-picker {
				display: grid;
			}

			.display-time-column {
				display: grid;
				gap: 3px;
				overflow: hidden;
				padding: 0;
			}

			.display-time-option {
				-webkit-appearance: none;
				appearance: none;
				display: flex;
				align-items: center;
				justify-content: center;
				height: 28px;
				border: 0 !important;
				border-radius: 7px;
				background: transparent !important;
				color: var(--display-text);
				font-size: 14px;
				font-weight: 800;
				line-height: 1;
				padding: 0;
				text-align: center;
				cursor: pointer;
				outline: none;
				box-shadow: none !important;
				transition: background .14s ease, color .14s ease, transform .14s ease;
			}

			.display-time-option:hover {
				background: var(--display-time-hover) !important;
				color: #ffffff;
			}

			.display-time-option.is-selected {
				background: var(--display-time-active) !important;
				color: #ffffff;
				box-shadow: 0 5px 14px rgba(45, 134, 255, .28) !important;
			}

			.display-schedule-row {
				grid-template-columns: minmax(72px, .7fr) minmax(112px, .8fr) minmax(96px, 1fr) 44px;
			}

			.display-compact-card {
				padding: 10px;
			}

			.display-compact-card .display-head {
				margin-bottom: 6px;
			}

			.display-schedule-name,
			.display-switch-text {
				color: var(--display-text);
				font-weight: 700;
				font-size: 14px;
				white-space: nowrap;
			}

			.display-schedule-percent {
				text-align: right;
				color: var(--display-blue);
				font-weight: 800;
				font-size: 14px;
			}

			.display-preview {
				min-height: 0;
				align-self: start;
				padding: 10px;
			}

			.display-preview .display-head {
				margin-bottom: 6px;
			}

			.display-preview-frame {
				/* Match the E87N NV3007 framebuffer exactly. The panel is
				 * 428 x 142 after the DTS rotation, so the LuCI preview must
				 * not keep the aspect ratio of the old design mock-up. */
				aspect-ratio: 428 / 142;
				width: 100%;
				border-radius: 8px;
				border: 1px solid rgba(99, 127, 176, .45);
				background: #050b15;
				overflow: hidden;
			}

			.display-preview-frame img,
			.display-preview-frame canvas,
			.display-screen-card img {
				display: block;
				width: 100%;
				height: 100%;
				object-fit: fill;
			}

			.display-preview-foot {
				justify-content: center;
				gap: 8px;
				margin-top: 6px;
				color: var(--display-muted);
				font-weight: 700;
				font-size: 13px;
			}

			.display-preview-foot strong {
				color: var(--display-blue);
			}

			.display-toast {
				position: fixed;
				right: 28px;
				top: 58px;
				z-index: 10000;
				max-width: min(420px, calc(100vw - 32px));
				padding: 18px 28px;
				border: 1px solid rgba(24, 211, 201, .85);
				border-radius: 8px;
				background: rgba(4, 22, 38, .94);
				box-shadow: 0 18px 48px rgba(0, 0, 0, .35);
				color: #eef7ff;
				font-size: 18px;
				font-weight: 700;
				line-height: 1.45;
				opacity: 0;
				transform: translateY(-10px);
				pointer-events: none;
				transition: opacity .18s ease, transform .18s ease;
			}

			.display-toast.is-visible {
				opacity: 1;
				transform: translateY(0);
			}

			.display-screen-card:hover {
				transform: translateY(-1px);
			}

			.display-select {
				margin-top: 10px;
				padding: 12px;
			}

			.display-section-title {
				margin: 0 0 10px;
				font-size: 17px;
				font-weight: 700;
				color: var(--display-text);
			}

			.display-screens {
				display: grid;
				grid-template-columns: repeat(4, minmax(0, 1fr));
				gap: 10px;
				align-items: stretch;
			}

			.display-screen-card {
				position: relative;
				display: grid;
				grid-template-rows: minmax(18px, auto) auto;
				gap: 8px;
				border: 1px solid rgba(151, 170, 202, .26);
				border-radius: 8px;
				background: var(--display-screen-bg);
				padding: 8px;
				color: var(--display-text);
				cursor: pointer;
				text-align: left;
				min-width: 0;
				width: 100%;
				min-height: 0;
				overflow: hidden;
				transition: transform .15s ease, border-color .15s ease, box-shadow .15s ease;
			}

			.display-screen-card.is-active {
				border-color: var(--display-blue);
				box-shadow: 0 0 0 1px rgba(45, 134, 255, .58), 0 12px 30px rgba(45, 134, 255, .14);
			}

			.display-screen-card-title {
				font-size: 13px;
				font-weight: 700;
				line-height: 1.35;
				min-height: 18px;
				padding-right: 26px;
			}

			.display-radio {
				position: absolute;
				top: 8px;
				right: 8px;
				width: 20px;
				height: 20px;
				border-radius: 50%;
				border: 2px solid rgba(170, 184, 205, .48);
				background: rgba(3, 11, 24, .5);
			}

			.display-screen-card.is-active .display-radio {
				border-color: var(--display-blue);
				background: var(--display-blue);
			}

			.display-screen-card.is-active .display-radio:after {
				content: "";
				position: absolute;
				left: 5px;
				top: 2px;
				width: 6px;
				height: 10px;
				border: solid #fff;
				border-width: 0 2px 2px 0;
				transform: rotate(45deg);
			}

			.display-screen-image {
				display: block;
				width: 100%;
				aspect-ratio: 428 / 142;
				overflow: hidden;
				border-radius: 6px;
				background: #050b15;
				line-height: 0;
			}

			.display-stats {
				display: grid;
				grid-template-columns: repeat(2, minmax(0, 1fr));
				gap: 8px;
				margin-top: 10px;
			}

			.display-stat {
				justify-content: space-between;
				gap: 10px;
				border: 1px solid var(--display-line);
				border-radius: 8px;
				background: var(--display-soft-bg);
				padding: 8px 10px;
				color: var(--display-muted);
				font-size: 13px;
			}

			.display-stat strong {
				color: var(--display-text);
				font-size: 14px;
			}

			.display-page ~ .cbi-page-actions,
			#maincontent > .cbi-page-actions,
			body .cbi-page-actions {
				display: none !important;
			}

			@media (prefers-color-scheme: light) {
				.display-app {
					--display-bg: #f5f8fc;
					--display-panel: #ffffff;
					--display-panel-2: #f8fafc;
					--display-line: #dfe6ef;
					--display-text: #1f2937;
					--display-muted: #697586;
					--display-blue: #197cff;
					--display-cyan: #10b8b0;
					--display-green: #25b56b;
					--display-orange: #f59e0b;
					--display-card-bg: linear-gradient(180deg, #ffffff 0%, #fbfdff 100%);
					--display-card-shadow: 0 10px 26px rgba(15, 23, 42, .08), inset 0 1px 0 rgba(255, 255, 255, .92);
					--display-field-bg: #f8fafc;
					--display-soft-bg: #ffffff;
					--display-track-bg: #d8dee8;
					--display-thumb-bg: #ffffff;
					--display-screen-bg: #ffffff;
					--display-clock: #667085;
					--display-time-bg: #f2f7fd;
					--display-time-border: #bcd2ec;
					--display-time-menu-bg: #edf6ff;
					--display-time-menu-border: #9cc8ff;
					--display-time-hover: #e8f2ff;
					--display-time-active: #197cff;
					background: linear-gradient(180deg, #f8fbff 0%, #f2f6fb 100%);
				}

				.display-card,
				.display-select {
					border-color: var(--display-line);
				}

				.display-icon {
					color: #667085;
				}

				.display-time-row .display-switch-label,
				.display-time-trigger,
				.display-schedule-row,
				.display-stat,
				.display-screen-card {
					box-shadow: 0 1px 2px rgba(15, 23, 42, .04);
				}

				.display-time-trigger:hover,
				.display-time-field.is-open .display-time-trigger {
					border-color: rgba(25, 124, 255, .58) !important;
					background: #e8f2ff !important;
					box-shadow: 0 0 0 2px rgba(25, 124, 255, .1) !important;
				}

				.display-time-picker {
					box-shadow: 0 18px 38px rgba(15, 23, 42, .16), 0 0 0 1px rgba(255, 255, 255, .65) inset;
				}

				.display-time-option:hover {
					color: #1f2937;
				}

				.display-switch span {
					background: #e6ebf2;
					border-color: #d3dbe7;
				}

				.display-switch input:checked + span {
					background: var(--display-blue);
				}

				.display-radio {
					background: #ffffff;
					border-color: #b6c0ce;
				}

				.display-screen-card.is-active {
					border-color: var(--display-blue);
					box-shadow: 0 0 0 1px rgba(25, 124, 255, .62), 0 12px 24px rgba(25, 124, 255, .12);
				}

				.display-toast {
					background: rgba(255, 255, 255, .96);
					box-shadow: 0 18px 42px rgba(15, 23, 42, .14);
					color: #1f2937;
				}
			}

			@media (max-width: 1480px) {
				.display-grid {
					grid-template-columns: minmax(0, 1.2fr) minmax(280px, .8fr);
				}

				.display-schedule-row {
					grid-template-columns: minmax(68px, .65fr) minmax(104px, .78fr) minmax(88px, 1fr) 42px;
				}
			}

			@media (max-width: 1180px) {
				.display-grid {
					grid-template-columns: 1fr;
				}

				.display-two {
					grid-template-columns: repeat(2, minmax(0, 1fr));
				}

				.display-preview {
					width: 100%;
				}

				.display-screens {
					grid-template-columns: repeat(2, minmax(0, 1fr));
				}
			}

			/* Browser zoom and the LuCI sidebar reduce the usable content width
			 * without necessarily crossing a viewport media breakpoint. Switch the
			 * two control cards by their real container width so range values never
			 * escape the card. */
			@container display-controls (max-width: 760px) {
				.display-two {
					grid-template-columns: 1fr;
				}
			}

			@media (max-width: 1024px) {
				.display-two {
					grid-template-columns: 1fr;
				}
			}

			@media (max-width: 640px) {
				.display-page-head {
					gap: 10px;
					min-height: 64px;
					margin: 0 -8px;
					padding: 8px 18px 12px 20px;
					background: linear-gradient(to bottom, #6074e5 0, #6074e5 30px, transparent 30px);
				}

				.display-page-head::before {
					inset: 8px 8px 12px;
				}

				.display-page-head h2 {
					min-height: 44px;
					margin: 0 !important;
					padding: 6px 0;
					font-size: 18px;
				}

				.display-master-switch {
					margin: 0 0 0 auto;
				}

				.display-app {
					padding: 8px;
					border-radius: 6px;
				}

				.display-card,
				.display-select {
					padding: 8px;
				}

				.display-head {
					align-items: center;
					flex-direction: row;
				}

				.display-title {
					min-width: 0;
					font-size: 16px;
				}

				.display-mobile-percent {
					display: block;
					color: var(--display-blue);
					font-size: 24px;
					font-weight: 800;
					line-height: 1;
				}

				.display-mobile-brightness-line {
					display: flex;
					align-items: center;
					justify-content: space-between;
					gap: 10px;
				}

				.display-row {
					align-items: stretch;
					flex-direction: column;
					gap: 8px;
				}

				.display-row > .display-percent,
				.display-row > .display-icon {
					display: none;
				}

				.display-percent {
					min-width: 0;
				}

				.display-screens,
				.display-stats {
					grid-template-columns: 1fr;
				}

				.display-power-row {
					grid-template-columns: minmax(0, 1fr) auto;
				}

				.display-power-row > .display-time-field {
					grid-column: 1;
					grid-row: 2;
				}

				.display-power-row > .display-switch {
					grid-column: 2;
					grid-row: 1 / span 2;
					align-self: center;
				}

				.display-schedule-row {
					grid-template-columns: minmax(0, 1fr) auto;
				}

				.display-schedule-name {
					grid-column: 1;
					grid-row: 1;
				}

				.display-schedule-row .display-time-field {
					grid-column: 1 / span 2;
					grid-row: 2;
				}

				.display-schedule-row .display-range {
					grid-column: 1 / span 2;
					grid-row: 3;
					width: 100%;
					min-width: 0;
				}

				.display-schedule-percent {
					grid-column: 2;
					grid-row: 1;
					align-self: center;
					text-align: right;
				}

				.display-time-picker {
					width: 100%;
				}

				.display-preview-foot {
					align-items: flex-start;
					flex-wrap: wrap;
					line-height: 1.35;
				}
			}

			@media (max-width: 420px) {
				.display-app,
				.display-card,
				.display-select {
					padding: 7px;
				}

				.display-grid,
				.display-left,
				.display-two,
				.display-time-grid,
				.display-screens {
					gap: 6px;
				}

				.display-switch span {
					width: 36px;
					height: 20px;
				}

				.display-switch span:before {
					width: 14px;
					height: 14px;
				}

				.display-switch input:checked + span:before {
					transform: translateX(15px);
				}

				.display-screen-card-title,
				.display-preview-foot {
					font-size: 12px;
				}
			}
		` ]));
	},

	icon: function(name) {
		var icons = {
			sun: {
				viewBox: '0 0 1024 1024',
				body: '<path d="M457.475135 976.622041l-0.511799-7.311413V950.885866a54.908714 54.908714 0 0 1 109.232516-7.311413l0.438684 7.311413v18.278534a54.8356 54.8356 0 0 1-109.159401 7.311413z m-307.591161-102.359787a54.8356 54.8356 0 0 1-5.337332-71.432509l5.337332-6.141587 12.868088-12.868087a54.8356 54.8356 0 0 1 82.911427 71.432508l-5.337331 6.141588-12.868088 12.868087a54.8356 54.8356 0 0 1-77.574096 0z m652.397415 5.337332l-6.141587-5.337332-12.868087-12.868087a54.8356 54.8356 0 0 1 71.432508-82.838314l6.141588 5.337332 12.868087 12.868087a54.8356 54.8356 0 0 1-71.432509 82.838314zM182.785334 512.201064A329.013602 329.013602 0 1 1 511.798936 841.214666 329.379172 329.379172 0 0 1 182.785334 512.201064z m73.114134 0A255.899468 255.899468 0 1 0 511.798936 256.301596 255.899468 255.899468 0 0 0 255.899468 512.201064z m73.114134 0A182.785334 182.785334 0 0 1 511.798936 329.41573v365.570668A182.785334 182.785334 0 0 1 329.013602 512.201064z m621.470137 54.8356a54.8356 54.8356 0 0 1-7.311414-109.159401l7.311414-0.511799h18.278533a54.8356 54.8356 0 0 1 7.311413 109.232515l-7.311413 0.438685z m-895.648139 0a54.8356 54.8356 0 0 1-7.311413-109.159401l7.311413-0.511799H73.114134a54.908714 54.908714 0 0 1 7.311413 109.232515L73.114134 567.036664z m728.436115-326.235265a54.8356 54.8356 0 0 1-5.337332-71.432508l5.337332-6.214702 12.868087-12.868087a54.8356 54.8356 0 0 1 82.838314 71.432509l-5.337332 6.214701-12.868088 12.868087a54.8356 54.8356 0 0 1-77.500981 0z m-620.446539 0l-12.941202-12.941201A54.8356 54.8356 0 0 1 227.45807 150.286102l12.868088 12.868087a54.8356 54.8356 0 0 1-77.500982 77.64721zM457.475135 80.827675L456.963336 73.516261V55.237728a54.908714 54.908714 0 0 1 109.232516-7.311413l0.438684 7.311413V73.516261a54.8356 54.8356 0 0 1-109.159401 7.311414z"></path>'
			},
			clock: {
				viewBox: '0 0 1025 1024',
				body: '<path d="M874.291209 149.9136C777.540276 53.256533 648.960009 0 512.119476 0 375.321609 0 246.724276 53.256533 150.016009 149.930667 53.273609 246.6304 9.0e-06 375.227733 9.0e-06 512c-0.0256 136.797867 53.205333 265.352533 149.9392 362.120533C246.630409 970.734933 375.244809 1024 512.025609 1024c136.840533 0 265.4976-53.2992 362.2656-149.981867C1073.928543 674.414933 1073.962676 349.602133 874.291209 149.9136L874.291209 149.9136 874.291209 149.9136zM827.093343 826.8288c-84.155733 84.1216-196.0448 130.491733-315.067733 130.491733-118.9376 0-230.801067-46.353067-314.9056-130.389333C113.024009 742.8352 66.713609 630.9888 66.739209 512c0-118.971733 46.327467-230.826667 130.449067-314.897067C281.309876 113.024 393.139209 66.730667 512.119476 66.730667c118.9888 0 230.8352 46.293333 314.973867 130.3808C1000.729609 370.7392 1000.729609 653.2608 827.093343 826.8288L827.093343 826.8288 827.093343 826.8288zM733.422943 637.474133 556.390409 535.125333 556.390409 256c0-23.552-19.114667-42.666667-42.683733-42.666667-23.552 0-42.666667 19.106133-42.666667 42.666667l0 298.6752c0 15.829333 19.677867 29.098667 32.477867 36.437333 3.541333 5.3248 13.269333 10.154667 19.208533 13.568l176.0512 106.6752c20.394667 11.767467 42.504533 4.770133 54.280533-15.616C764.834143 675.328 753.868809 649.258667 733.422943 637.474133L733.422943 637.474133 733.422943 637.474133zM733.422943 637.474133"></path>'
			},
			screen: {
				viewBox: '0 0 24 24',
				stroke: true,
				body: '<rect x="3" y="4" width="18" height="13" rx="2"/><path d="M8 21h8M12 17v4"/>'
			},
			save: {
				viewBox: '0 0 1024 1024',
				body: '<path d="M870.4 0H153.6A153.6 153.6 0 0 0 0 153.6v716.8A153.984 153.984 0 0 0 153.6 1024h716.8A153.984 153.984 0 0 0 1024 870.4V153.6A153.6 153.6 0 0 0 870.4 0z m-96.32 71.296V432H249.984V71.296H774.08zM952.704 870.4a82.112 82.112 0 0 1-82.304 81.92H153.6a82.112 82.112 0 0 1-82.304-81.92V153.6c0-45.44 36.864-82.24 82.304-82.304h24.96V467.84a35.456 35.456 0 0 0 35.648 35.456h595.456a35.456 35.456 0 0 0 35.712-35.456V71.296H870.4c45.44 0 82.24 36.864 82.304 82.304v716.8zM649.6 355.456a35.84 35.84 0 0 0 35.648-35.84V172.608a35.712 35.712 0 0 0-71.36 0v147.008a35.84 35.84 0 0 0 35.584 35.84h0.128zM256 658.304h512a36.544 36.544 0 0 1 0 73.152H256a36.544 36.544 0 0 1 0-73.152z"></path>'
			},
			reset: {
				viewBox: '0 0 1024 1024',
				body: '<path d="M911.40249 607.60166c-21.244813-4.248963-46.738589 8.497925-50.987552 29.742738-42.489627 174.207469-195.452282 293.178423-373.908714 293.178424-212.448133 0-386.655602-174.207469-386.655602-386.655602s174.207469-386.655602 386.655602-386.655602c97.726141 0 195.452282 38.240664 263.435685 106.224067h-178.456432c-21.244813 0-42.489627 16.995851-42.489626 42.489626 0 21.244813 16.995851 42.489627 42.489626 42.489627h263.435685c21.244813 0 42.489627-16.995851 42.489626-42.489627v-263.435684c0-21.244813-16.995851-42.489627-42.489626-42.489627-21.244813 0-42.489627 16.995851-42.489627 42.489627v148.713693c-84.979253-76.481328-195.452282-118.970954-310.174274-118.970955-259.186722 0-471.634855 212.448133-471.634854 471.634855s212.448133 471.634855 471.634854 471.634855c216.697095 0 403.651452-148.713693 458.887967-356.912863 4.248963-21.244813-8.497925-42.489627-29.742738-50.987552z"></path>'
			}
		};
		var icon = icons[name] || icons.screen;
		var svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
		var parsed = new DOMParser().parseFromString(
			'<svg xmlns="http://www.w3.org/2000/svg">' + icon.body + '</svg>',
			'image/svg+xml'
		).documentElement;

		svg.setAttribute('viewBox', icon.viewBox);

		if (icon.stroke) {
			svg.setAttribute('fill', 'none');
			svg.setAttribute('stroke', 'currentColor');
			svg.setAttribute('stroke-width', '2');
			svg.setAttribute('stroke-linecap', 'round');
			svg.setAttribute('stroke-linejoin', 'round');
		} else {
			svg.setAttribute('fill', 'currentColor');
		}

		Array.prototype.slice.call(parsed.childNodes).forEach(function(child) {
			svg.appendChild(document.importNode(child, true));
		});

		return E('span', { class: 'display-icon', 'aria-hidden': 'true' }, [
			svg
		]);
	},

	screenImage: function(id) {
		return L.resource('display/screen' + id + '.png');
	},

	readFramebuffer: function() {
		return fs.exec_direct(HELPER, [ 'frame' ], 'blob').then(function(blob) {
			return blob.arrayBuffer();
		});
	},

	framebuffersDiffer: function(left, right) {
		var a, b, i;

		if (!(left instanceof ArrayBuffer) || !(right instanceof ArrayBuffer))
			return true;

		if (left.byteLength !== right.byteLength)
			return true;

		a = new Uint8Array(left);
		b = new Uint8Array(right);
		for (i = 0; i < a.length; i++) {
			if (a[i] !== b[i])
				return true;
		}

		return false;
	},

	waitForFramebufferChange: function(generation, frozen, attempts) {
		var self = this;

		if (generation !== (this.frameGeneration || 0))
			return Promise.resolve(null);

		return this.readFramebuffer().then(function(buffer) {
			if (generation !== (self.frameGeneration || 0))
				return null;

			if (self.framebuffersDiffer(frozen, buffer))
				return buffer;

			if (attempts <= 0)
				return null;

			return new Promise(function(resolve) {
				window.setTimeout(resolve, 50);
			}).then(function() {
				return self.waitForFramebufferChange(generation, frozen, attempts - 1);
			});
		}, function() {
			if (attempts <= 0 || generation !== (self.frameGeneration || 0))
				return null;

			return new Promise(function(resolve) {
				window.setTimeout(resolve, 50);
			}).then(function() {
				return self.waitForFramebufferChange(generation, frozen, attempts - 1);
			});
		});
	},

	paintFramebuffer: function(canvas) {
		var info = this.frameInfo || {};
		var buffer = this.frameBuffer;
		var width = this.clamp(info.width || 428, 1, 4096);
		var height = this.clamp(info.height || 142, 1, 4096);
		var bpp = this.clamp(info.bpp || 16, 1, 64);
		var bytesPerPixel = Math.ceil(bpp / 8);
		var stride = this.clamp(info.stride || width * bytesPerPixel, width * bytesPerPixel, 1048576);
		var bytes, source, sourceContext, image, out, x, y, offset, value;
		var targetContext;

		if (!canvas || !(buffer instanceof ArrayBuffer))
			return false;

		if (bpp !== 16 && bpp !== 24 && bpp !== 32)
			return false;

		bytes = new Uint8Array(buffer);
		if (bytes.byteLength < stride * height)
			return false;

		source = document.createElement('canvas');
		source.width = width;
		source.height = height;
		sourceContext = source.getContext('2d');
		image = sourceContext.createImageData(width, height);
		out = image.data;

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				offset = y * stride + x * bytesPerPixel;
				value = (y * width + x) * 4;

				if (bpp === 16) {
					var rgb565 = bytes[offset] | (bytes[offset + 1] << 8);
					out[value] = Math.round(((rgb565 >> 11) & 0x1f) * 255 / 31);
					out[value + 1] = Math.round(((rgb565 >> 5) & 0x3f) * 255 / 63);
					out[value + 2] = Math.round((rgb565 & 0x1f) * 255 / 31);
				} else {
					/* Linux framebuffer 24/32-bit layouts on this target are BGR(A). */
					out[value] = bytes[offset + 2];
					out[value + 1] = bytes[offset + 1];
					out[value + 2] = bytes[offset];
				}

				out[value + 3] = 255;
			}
		}

		sourceContext.putImageData(image, 0, 0);
		targetContext = canvas.getContext('2d');
		targetContext.clearRect(0, 0, canvas.width, canvas.height);
		targetContext.imageSmoothingEnabled = false;
		targetContext.drawImage(source, 0, 0, canvas.width, canvas.height);
		return true;
	},

	renderPreviewVisual: function() {
		var canvas;

		if (this.state && this.state.enabled && this.frameBuffer) {
			canvas = E('canvas', {
				class: 'display-live-frame',
				width: '428',
				height: '142',
				'aria-label': _('当前屏幕实时预览')
			});

			if (this.paintFramebuffer(canvas))
				return canvas;
		}

		return E('img', { src: this.previewImage(), alt: _('当前屏幕预览') });
	},

	updateLivePreviewDom: function() {
		var frame = document.querySelector('.display-preview-frame');
		var canvas = frame ? frame.querySelector('canvas.display-live-frame') : null;

		if (!frame)
			return;

		if (this.state.enabled && canvas && this.paintFramebuffer(canvas))
			return;

		dom.content(frame, [ this.renderPreviewVisual() ]);
	},

	captureFramebuffer: function() {
		var self = this;
		var generation = this.frameGeneration || 0;

		if (this.frameCaptureBusy || this.frameSyncPaused || !this.state || !this.state.enabled)
			return Promise.resolve();

		this.frameCaptureBusy = true;
		return this.readFramebuffer().then(function(buffer) {
			self.frameCaptureBusy = false;
			if (self.frameSyncPaused || generation !== (self.frameGeneration || 0))
				return;

			self.frameBuffer = buffer;
			self.updateLivePreviewDom();
		}, function() {
			/* Keep the static selected-screen fallback if fb0 is unavailable. */
			self.frameCaptureBusy = false;
		});
	},

	startFramebufferSync: function() {
		var self = this;

		if (this.frameTimer)
			window.clearInterval(this.frameTimer);

		/* Do not block initial page rendering on fb0. The selected static theme is
		 * painted immediately, then this first capture replaces it as soon as the
		 * binary frame arrives. */
		this.captureFramebuffer();
		this.frameTimer = window.setInterval(function() {
			if (!document.querySelector('.display-preview-frame')) {
				window.clearInterval(self.frameTimer);
				self.frameTimer = null;
				return;
			}

			if (!document.hidden)
				self.captureFramebuffer();
		}, FRAME_REFRESH_MS);
	},

	previewImage: function() {
		return this.screenImage(this.state && this.state.enabled ? this.state.screen : 0);
	},

	currentScreenTitle: function() {
		if (!this.state || !this.state.enabled)
			return _('显示已关闭');

		return SCREENS[this.state.screen - 1].title;
	},

	clamp: function(value, min, max) {
		value = parseInt(value, 10);
		if (isNaN(value))
			value = min;

		return Math.max(min, Math.min(max, value));
	},

	brightnessToPercent: function(raw, max) {
		raw = this.clamp(raw, 0, max);
		if (!max)
			return 0;

		return this.clamp(Math.round((max - raw) * 100 / max), 0, 100);
	},

	percentToBrightness: function(percent) {
		var max = this.state && this.state.maxBrightness ? this.state.maxBrightness : 26;

		percent = this.clamp(percent, 0, 100);
		return this.clamp(Math.round(max * (100 - percent) / 100), 0, max);
	},

	displayBrightnessValue: function(raw) {
		var max = this.state && this.state.maxBrightness ? this.state.maxBrightness : 26;

		return this.clamp(max - this.clamp(raw, 0, max), 0, max);
	},

	setRangeValue: function(range, percent) {
		var wrap;

		percent = this.clamp(percent, 0, 100);

		if (!range)
			return;

		range.value = percent;
		wrap = range.closest ? range.closest('.display-range') : range.parentNode;

		if (wrap) {
			wrap.style.setProperty('--value', percent + '%');
			wrap.style.setProperty('--value-num', String(percent / 100));
		}
	},

	rangeControl: function(percent, className, onInput, onChange) {
		percent = this.clamp(percent, 0, 100);

		return E('span', {
			class: 'display-range ' + (className || ''),
			style: '--value:' + percent + '%;--value-num:' + (percent / 100)
		}, [
			E('input', {
				class: 'display-slider',
				type: 'range',
				min: '0',
				max: '100',
				value: String(percent),
				input: onInput,
				change: onChange
			})
		]);
	},

	parseStatus: function(result) {
		var data = {};

		try {
			data = JSON.parse((result && result.stdout ? result.stdout : '').trim() || '{}');
		} catch (e) {
			data = {};
		}

		return data.ok ? data : {};
	},

	uciBool: function(option, fallback) {
		var value = uci.get('display', 'settings', option);

		if (value == null)
			return !!fallback;

		return value === '1' || value === 'true' || value === 'on';
	},

	readState: function(status) {
		var max = this.clamp(status.max_brightness || 26, 1, 255);
		var raw = this.clamp(status.brightness != null ? status.brightness : max, 0, max);
		var savedPercent = this.clamp(uci.get('display', 'settings', 'brightness_percent') || 72, 0, 100);
		/* The helper reads committed UCI directly, so prefer it over LuCI's
		 * potentially stale in-memory cache after an immediate page refresh. */
		var screen = this.clamp(status.screen || uci.get('display', 'settings', 'screen') || 1, 1, 4);
		var hardwarePercent = status.brightness != null ? this.brightnessToPercent(raw, max) : savedPercent;

		this.state = {
			enabled: status.enabled != null ? status.enabled !== 0 : this.uciBool('enabled', true),
			maxBrightness: max,
			rawBrightness: raw,
			percent: hardwarePercent,
			screen: screen,
			timerEnabled: this.uciBool('timer_enabled', false),
			powerOnEnabled: this.uciBool('power_on_enabled', false),
			powerOnTime: uci.get('display', 'settings', 'power_on_time') || '07:30',
			powerOffEnabled: this.uciBool('power_off_enabled', false),
			powerOffTime: uci.get('display', 'settings', 'power_off_time') || '22:00',
			brightnessScheduleEnabled: this.uciBool('brightness_schedule_enabled', false),
			brightnessSlots: [
				{ key: 'slot2', name: _('日间模式'), time: uci.get('display', 'settings', 'brightness_slot2_time') || '08:30', percent: this.clamp(uci.get('display', 'settings', 'brightness_slot2_percent') || 80, 0, 100) },
				{ key: 'slot3', name: _('夜间模式'), time: uci.get('display', 'settings', 'brightness_slot3_time') || '21:30', percent: this.clamp(uci.get('display', 'settings', 'brightness_slot3_percent') || 10, 0, 100) }
			]
		};

		this.frameInfo = {
			width: this.clamp(status.fb_width || 428, 1, 4096),
			height: this.clamp(status.fb_height || 142, 1, 4096),
			bpp: this.clamp(status.fb_bpp || 16, 1, 64),
			stride: this.clamp(status.fb_stride || 856, 1, 1048576)
		};
	},

	runHelper: function(args) {
		return fs.exec(HELPER, args).then(function(result) {
			var message = result && result.stderr ? result.stderr.trim() : '';
			var json = {};

			try {
				json = JSON.parse((result && result.stdout ? result.stdout : '').trim() || '{}');
			} catch (e) {
				json = {};
			}

			if (result && result.code)
				throw new Error((json && json.message) || message || _('Command failed'));

			if (json && json.ok === false)
				throw new Error(json.message || _('Command failed'));

			return json;
		});
	},

	fireHelper: function(args) {
		fs.exec(HELPER, args).catch(function() {});
	},

	refreshLuCIChanges: function() {
		if (!(ui.changes && ui.changes.init))
			return;

		window.setTimeout(function() {
			ui.changes.init();
		}, 900);

		window.setTimeout(function() {
			ui.changes.init();
		}, 2200);
	},

	showToast: function(message) {
		var toast = document.querySelector('.display-toast');

		if (!toast) {
			toast = document.body.appendChild(E('div', { class: 'display-toast' }));
		}

		toast.textContent = message;
		toast.classList.add('is-visible');

		if (this.toastTimer)
			window.clearTimeout(this.toastTimer);

		this.toastTimer = window.setTimeout(function() {
			toast.classList.remove('is-visible');
		}, 2600);
	},

	updateBrightnessDom: function() {
		var range = document.querySelector('.display-main-range .display-slider');
		var percent = document.querySelector('.display-percent');
		var mobilePercent = document.querySelector('.display-mobile-percent');
		var raw = document.querySelector('.display-raw-value');

		this.setRangeValue(range, this.state.percent);

		if (percent)
			percent.textContent = this.state.percent + '%';

		if (mobilePercent)
			mobilePercent.textContent = this.state.percent + '%';

		if (raw)
			raw.textContent = this.displayBrightnessValue(this.percentToBrightness(this.state.percent));
	},

	updateScreenDom: function() {
		var label = document.querySelector('.display-current-screen');
		var cards = document.querySelectorAll('.display-screen-card');
		var i;

		this.updateLivePreviewDom();

		if (label)
			label.textContent = this.currentScreenTitle();

		for (i = 0; i < cards.length; i++)
			cards[i].classList.toggle('is-active', parseInt(cards[i].getAttribute('data-screen'), 10) === this.state.screen);
	},

	updateScreenSelectionDom: function(screen) {
		var cards = document.querySelectorAll('.display-screen-card');
		var i;

		for (i = 0; i < cards.length; i++)
			cards[i].classList.toggle('is-active', parseInt(cards[i].getAttribute('data-screen'), 10) === screen);
	},

	updateFeatureDom: function() {
		var input = document.querySelector('.display-master-switch input');

		if (input)
			input.checked = !!this.state.enabled;

		this.updateScreenDom();
	},

	applyBrightness: function() {
		var self = this;
		var raw = this.percentToBrightness(this.state.percent);

		this.state.rawBrightness = raw;
		this.updateBrightnessDom();

		if (!this.state.enabled)
			return this.persistSettings();

		/* Dragging only updates the preview. On release, apply the final
		 * hardware value first, then perform exactly one durable UCI commit. */
		return this.runHelper([ 'brightness', String(raw) ]).then(function() {
			return self.persistSettings();
		}).catch(function(error) {
			self.showToast(error.message || _('设置应用失败'));
		});
	},

	selectScreen: function(screen) {
		var self = this;
		var selected = this.clamp(screen, 1, 4);
		var generation, frozen, operation;

		if (selected === this.state.screen && !this.pendingScreen)
			return this.captureFramebuffer();

		this.frameGeneration = (this.frameGeneration || 0) + 1;
		generation = this.frameGeneration;
		this.frameSyncPaused = true;
		this.pendingScreen = selected;
		frozen = this.frameBuffer;

		/* Click fires on button release. Highlight the target immediately, but
		 * leave the current live canvas untouched. It stays frozen until fb0 has
		 * actually drawn the selected screen. */
		this.updateScreenSelectionDom(selected);

		/* One RPC both commits the selected screen and applies it. This avoids
		 * the old split-request state where hardware changed but UCI stayed at 1.
		 * Serialize rapid clicks, while framebuffer polling remains per-click. */
		operation = (this.screenQueue || Promise.resolve()).catch(function() {}).then(function() {
			return self.runHelper([ 'screen-persist', String(selected) ]);
		});
		this.screenQueue = operation;

		return operation.then(function(result) {
			if (parseInt(result.screen, 10) !== selected)
				throw new Error(_('Screen persistence verification failed'));

			if (generation !== self.frameGeneration)
				return null;

			return self.waitForFramebufferChange(generation, frozen, 20);
		}).then(function(buffer) {
			var label;

			if (generation !== self.frameGeneration)
				return;

			self.state.screen = selected;
			self.pendingScreen = null;
			self.frameSyncPaused = false;

			if (buffer) {
				self.frameBuffer = buffer;
				self.updateScreenDom();
				return;
			}

			/* Keep the old frozen preview if fb0 is briefly unavailable. Never flash
			 * a default static target image during a switch; the 100 ms loop will
			 * replace this frame as soon as the real framebuffer can be read. */
			label = document.querySelector('.display-current-screen');
			if (label)
				label.textContent = self.currentScreenTitle();
			self.updateScreenSelectionDom(selected);
			return self.captureFramebuffer();
		}).catch(function(error) {
			if (generation !== self.frameGeneration)
				return;

			self.pendingScreen = null;
			self.updateScreenSelectionDom(self.state.screen);
			self.frameSyncPaused = false;
			self.showToast(error.message || _('设置应用失败'));
		});
	},

	settingsArgs: function(command) {
		return [
			command,
			this.state.enabled ? '1' : '0',
			String(this.state.percent),
			String(this.state.screen),
			this.state.timerEnabled ? '1' : '0',
			this.state.powerOnEnabled ? '1' : '0',
			this.state.powerOnTime || '07:30',
			this.state.powerOffEnabled ? '1' : '0',
			this.state.powerOffTime || '22:00',
			this.state.brightnessScheduleEnabled ? '1' : '0',
			this.state.brightnessSlots[0].time || '08:30',
			String(this.state.brightnessSlots[0].percent),
			this.state.brightnessSlots[1].time || '21:30',
			String(this.state.brightnessSlots[1].percent)
		];
	},

	persistSettings: function() {
		var self = this;
		var args = this.settingsArgs('persist');

		return this.runHelper(args).catch(function(error) {
			self.showToast(error.message || _('设置应用失败'));
		});
	},

	setFeatureEnabled: function(enable) {
		var self = this;
		var previous = this.state.enabled;
		var desired = !!enable;
		var generation;
		var operation;

		this.frameGeneration = (this.frameGeneration || 0) + 1;
		generation = this.frameGeneration;
		this.frameSyncPaused = true;
		this.frameBuffer = null;
		this.state.enabled = desired;
		this.updateFeatureDom();

		/* Serialize total-switch clicks and let one helper RPC own UCI, panel
		 * power and display process state. The old parallel apply/off request was
		 * able to finish out of order and made F5 read a stale enabled value. */
		operation = (this.featureQueue || Promise.resolve()).catch(function() {}).then(function() {
			return self.runHelper([ desired ? 'enable' : 'disable' ]);
		});
		this.featureQueue = operation;

		return operation.then(function() {
			if (generation !== self.frameGeneration)
				return;

			self.frameSyncPaused = false;
			self.updateFeatureDom();
			if (desired)
				return self.captureFramebuffer();
		}).catch(function(error) {
			if (generation !== self.frameGeneration)
				return;

			self.frameSyncPaused = false;
			self.state.enabled = previous;
			self.updateFeatureDom();
			self.showToast(error.message || _('显示开关操作失败'));
		});
	},

	switchControl: function(checked, change, className) {
		return E('label', { class: 'display-switch' + (className ? ' ' + className : '') }, [
			E('input', {
				type: 'checkbox',
				checked: checked ? true : null,
				change: change
			}),
			E('span', {})
		]);
	},

	timePart: function(value) {
		value = parseInt(value, 10);
		if (isNaN(value))
			value = 0;

		return value < 10 ? '0' + value : String(value);
	},

	normalizeTime: function(value) {
		var parts = String(value || '').split(':');
		var hour = this.clamp(parts[0], 0, 23);
		var minute = this.clamp(parts[1], 0, 59);

		return this.timePart(hour) + ':' + this.timePart(minute);
	},

	closeTimePickers: function(except) {
		var fields = document.querySelectorAll('.display-time-field.is-open');
		var i;

		for (i = 0; i < fields.length; i++) {
			if (fields[i] !== except)
				fields[i].classList.remove('is-open');
		}
	},

	openTimePicker: function(field) {
		if (!this.timePickerBound) {
			document.addEventListener('click', L.bind(function(ev) {
				if (!ev.target.closest || !ev.target.closest('.display-time-field'))
					this.closeTimePickers();
			}, this));
			this.timePickerBound = true;
		}

		this.closeTimePickers(field);
		field.classList.toggle('is-open');
	},

	selectTimePart: function(field, part, value, change) {
		var current = this.normalizeTime(field.getAttribute('data-value'));
		var pieces = current.split(':');
		var next;
		var valueNode = field.querySelector('.display-time-value');

		if (part === 'hour')
			pieces[0] = value;
		else
			pieces[1] = value;

		next = pieces[0] + ':' + pieces[1];
		field.setAttribute('data-value', next);

		if (valueNode)
			valueNode.textContent = next;

		this.updateTimePicker(field, change);

		if (change)
			change({ target: { value: next } });
	},

	stepTimePart: function(field, part, delta, change) {
		var current = this.normalizeTime(field.getAttribute('data-value'));
		var pieces = current.split(':');
		var count = part === 'hour' ? 24 : 60;
		var idx = part === 'hour' ? 0 : 1;
		var value = (parseInt(pieces[idx], 10) + delta) % count;

		if (value < 0)
			value += count;

		this.selectTimePart(field, part, this.timePart(value), change);
	},

	updateTimePicker: function(field, change) {
		var current = this.normalizeTime(field.getAttribute('data-value'));
		var parts = current.split(':');
		var columns = field.querySelectorAll('.display-time-column');

		if (columns[0])
			dom.content(columns[0], this.timeOptions('hour', parts[0], 24, change));

		if (columns[1])
			dom.content(columns[1], this.timeOptions('minute', parts[1], 60, change));
	},

	timeOptions: function(part, selected, count, change) {
		var self = this;
		var items = [];
		var selectedInt = this.clamp(selected, 0, count - 1);
		var value;
		var raw;
		var i;

		for (i = -2; i <= 2; i++) {
			raw = (selectedInt + i) % count;
			if (raw < 0)
				raw += count;

			value = this.timePart(raw);
			items.push(E('button', {
				class: 'display-time-option' + (value === selected ? ' is-selected' : ''),
				type: 'button',
				'data-part': part,
				'data-value': value,
				click: function(ev) {
					var field = ev.currentTarget.closest('.display-time-field');
					self.selectTimePart(field, ev.currentTarget.getAttribute('data-part'), ev.currentTarget.getAttribute('data-value'), change);
				},
				wheel: function(ev) {
					var field = ev.currentTarget.closest('.display-time-field');
					ev.preventDefault();
					self.stepTimePart(field, ev.currentTarget.getAttribute('data-part'), ev.deltaY > 0 ? 1 : -1, change);
				}
			}, value));
		}

		return items;
	},

	timeInput: function(value, change) {
		var self = this;
		var time = this.normalizeTime(value);
		var parts = time.split(':');

		return E('span', { class: 'display-time-field', 'data-value': time }, [
			E('button', {
				class: 'display-time-trigger',
				type: 'button',
				click: function(ev) {
					ev.preventDefault();
					self.openTimePicker(ev.currentTarget.closest('.display-time-field'));
				}
			}, [
				E('span', { class: 'display-time-value' }, time),
				E('span', { class: 'display-time-clock' }, [
					this.icon('clock')
				])
			]),
			E('span', { class: 'display-time-picker' }, [
				E('span', {
					class: 'display-time-column',
					wheel: function(ev) {
						var field = ev.currentTarget.closest('.display-time-field');
						ev.preventDefault();
						self.stepTimePart(field, 'hour', ev.deltaY > 0 ? 1 : -1, change);
					}
				}, this.timeOptions('hour', parts[0], 24, change)),
				E('span', {
					class: 'display-time-column',
					wheel: function(ev) {
						var field = ev.currentTarget.closest('.display-time-field');
						ev.preventDefault();
						self.stepTimePart(field, 'minute', ev.deltaY > 0 ? 1 : -1, change);
					}
				}, this.timeOptions('minute', parts[1], 60, change))
			])
		]);
	},

	renderBrightnessCard: function() {
		var self = this;

		return E('section', { class: 'display-card' }, [
			E('div', { class: 'display-head' }, [
				E('div', { class: 'display-title' }, [
					this.icon('sun'),
					E('span', {}, _('亮度调节'))
				])
			]),
			E('div', { class: 'display-row' }, [
				E('span', { class: 'display-mobile-brightness-line' }, [
					E('span', { class: 'display-label' }, _('亮度')),
					E('strong', { class: 'display-mobile-percent' }, this.state.percent + '%')
				]),
				E('strong', { class: 'display-percent' }, this.state.percent + '%'),
				this.rangeControl(this.state.percent, 'display-main-range', function(ev) {
					self.state.percent = self.clamp(ev.target.value, 0, 100);
					self.updateBrightnessDom();
				}, function() {
					self.applyBrightness();
				}),
				this.icon('sun')
			]),
			E('div', { class: 'display-stats' }, [
				E('div', { class: 'display-stat' }, [
					E('span', {}, _('当前亮度值')),
					E('strong', { class: 'display-raw-value' }, String(this.displayBrightnessValue(this.percentToBrightness(this.state.percent))))
				]),
				E('div', { class: 'display-stat' }, [
					E('span', {}, _('最大亮度值')),
					E('strong', {}, String(this.state.maxBrightness))
				])
			])
		]);
	},

	renderTimerCard: function() {
		var self = this;

		return E('section', { class: 'display-card display-compact-card display-timer-card' }, [
			E('div', { class: 'display-head' }, [
				E('div', { class: 'display-title' }, [
					this.icon('clock'),
					E('span', {}, _('定时开关'))
				]),
				this.switchControl(this.state.timerEnabled, function(ev) {
					self.state.timerEnabled = ev.target.checked;
					self.persistSettings();
				})
			]),
			E('div', { class: 'display-time-grid' }, [
				E('div', { class: 'display-time-row' }, [
					E('div', { class: 'display-power-row' }, [
						E('span', { class: 'display-switch-text' }, [
							E('span', {}, _('定时开启'))
						]),
						this.timeInput(this.state.powerOnTime, function(ev) {
							self.state.powerOnTime = ev.target.value || '07:30';
							self.persistSettings();
						}),
						this.switchControl(this.state.powerOnEnabled, function(ev) {
							self.state.powerOnEnabled = ev.target.checked;
							self.persistSettings();
						})
					])
				]),
				E('div', { class: 'display-time-row' }, [
					E('div', { class: 'display-power-row' }, [
						E('span', { class: 'display-switch-text' }, [
							E('span', {}, _('定时关闭'))
						]),
						this.timeInput(this.state.powerOffTime, function(ev) {
							self.state.powerOffTime = ev.target.value || '22:00';
							self.persistSettings();
						}),
						this.switchControl(this.state.powerOffEnabled, function(ev) {
							self.state.powerOffEnabled = ev.target.checked;
							self.persistSettings();
						})
					])
				])
			])
		]);
	},

	renderScheduleCard: function() {
		var self = this;

		return E('section', { class: 'display-card display-compact-card display-schedule-card' }, [
			E('div', { class: 'display-head' }, [
				E('div', { class: 'display-title' }, [
					this.icon('clock'),
					E('span', {}, _('定时亮度调节'))
				]),
				this.switchControl(this.state.brightnessScheduleEnabled, function(ev) {
					self.state.brightnessScheduleEnabled = ev.target.checked;
					self.persistSettings();
				})
			]),
			E('div', { class: 'display-time-grid' }, this.state.brightnessSlots.map(function(slot, idx) {
				return E('div', { class: 'display-schedule-row', 'data-slot': String(idx) }, [
					E('span', { class: 'display-schedule-name' }, slot.name),
					self.timeInput(slot.time, function(ev) {
						slot.time = ev.target.value || slot.time;
						self.persistSettings();
					}),
					self.rangeControl(slot.percent, 'display-slot-range', function(ev) {
						var row = ev.target.closest ? ev.target.closest('.display-schedule-row') : ev.target.parentNode.parentNode;
						var percent = self.clamp(ev.target.value, 0, 100);
						var label = row ? row.querySelector('.display-schedule-percent') : null;

						slot.percent = percent;
						self.setRangeValue(ev.target, percent);

						if (label)
							label.textContent = percent + '%';
					}, function() {
						self.persistSettings();
					}),
					E('span', { class: 'display-schedule-percent' }, slot.percent + '%')
				]);
			}))
		]);
	},

	renderPreviewCard: function() {
		return E('section', { class: 'display-card display-preview' }, [
			E('div', { class: 'display-head' }, [
				E('div', { class: 'display-title' }, [
					this.icon('screen'),
					E('span', {}, _('预览'))
				])
			]),
			E('div', { class: 'display-preview-frame' }, [
				this.renderPreviewVisual()
			]),
			E('div', { class: 'display-preview-foot' }, [
				E('span', {}, _('当前显示界面：')),
				E('strong', { class: 'display-current-screen' }, this.currentScreenTitle())
			])
		]);
	},

	renderScreenCard: function(item) {
		var self = this;

		return E('div', {
			class: 'display-screen-card' + (item.id === this.state.screen ? ' is-active' : ''),
			role: 'button',
			tabindex: '0',
			'data-screen': String(item.id),
			click: function() {
				self.selectScreen(item.id);
			},
			keydown: function(ev) {
				if (ev.key === 'Enter' || ev.key === ' ') {
					ev.preventDefault();
					self.selectScreen(item.id);
				}
			}
		}, [
			E('span', { class: 'display-radio' }),
			E('span', { class: 'display-screen-card-title' }, item.title),
			E('span', { class: 'display-screen-image' }, [
				E('img', { src: this.screenImage(item.id), alt: item.title })
			])
		]);
	},

	renderScreenSelect: function() {
		return E('section', { class: 'display-select' }, [
			E('div', { class: 'display-section-title' }, _('选择显示界面')),
			E('div', { class: 'display-screens' }, SCREENS.map(L.bind(this.renderScreenCard, this)))
		]);
	},

	renderPageHeader: function() {
		var self = this;

		return E('div', { class: 'display-page-head' }, [
			E('h2', {}, _('屏幕控制')),
			this.switchControl(this.state.enabled, function(ev) {
				self.setFeatureEnabled(ev.target.checked);
			}, 'display-master-switch')
		]);
	},

	render: function(data) {
		var page;

		this.addStyle();
		this.readState(this.parseStatus(data[0]));
		this.frameBuffer = null;

		page = E('div', { class: 'display-page' }, [
			this.renderPageHeader(),
			E('div', { class: 'display-app' }, [
				E('div', { class: 'display-grid' }, [
					E('div', { class: 'display-left' }, [
						this.renderBrightnessCard(),
						E('div', { class: 'display-two' }, [
							this.renderTimerCard(),
							this.renderScheduleCard()
						])
					]),
					this.renderPreviewCard()
				]),
				this.renderScreenSelect()
			])
		]);

		window.setTimeout(L.bind(this.startFramebufferSync, this), 0);
		return page;
	}
});
