#include <linux/dev_printk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/spi-nor.h>
#include <linux/of_platform.h>

#include "core.h"

int spi_nor_parse_of_override(struct spi_nor *nor)
{
    struct mtd_info *mtd = &nor->mtd;
    struct device_node *mtd_node;
    struct device_node *ofregions_node;
    struct device_node *pr;
    struct mtd_erase_region_info *erase_regions;
    const struct spi_nor_erase_map *erase_map;
    int nr_regions, i;
    int ret = 0;

    mtd_node = mtd_get_of_node(mtd);
    if (!mtd_node) {
        dev_warn(nor->dev, "MTD OF section is not found.\n");
		return 0;
    }

    ofregions_node = of_get_child_by_name(mtd_node, "regions");
    if (!ofregions_node) {
        dev_warn(nor->dev, "OF regions section is not found.\n");
        return 0;
    }

    /* First count the subnodes */
	nr_regions = 0;
	for_each_child_of_node(ofregions_node,  pr) {
		nr_regions++;
	}

    if (nr_regions == 0) {
        dev_warn(nor->dev, "no OF regions in \"regions\" section.\n");
		goto out_node;
    }

    erase_map = &nor->params->erase_map;

    if (!erase_map->uniform_erase_type) {
        dev_warn(nor->dev, "Device has non-uniform layout. This is not yet supported for OF region override.\n");

        ret = -EOPNOTSUPP;
        goto out_node;
    }

    erase_regions = devm_kcalloc(nor->dev, nr_regions, sizeof(*erase_regions), GFP_KERNEL);
    if (!erase_regions) {
        ret = -ENOMEM;
        goto out_node;
    }

    i = 0;
    for_each_child_of_node(ofregions_node, pr) {
        const __be32 *reg, *erase_block;
        int reg_len, erase_block_len;
        int et;
        int a_cells, s_cells;
        uint32_t region_offset, region_size, region_erasesize;

        reg = of_get_property(pr, "reg", &reg_len);
		if (!reg) {
			dev_warn(nor->dev, "ofregion region %pOF (%pOF) missing reg property.\n",
				 pr, mtd_node);
			goto ofregion_fail;
		}

        erase_block = of_get_property(pr, "erase-block", &erase_block_len);
        if (!erase_block) {
			dev_warn(nor->dev, "ofregion region %pOF (%pOF) missing reg property.\n",
				 pr, mtd_node);
			goto ofregion_fail;
        }

        a_cells = of_n_addr_cells(pr);
		s_cells = of_n_size_cells(pr);

        if (reg_len / 4 != a_cells + s_cells) {
			dev_dbg(nor->dev, "ofregion region %pOF (%pOF) error parsing reg property.\n",
                      pr, mtd_node);
			goto ofregion_fail;
		}

        if (erase_block_len / 4 != s_cells) {
			dev_dbg(nor->dev, "ofregion region %pOF (%pOF) error parsing erase-block property.\n",
                      pr, mtd_node);
			goto ofregion_fail;
		}

        region_offset = of_read_number(reg, a_cells);
        region_size = of_read_number(reg + a_cells, s_cells);
        region_erasesize = of_read_number(erase_block, s_cells);

        for(et = 0; et < SNOR_ERASE_TYPE_MAX; et++) {
            if (erase_map->erase_type[et].size != region_erasesize)
                continue;

            if (region_offset & erase_map->erase_type[et].size_mask) {
                dev_warn(nor->dev, "ofregion region %pOF (%pOF) offset is not aligned to the requested erase block 0x%08x.\n", pr, mtd_node, region_offset);
                goto ofregion_fail;
            }

            if (region_size & erase_map->erase_type[et].size_mask) {
                dev_warn(nor->dev, "ofregion region %pOF (%pOF) size is not aligned to the requested erase block 0x%08x.\n", pr, mtd_node, region_size);
                goto ofregion_fail;
            }

            break;
        }

        if (et >= SNOR_ERASE_TYPE_MAX) {
            dev_warn(nor->dev, "ofregion region %pOF (%pOF) erase block size 0x%06x is not supported by flash device.\n", pr, mtd_node, region_erasesize);
            goto ofregion_fail;
        }

        erase_regions[i].offset = region_offset;
        erase_regions[i].numblocks = region_size >> erase_map->erase_type[et].size_shift;
        erase_regions[i].erasesize = region_erasesize;
        erase_regions[i].lockmap = NULL;

        dev_info(nor->dev, "found ofregion 0x%08x@0x%08x with erase block size 0x%06x", region_offset, region_size, region_erasesize);

        i++;
    }

    mtd->numeraseregions = nr_regions;
    mtd->eraseregions = erase_regions;

    ret = 0;

    goto out_loop;
ofregion_fail:
	dev_err(nor->dev, "error parsing ofregion region %pOF (%pOF)\n",
	       pr, mtd_node);

    devm_kfree(nor->dev, erase_regions);
    ret = -EINVAL;
out_loop:
    of_node_put(pr);
out_node:
    of_node_put(ofregions_node);
    return ret;
}
