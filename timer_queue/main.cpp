#include "precompiled.h"
#include "xutils/timer_queue.h"

int main()
{
    using namespace std::literals::chrono_literals;

    baio::io_context io_ctx;

    xutils::timer_queue tq{io_ctx};
    tq.enqueue_callback(7s, [] { std::printf("The third callback\n"); });
    tq.enqueue_callback(10s, [] { std::printf("The forth callback\n"); });
    tq.enqueue_callback(10s, [&] {
        std::printf("The fifth callback\n");
        tq.remove_all();
    });
    tq.enqueue_callback(11s, [] { std::printf("No called callback\n"); });
    tq.enqueue_callback(5s, [&] {
        std::printf("The first callback\n");
        tq.enqueue_callback(1s, [] { std::printf("The second callback\n"); });
    });

    io_ctx.run();

    return 0;
}
