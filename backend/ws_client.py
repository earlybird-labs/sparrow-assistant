import websocket
import wave
import threading
import time


class AudioWebSocketClient:
    def __init__(self, ws_url):
        self.ws_url = ws_url
        self.ws = None
        self.is_listening = False
        self.file_index = 0
        self.wav_file = None
        self.lock = threading.Lock()
        self.last_speaking_time = time.time()
        self.silence_threshold = 5  # Initial silence threshold
        self.buffer_time = 2  # Additional buffer time to prevent early cutoff

    def on_message(self, ws, message):
        with self.lock:
            current_time = time.time()
            if message == "START_SPEAKING":
                if not self.is_listening:
                    self.is_listening = True
                    self.open_new_file()
                self.last_speaking_time = current_time
            elif message == "STOP_SPEAKING":
                # Wait for buffer time before actually stopping
                if current_time - self.last_speaking_time >= self.buffer_time:
                    self.is_listening = False
                    self.close_file()
            else:
                if self.is_listening and self.wav_file:
                    self.wav_file.writeframes(message)
                    self.last_speaking_time = current_time  # Update last speaking time on receiving audio data

    def check_silence(self):
        while True:
            time.sleep(
                1
            )  # Check every second instead of every `silence_threshold` to be more responsive
            with self.lock:
                if (
                    time.time() - self.last_speaking_time
                    > self.silence_threshold + self.buffer_time
                    and self.is_listening
                ):
                    self.is_listening = False
                    self.close_file()

    def on_error(self, ws, error):
        print(f"Error: {error}")

    def on_close(self, ws, close_status_code, close_msg):
        print("### closed ###")
        self.close_file()

    def on_open(self, ws):
        print("WebSocket opened")

    def open_new_file(self):
        self.close_file()  # Ensure the previous file is closed
        self.file_index += 1
        file_name = f"wav/audio_stream.wav"
        self.wav_file = wave.open(file_name, "wb")
        self.wav_file.setnchannels(1)
        self.wav_file.setsampwidth(2)
        self.wav_file.setframerate(44100)

    def close_file(self):
        if self.wav_file:
            self.wav_file.close()
            self.wav_file = None

    def run(self):
        self.ws = websocket.WebSocketApp(
            self.ws_url,
            on_message=self.on_message,
            on_error=self.on_error,
            on_close=self.on_close,
        )
        self.ws.on_open = self.on_open
        threading.Thread(target=self.check_silence).start()
        self.ws.run_forever()


if __name__ == "__main__":
    client = AudioWebSocketClient("ws://192.168.4.142:8888")
    client.run()
