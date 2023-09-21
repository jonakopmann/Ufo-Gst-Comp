#include <ufo/ufo.h>
#include <iostream>
#include <chrono>

#define WIDTH 512u
#define HEIGHT 512u
#define NUMBER 1000u

/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData
{
    UfoTaskGraph* graph;
    UfoPluginManager* manager;
    UfoBaseScheduler* scheduler;

    UfoTaskNode* memory_in;
    UfoTaskNode* flip;
    UfoTaskNode* memory_out;
} CustomData;

static void
check_error(GError** error)
{
    if (*error != NULL)
    {
        g_error((*error)->message);
    }
}

int
main(int argc, char* argv[])
{
    CustomData data;
    GError* error = NULL;

    /* Initialize cumstom data structure */
    memset(&data, 0, sizeof(data));

    /* Initialize Ufo */
    data.graph = UFO_TASK_GRAPH(ufo_task_graph_new());
    data.manager = ufo_plugin_manager_new();

    /* Create the tasks */
    data.memory_in = ufo_plugin_manager_get_task(data.manager, "memory-in", &error);
    check_error(&error);
    data.flip = ufo_plugin_manager_get_task(data.manager, "flip", &error);
    check_error(&error);
    data.memory_out = ufo_plugin_manager_get_task(data.manager, "memory-out", &error);
    check_error(&error);

    gpointer buffer = g_malloc0(WIDTH * HEIGHT * NUMBER);
    for (guint32 i = 0; i < NUMBER; i++)
    {
        memset((gint8*)buffer + i * WIDTH * HEIGHT, 0xFF, WIDTH * HEIGHT / 2);
    }

    /* Configure memory-in */
    g_object_set(G_OBJECT(data.memory_in),
        "pointer", buffer,
        "width", WIDTH,
        "height", HEIGHT,
        "number", NUMBER,
        "bitdepth", sizeof(guint8) * 8,
        NULL);

    /* Configure flip */
    g_object_set(G_OBJECT(data.flip),
        "direction", 1,
        NULL);

    gpointer outBuffer = g_malloc(WIDTH * HEIGHT * NUMBER * 8);
    /* Configure memory-out */
    g_object_set(G_OBJECT(data.memory_out),
        "pointer", outBuffer,
        "max-size", WIDTH * HEIGHT * NUMBER * 8,
        NULL);

    /* Connect tasks in graph */
    ufo_task_graph_connect_nodes(data.graph, data.memory_in, data.flip);
    ufo_task_graph_connect_nodes(data.graph, data.flip, data.memory_out);

    /* Run graph */
    data.scheduler = ufo_scheduler_new();
    auto t1 = std::chrono::high_resolution_clock::now();

    ufo_base_scheduler_run(data.scheduler, data.graph, &error);

    auto t2 = std::chrono::high_resolution_clock::now();

    check_error(&error);

    g_free(buffer);

    /* Destroy all objects */
    g_object_unref(data.memory_in);
    g_object_unref(data.flip);
    g_object_unref(data.memory_out);
    g_object_unref(data.graph);
    g_object_unref(data.scheduler);
    g_object_unref(data.manager);

    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "Flipping " << NUMBER << ' ' << HEIGHT << 'x' << WIDTH << " images took " << ms_int.count() << " milliseconds" << std::endl;

    return 0;
}