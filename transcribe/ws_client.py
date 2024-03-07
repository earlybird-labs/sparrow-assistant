import websocket
import wave
import time
import threading

# Global variables to manage audio data and timing
audio_frames = []
start_time = None


def save_audio():
    """Saves the audio data to a WAV file every 10 seconds."""
    global audio_frames, start_time
    while True:
        if time.time() - start_time >= 10:
            with wave.open(f"audio/audio_{int(time.time())}.wav", "wb") as wf:
                wf.setnchannels(1)
                wf.setsampwidth(2)
                wf.setframerate(44100)
                wf.writeframes(b"".join(audio_frames))
            audio_frames = []  # Reset the audio data
            start_time = time.time()  # Reset the timer


def on_message(ws, message):
    """Handles incoming messages (audio data) and appends them to the global audio_frames list."""
    global audio_frames, start_time
    if start_time is None:
        start_time = time.time()
        # Start the thread to save audio every 10 seconds
        threading.Thread(target=save_audio, daemon=True).start()
    audio_frames.append(message)


def on_error(ws, error):
    """Handles any errors that occur."""
    print(f"Error: {error}")


def on_close(ws, close_status_code, close_msg):
    """Handles the closing of the WebSocket connection."""
    print("### closed ###")


if __name__ == "__main__":
    # websocket.enableTrace(True)  # Enable logging to help debug
    ws = websocket.WebSocketApp(
        "ws://localhost:8888",
        on_message=on_message,
        on_error=on_error,
        on_close=on_close,
    )

    ws.run_forever()
