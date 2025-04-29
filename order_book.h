#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <pthread.h>

typedef enum
{
    BUY,
    SELL
} OrderType;

typedef struct Order
{
    int id;
    OrderType type;
    double price;
    int quantity;
    struct Order *next;
} Order;

typedef struct
{
    Order *buy_orders;
    Order *sell_orders;
    pthread_mutex_t lock;
} OrderBook;

void init_order_book(OrderBook *book);
Order *create_order(int id, OrderType type, double price, int quantity);
void add_order(OrderBook *book, Order *order);
void print_order_book(const OrderBook *book);
void match_orders(OrderBook *book);

#endif