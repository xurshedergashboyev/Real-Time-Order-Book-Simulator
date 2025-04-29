#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "order_book.h"

long total_latency_ns = 0;
int total_orders = 0;
pthread_mutex_t latency_lock;

long current_time_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

double ns_to_ms(long ns)
{
    return (double)ns / 1e6;
}

void *client_thread(void *arg)
{
    OrderBook *book = (OrderBook *)arg;

    for (int i = 0; i < 10; i++)
    {
        int id = rand() % 10000;
        OrderType type = (rand() % 2 == 0) ? BUY : SELL;
        double price = 95.0 + (rand() % 1000) / 10.0;
        int quantity = (rand() % 10) + 1;

        Order *new_order = create_order(id, type, price, quantity);

        long start = current_time_ns();

        add_order(book, new_order);

        pthread_mutex_lock(&book->lock);
        match_orders(book);
        pthread_mutex_unlock(&book->lock);

        long end = current_time_ns();
        long latency = end - start;

        pthread_mutex_lock(&latency_lock);
        total_latency_ns += latency;
        total_orders++;
        pthread_mutex_unlock(&latency_lock);

        usleep(100000);
    }
    return NULL;
}

int main()
{
    OrderBook book;
    init_order_book(&book);

    pthread_mutex_init(&latency_lock, NULL);

    srand(time(NULL));

    char line[256];
    int order_id = 0;

    printf("Waiting for real-time trade data...\n");

    while (fgets(line, sizeof(line), stdin))
    {
        char *side = strtok(line, " ");
        char *price_str = strtok(NULL, " ");
        char *quantity_str = strtok(NULL, " ");

        if (!side || !price_str || !quantity_str)
            continue;

        double price = atof(price_str);
        int quantity = (int)(atof(quantity_str) * 1000);

        OrderType type = (strcmp(side, "BUY") == 0) ? BUY : SELL;

        Order *order = create_order(order_id++, type, price, quantity);

        long start = current_time_ns();

        pthread_mutex_lock(&book.lock);
        add_order(&book, order);
        match_orders(&book);
        pthread_mutex_unlock(&book.lock);

        long end = current_time_ns();
        long latency = end - start;

        pthread_mutex_lock(&latency_lock);
        total_latency_ns += latency;
        total_orders++;
        pthread_mutex_unlock(&latency_lock);

        if (order_id % 50 == 0)
        {
            double avg_latency_us = (double)total_latency_ns / total_orders / 1000.0;
            printf("\nProcessed %d orders | Avg latency: %.2f us\n", total_orders, avg_latency_us);
        }
    }

    return 0;
}