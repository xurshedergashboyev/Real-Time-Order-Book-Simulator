import websocket
import json

def on_open(ws):
    print("Connected to Coinbase BTC-USD feed", flush=True)
    subscribe_message = {
        "type": "subscribe",
        "product_ids": ["BTC-USD"],
        "channels": ["matches"]
    }
    ws.send(json.dumps(subscribe_message))

def on_message(ws, message):
    data = json.loads(message)
    if data['type'] == 'match':
        price = float(data['price'])
        quantity = float(data['size'])
        side = data['side'].upper()
        print(f"{side} {price} {quantity}", flush=True)

def on_error(ws, error):
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### Closed ###")

if __name__ == "__main__":
    url = "wss://ws-feed.exchange.coinbase.com"
    ws = websocket.WebSocketApp(url,
                                on_open=on_open,
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close)

    ws.run_forever()
