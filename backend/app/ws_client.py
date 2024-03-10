import websocket
import wave
import threading
import time
import os

from transcribe import transcribe_audio_to_json


class AudioWebSocketClient:
    def __init__(self, ws_url):
        self.ws_url = ws_url
        self.ws = None
        self.is_listening = False
        self.file_index = self.find_next_file_index()
        self.wav_file = None
        self.lock = threading.Lock()

    def on_message(self, ws, message):
        with self.lock:
            if message == "START_SPEAKING":
                if not self.is_listening:
                    self.is_listening = True
                    self.open_new_file()
            elif message == "STOP_SPEAKING":
                self.is_listening = False
                self.close_file()
                # Save the file immediately after stopping
            else:
                if self.is_listening and self.wav_file:
                    self.wav_file.writeframes(message)

    def on_error(self, ws, error):
        print(f"Error: {error}")

    def on_close(self, ws, close_status_code, close_msg):
        print("### closed ###")
        self.close_file()
        # Ensure the file is saved upon WebSocket closure

    def on_open(self, ws):
        print("WebSocket opened")

    def open_new_file(self):
        self.close_file()  # Ensure the previous file is closed
        self.file_index += 1
        file_name = f"tmp/audio/{self.file_index}.wav"

        self.wav_file = wave.open(file_name, "wb")
        self.wav_file.setnchannels(1)
        self.wav_file.setsampwidth(2)
        self.wav_file.setframerate(44100)

    def close_file(self):
        if self.wav_file:
            file_name = self.wav_file._file.name  # Get the current file name
            self.wav_file.close()
            self.wav_file = None
            self.transcribe_file_in_background(file_name)

    def transcribe_file_in_background(self, file_name):
        """
        Transcribes the audio file in the background and saves the transcription
        to a JSON file in the tmp/transcript directory.
        """
        output_file_name = f"{self.file_index}.json"
        output_file_path = os.path.join("tmp/transcript", output_file_name)

        # Ensure the output directory exists
        os.makedirs(os.path.dirname(output_file_path), exist_ok=True)

        # Start the transcription in a new thread
        threading.Thread(
            target=transcribe_audio_to_json, args=(file_name, output_file_path)
        ).start()

    def find_next_file_index(self):
        """
        Finds the next available file index by checking the tmp/audio directory.
        """
        files = os.listdir("tmp/audio")
        indices = [int(file.split(".")[0]) for file in files if file.endswith(".wav")]
        index = max(indices) + 1 if indices else 0
        print(f"Next file index: {index}")
        return index

    def run(self):
        self.ws = websocket.WebSocketApp(
            self.ws_url,
            on_message=self.on_message,
            on_error=self.on_error,
            on_close=self.on_close,
        )
        self.ws.on_open = self.on_open
        self.ws.run_forever()


if __name__ == "__main__":
    client = AudioWebSocketClient("ws://192.168.4.142:8888")
    client.run()
