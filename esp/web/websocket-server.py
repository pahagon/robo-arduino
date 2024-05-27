# server.py
import asyncio
import websockets

async def handler(websocket, path):
    try:
        async for message in websocket:
            if message == "ping":
                await websocket.send("pong")
            else:
                print(f"Received message from client: {message}")
                response = f"Hello from server! #{message}"
                await websocket.send(response)
    except websockets.ConnectionClosed:
        print("Connection closed")

async def main():
    async with websockets.serve(handler, "localhost", 8765):
        await asyncio.Future()  # run forever

asyncio.run(main())
