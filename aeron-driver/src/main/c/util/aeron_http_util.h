/*
 * Copyright 2014-2019 Real Logic Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AERON_HTTP_UTIL_H
#define AERON_HTTP_UTIL_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "aeron_socket.h"
#include "aeron_parse_util.h"
#include "aeron_arrayutil.h"
#include "aeron_bitutil.h"

#define AERON_MAX_HTTP_USERINFO_LENGTH (384)
#define AERON_MAX_HTTP_PATH_AND_QUERY_LENGTH (512)

typedef struct aeron_http_parsed_url_stct
{
    char userinfo[AERON_MAX_HTTP_USERINFO_LENGTH];
    char host_and_port[AERON_MAX_HOST_LENGTH + 1 + AERON_MAX_PORT_LENGTH];
    char path_and_query[AERON_MAX_HTTP_PATH_AND_QUERY_LENGTH];
    struct sockaddr_storage address;
    int ip_version_hint;
}
aeron_http_parsed_url_t;

int aeron_http_parse_url(const char *url, aeron_http_parsed_url_t *parsed_url);

typedef struct aeron_http_response_stct
{
    char *buffer;
    size_t cursor;
    size_t body_offset;
    size_t length;
    size_t capacity;
    size_t response_code;
    size_t content_length;
    bool parse_err;
}
aeron_http_response_t;

#define AERON_HTTP_RESPONSE_INITIAL_BUFFER_CAPACITY (4 * 1024)
#define AERON_HTTP_RESPONSE_RECV_LENGTH (4 * 1024)
#define AERON_HTTP_MAX_HEADER_LENGTH (1024)

inline int aeron_http_response_ensure_capacity(aeron_http_response_t *response, size_t new_capacity)
{
    if (new_capacity > response->capacity)
    {
        new_capacity = aeron_find_next_power_of_two((int32_t)new_capacity);

        if (aeron_array_ensure_capacity((uint8_t **)&response->buffer, 1, response->capacity, new_capacity) < 0)
        {
            return -1;
        }

        response->capacity = new_capacity;
        return 0;
    }

    return 0;
}

inline void aeron_http_response_delete(aeron_http_response_t *response)
{
    if (NULL != response)
    {
        aeron_free(response->buffer);
        aeron_free(response);
    }
}

int aeron_http_retrieve(aeron_http_response_t **response, const char *url, int64_t timeout_ns);

#endif //AERON_HTTP_UTIL_H
