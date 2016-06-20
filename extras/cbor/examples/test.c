#include "cbor.h"
#include <stdio.h>
#ifndef CBOR_NO_CTIME
#include <time.h>
#endif /* CBOR_NO_CTIME */

static unsigned char stream_data[1024];
static cbor_stream_t stream = {stream_data, sizeof(stream_data), 0};

static cbor_stream_t empty_stream = {NULL, 0, 0}; /* stream that is not large enough */

static unsigned char invalid_stream_data[] = {0x40}; /* empty string encoded in CBOR */
static cbor_stream_t invalid_stream = {invalid_stream_data, sizeof(invalid_stream_data),
                                sizeof(invalid_stream_data)
                               };

int main ()
{
    cbor_clear(&stream);

    cbor_serialize_int(&stream, 1);
    cbor_serialize_uint64_t(&stream, 2llu);
    cbor_serialize_int64_t(&stream, 3);
    cbor_serialize_int64_t(&stream, -5);
    cbor_serialize_bool(&stream, true);
#ifndef CBOR_NO_FLOAT
    cbor_serialize_float_half(&stream, 1.1f);
    cbor_serialize_float(&stream, 1.5f);
    cbor_serialize_double(&stream, 2.0);
#endif /* CBOR_NO_FLOAT */
    cbor_serialize_byte_string(&stream, "abc");
    cbor_serialize_unicode_string(&stream, "def");

    cbor_serialize_array(&stream, 2);
    cbor_serialize_int(&stream, 0);
    cbor_serialize_int(&stream, 1);

    cbor_serialize_array_indefinite(&stream);
    cbor_serialize_int(&stream, 10);
    cbor_serialize_int(&stream, 11);
    cbor_write_break(&stream);

    cbor_serialize_map(&stream, 2);
    cbor_serialize_int(&stream, 1);
    cbor_serialize_byte_string(&stream, "1");
    cbor_serialize_int(&stream, 2);
    cbor_serialize_byte_string(&stream, "2");

    cbor_serialize_map_indefinite(&stream);
    cbor_serialize_int(&stream, 10);
    cbor_serialize_byte_string(&stream, "10");
    cbor_serialize_int(&stream, 11);
    cbor_serialize_byte_string(&stream, "11");
    cbor_write_break(&stream);

#ifndef CBOR_NO_SEMANTIC_TAGGING
#ifndef CBOR_NO_CTIME
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    cbor_serialize_date_time(&stream, timeinfo);
    cbor_serialize_date_time_epoch(&stream, rawtime);
#endif /* CBOR_NO_CTIME */

    /* decoder should skip the tag and print 'unsupported' here */
    cbor_write_tag(&stream, 2);
    cbor_serialize_byte_string(&stream, "1");
#endif /* CBOR_NO_SEMANTIC_TAGGING */

    //cbor_stream_decode(&stream);

    printf("Hello CBOR!\n");

    return 0;
}
