// SPDX-License-Identifier: GPL-2.0+
/*
 * erofs-utils/include/erofs/block_list.h
 *
 * Copyright (C), 2021, Coolpad Group Limited.
 * Created by Yue Hu <huyue2@yulong.com>
 */
#ifndef __EROFS_BLOCK_LIST_H
#define __EROFS_BLOCK_LIST_H

#include "internal.h"

#ifdef WITH_ANDROID
int erofs_droid_blocklist_fopen(void);
void erofs_droid_blocklist_fclose(void);
void erofs_droid_blocklist_write(struct erofs_inode *inode,
				 erofs_blk_t blk_start, erofs_blk_t nblocks);
void erofs_droid_blocklist_write_tail_end(struct erofs_inode *inode,
					  erofs_blk_t blkaddr);
#else
static inline void erofs_droid_blocklist_write(struct erofs_inode *inode,
				 erofs_blk_t blk_start, erofs_blk_t nblocks) {}
static inline
void erofs_droid_blocklist_write_tail_end(struct erofs_inode *inode,
					  erofs_blk_t blkaddr) {}
#endif
#endif
