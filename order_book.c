#include <stdio.h>
#include <stdlib.h>
#include "order_book.h"

void init_order_book(OrderBook *book)
{
    book->buy_orders = NULL;
    book->sell_orders = NULL;
    pthread_mutex_init(&book->lock, NULL);
}

Order *create_order(int id, OrderType type, double price, int quantity)
{
    Order *new_order = (Order *)malloc(sizeof(Order));
    if (!new_order)
    {
        perror("Failed to allocate memory for order");
        exit(1);
    }
    new_order->id = id;
    new_order->type = type;
    new_order->price = price;
    new_order->quantity = quantity;
    new_order->next = NULL;
    return new_order;
}

void add_order(OrderBook *book, Order *order)
{
    Order **head = (order->type == BUY) ? &book->buy_orders : &book->sell_orders;

    Order *old_head;
    do
    {
        old_head = *head;
        order->next = old_head;
    } while (!__atomic_compare_exchange_n(
        head,
        &old_head,
        order,
        0,
        __ATOMIC_SEQ_CST,
        __ATOMIC_SEQ_CST));
}

void print_order_book(const OrderBook *book)
{
    printf("Buy Orders:\n");
    Order *current = book->buy_orders;
    while (current)
    {
        printf("ID %d: Buy %d @ %.2f\n", current->id, current->quantity, current->price);
        current = current->next;
    }

    printf("\nSell Orders:\n");
    current = book->sell_orders;
    while (current)
    {
        printf("ID %d: Sell %d @ %.2f\n", current->id, current->quantity, current->price);
        current = current->next;
    }
}

void match_orders(OrderBook *book)
{
    while (book->buy_orders && book->sell_orders &&
           book->buy_orders->price >= book->sell_orders->price)
    {

        Order *buy = book->buy_orders;
        Order *sell = book->sell_orders;

        int trade_quantity = (buy->quantity < sell->quantity) ? buy->quantity : sell->quantity;
        double trade_price = sell->price;

        printf("Matched: Buy ID %d and Sell ID %d for %d units at $%.2f\n",
               buy->id, sell->id, trade_quantity, trade_price);

        buy->quantity -= trade_quantity;
        sell->quantity -= trade_quantity;

        if (buy->quantity == 0)
        {
            Order *temp = buy;
            book->buy_orders = buy->next;
            free(temp);
        }

        if (sell->quantity == 0)
        {
            Order *temp = sell;
            book->sell_orders = sell->next;
            free(temp);
        }
    }
}
