// SPDX-License-Identifier: GPL-2.0+
/*
 * erofs-utils/lib/compressor_liblzma.c
 *
 * Copyright (C) 2021 Gao Xiang <xiang@kernel.org>
 */
#include <stdlib.h>
#include <lzma.h>
#include "erofs/internal.h"
#include "compressor.h"

struct erofs_liblzma_context {
	lzma_options_lzma opt;
	lzma_stream strm;
};

static int erofs_liblzma_compress_destsize(struct erofs_compress *c,
					   void *src, unsigned int *srcsize,
					   void *dst, unsigned int dstsize)
{
	struct erofs_liblzma_context *ctx = c->private_data;
	lzma_stream *strm = &ctx->strm;

	lzma_ret ret = lzma_erofs_encoder(strm, &ctx->opt);
	if (ret != LZMA_OK)
		return -EFAULT;

	strm->next_in = src;
	strm->avail_in = *srcsize;
	strm->next_out = dst;
	strm->avail_out = dstsize;

	ret = lzma_code(strm, LZMA_FINISH);
	if (ret != LZMA_STREAM_END)
		return -EBADMSG;

	*srcsize = strm->total_in;
	return strm->total_out;
}

static int erofs_compressor_liblzma_exit(struct erofs_compress *c)
{
	struct erofs_liblzma_context *ctx = c->private_data;

	if (!ctx)
		return -EINVAL;

	lzma_end(&ctx->strm);
	free(ctx);
	return 0;
}

static int erofs_compressor_liblzma_setlevel(struct erofs_compress *c,
					     int compression_level)
{
	struct erofs_liblzma_context *ctx = c->private_data;

	if (compression_level < 0)
		compression_level = LZMA_PRESET_DEFAULT;

	if (lzma_lzma_preset(&ctx->opt, compression_level))
		return -EINVAL;

	ctx->opt.dict_size = 64U << 10;
	c->compression_level = compression_level;
	return 0;
}

static int erofs_compressor_liblzma_init(struct erofs_compress *c)
{
	struct erofs_liblzma_context *ctx;

	c->alg = &erofs_compressor_lzma;
	ctx = malloc(sizeof(*ctx));
	if (!ctx)
		return -ENOMEM;
	ctx->strm = (lzma_stream)LZMA_STREAM_INIT;
	c->private_data = ctx;
	return 0;
}

struct erofs_compressor erofs_compressor_lzma = {
	.name = "lzma",
	.default_level = LZMA_PRESET_DEFAULT,
	.best_level = LZMA_PRESET_EXTREME,
	.init = erofs_compressor_liblzma_init,
	.exit = erofs_compressor_liblzma_exit,
	.setlevel = erofs_compressor_liblzma_setlevel,
	.compress_destsize = erofs_liblzma_compress_destsize,
};

