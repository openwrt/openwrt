# Banana Pi R4 + MT7927 — notas rápidas

Este perfil añade soporte de paquete para MT7927 en BPi-R4.

## Compilación

```bash
cp configs/bananapi-r4-mt7927.config .config
make defconfig
make -j"$(nproc)"
```

## Qué activa

- `kmod-mt7927` (metapaquete)
- `kmod-mt7927-firmware`
- Dependencias de `mt76`/`mac80211` resueltas por `make defconfig`

## Firmware requerido (en árbol fuente)

- `target/linux/mediatek/mt7927/WIFI_MT6639_PATCH_MCU_2_1_hdr.bin`
- `target/linux/mediatek/mt7927/WIFI_RAM_CODE_MT6639_2_1.bin`

## Nota de diseño

`kmod-mt7927` reutiliza la ruta del driver PCIe existente (sin módulo “nuevo” separado), junto con firmware MT6639.

## Alcance

Este documento describe empaquetado y build para MT7927.  
No define ni valida overlays DTS específicos de modelos Wi‑Fi concretos.
