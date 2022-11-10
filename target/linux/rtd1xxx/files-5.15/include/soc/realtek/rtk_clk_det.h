#ifndef __RTK_CLK_DET_H
#define __RTK_CLK_DET_H

struct device;
struct regmap;
struct clk;

enum {
	CLK_DET_TYPE_GENERIC = 0,
	CLK_DET_TYPE_CRT = 0,     /* for compatible */
	CLK_DET_TYPE_SC_WRAP = 1,
};

#if IS_ENABLED(CONFIG_RTK_CLK_DET)

struct clk *clk_det_register(struct device *dev, const char *name,
	struct regmap *regmap, int ofs, int type);
void clk_det_unregister(struct clk *clk);

struct clk *devm_clk_det_register(struct device *dev, const char *name,
        struct regmap *regmap, int ofs, int type);

#else

static inline
struct clk *clk_det_register(struct device *dev, const char *name,
        struct regmap *regmap, int ofs, int type)
{
	return ERR_PTR(-EINVAL);
}

static inline
void clk_det_unregister(struct clk *clk)
{}

static inline
struct clk *devm_clk_det_register(struct device *dev, const char *name,
        struct regmap *regmap, int ofs, int type)
{
	return ERR_PTR(-EINVAL);
}

#endif

#endif
