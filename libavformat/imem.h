/*
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

#ifndef AVFORMAT_IMEM_H
#define AVFORMAT_IMEM_H

#include "libavformat/url.h"

typedef struct IMemContext {
    const AVClass *class;
    int fd;
    int trunc;
    int blocksize;
} IMemContext;

typedef struct IMemCallbacks {
    int     (*imem_open)( URLContext *h, const char *url, int flags);
    /**
     * Read data from the protocol.
     * If data is immediately available (even less than size), EOF is
     * reached or an error occurs (including EINTR), return immediately.
     * Otherwise:
     * In non-blocking mode, return AVERROR(EAGAIN) immediately.
     * In blocking mode, wait for data/EOF/error with a short timeout (0.1s),
     * and return AVERROR(EAGAIN) on timeout.
     * Checking interrupt_callback, looping on EINTR and EAGAIN and until
     * enough data has been read is left to the calling function; see
     * retry_transfer_wrapper in avio.c.
     */
    int     (*imem_read)( URLContext *h, unsigned char *buf, int size);
    int     (*imem_write)(URLContext *h, const unsigned char *buf, int size);
    int64_t (*imem_seek)( URLContext *h, int64_t pos, int whence);
    int     (*imem_close)(URLContext *h);
    int     (*imem_get_handle)(URLContext *h);
    int     (*imem_check)(URLContext *h, int mask);
} IMemCallbacks;

/**
 * Initialize callback handlers for IMEM protocol
 *
 * @param c IMemCallbacks, structure with callback handlers
 */
void imem_set_callbacks(IMemCallbacks *c);

#endif /* AVFORMAT_IMEM_H */
