#include <glib.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    GQueue *queue;
    queue = g_queue_new();  // Create a new queue

    // Add elements to the queue
    g_queue_push_tail(queue, "First");
    g_queue_push_tail(queue, "Second");
    g_queue_push_tail(queue, "Third");

    // Print the elements in the queue
    GList *iter = queue->head;
    while (iter != NULL) {
        printf("%s\n", (char*)iter->data);
        printf("%p\n", (char*)iter);
        iter = iter->next;
    }

    // Free the queue (and its elements)
    g_queue_free(queue);

    return 0;
}

