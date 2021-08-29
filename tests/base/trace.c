#include <uuki/base/trace.h>

#include <rexo.h>

struct dummy {
    int value;
};

struct dummy_trace_data {
    int count;
};

W_TRACER_INTIALIZE(dummy_tracer, struct dummy_trace_data, 128);

RX_TEST_CASE(trace, main)
{
    struct dummy dummy_1;
    struct dummy dummy_2;
    struct dummy_trace_data *trace_data;

    W_TRACER_ATTACH_DATA(
        dummy_tracer,
        &dummy_1,
        (
            &(struct dummy_trace_data) {
                .count = 123,
            }
        )
    );
    W_TRACER_ATTACH_DATA(
        dummy_tracer,
        &dummy_2,
        (
            &(struct dummy_trace_data) {
                .count = 234,
            }
        )
    );

    W_TRACER_FIND_DATA(dummy_tracer, &trace_data, &dummy_1);
    RX_INT_REQUIRE_EQUAL(trace_data->count, 123);

    W_TRACER_FIND_DATA(dummy_tracer, &trace_data, &dummy_2);
    RX_INT_REQUIRE_EQUAL(trace_data->count, 234);

    W_TRACER_DETACH_DATA(dummy_tracer, &dummy_1);
    W_TRACER_DETACH_DATA(dummy_tracer, &dummy_2);
}

int
main(
    int argc,
    const char **argv
)
{
    return rx_main(0, NULL, argc, argv) == RX_SUCCESS ? 0 : 1;
}
