#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    GQueue *queue;
    queue = g_queue_new();  // Create a new queue

    // Add elements to the queue
    g_queue_push_tail(queue, "First");
    g_queue_push_tail(queue, "Second");
    g_queue_push_tail(queue, "Third");

    // Remove and print the elements from the head of the queue
    while (!g_queue_is_empty(queue)) {
        gpointer element = g_queue_pop_head(queue);
        printf("Removed: %s\n", (char*)element);
    }

    // Free the queue (and its elements)
    g_queue_free(queue);

    return 0;
}

