/*
 * imem I/O
 * Copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavutil/avstring.h"
#include "libavutil/internal.h"
#include "libavutil/opt.h"
#include "avformat.h"
#include <fcntl.h>
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <stdlib.h>
#include "os_support.h"
#include "url.h"
#include "imem.h"

/* Some systems may not have S_ISFIFO */
#ifndef S_ISFIFO
#  ifdef S_IFIFO
#    define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#  else
#    define S_ISFIFO(m) 0
#  endif
#endif

static IMemCallbacks imem_callbacks = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

static const AVOption imem_options[] = {
    { "truncate", "Truncate existing files on write", offsetof(IMemContext, trunc), AV_OPT_TYPE_INT, { .i64 = 1 }, 0, 1, AV_OPT_FLAG_ENCODING_PARAM },
    { "blocksize", "set I/O operation maximum block size", offsetof(IMemContext, blocksize), AV_OPT_TYPE_INT, { .i64 = INT_MAX }, 1, INT_MAX, AV_OPT_FLAG_ENCODING_PARAM },
    { NULL }
};

static const AVClass imem_class = {
    .class_name = "imem",
    .item_name  = av_default_item_name,
    .option     = imem_options,
    .version    = LIBAVUTIL_VERSION_INT,
};

static int imem_read(URLContext *h, unsigned char *buf, int size)
{
    return imem_callbacks.imem_read(h, buf, size);

    /*
    IMemContext *c = h->priv_data;
    int r;
    size = FFMIN(size, c->blocksize);
    r = read(c->fd, buf, size);
    return (-1 == r)?AVERROR(errno):r;
    */
}

static int imem_write(URLContext *h, const unsigned char *buf, int size)
{
    return imem_callbacks.imem_write(h, buf, size);
}

static int imem_get_handle(URLContext *h)
{
    return imem_callbacks.imem_get_handle(h);
}

static int imem_check(URLContext *h, int mask)
{
    return imem_callbacks.imem_check(h, mask);
}

#if CONFIG_IMEM_PROTOCOL

static int imem_open(URLContext *h, const char *filename, int flags)
{
    return imem_callbacks.imem_open(h, filename, flags);
}

/* XXX: use llseek */
static int64_t imem_seek(URLContext *h, int64_t pos, int whence)
{
    return imem_callbacks.imem_seek(h, pos, whence);
}

static int imem_close(URLContext *h)
{
    return imem_callbacks.imem_close(h);
}

void imem_set_callbacks(IMemCallbacks *c)
{
    imem_callbacks = *c;
}

URLProtocol ff_imem_protocol = {
    .name                = "imem",
    .url_open            = imem_open,
    .url_read            = imem_read,
    .url_write           = imem_write,
    .url_seek            = imem_seek,
    .url_close           = imem_close,
    .url_get_file_handle = imem_get_handle,
    .url_check           = imem_check,
    .priv_data_size      = sizeof(IMemContext),
    .priv_data_class     = &imem_class,
};

#endif /* CONFIG_IMEM_PROTOCOL */
