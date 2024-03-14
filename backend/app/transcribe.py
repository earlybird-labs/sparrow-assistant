import os
import json
from dotenv import load_dotenv
from deepgram import DeepgramClient, PrerecordedOptions, FileSource

load_dotenv()

API_KEY = os.getenv("DEEPGRAM_API_KEY")


def transcribe_audio_to_json(input_filepath: str, output_filepath: str):
    """
    Transcribes an audio file to text and saves the result in a JSON file.

    :param input_filepath: Path to the input audio file (.wav).
    :param output_filepath: Path to the output JSON file.
    """
    try:
        # Create a Deepgram client using the API key
        deepgram = DeepgramClient(API_KEY)

        with open(input_filepath, "rb") as file:
            buffer_data = file.read()

        payload: FileSource = {
            "buffer": buffer_data,
        }

        # Configure Deepgram options for audio analysis
        options = PrerecordedOptions(
            model="nova-2",
            smart_format=True,
            diarize=True,
            punctuate=True,
        )

        # Call the transcribe_file method with the text payload and options
        response = deepgram.listen.prerecorded.v("1").transcribe_file(payload, options)

        json_response = json.loads(response.to_json())

        # Convert the response to JSON and save it to the specified file with an indentation of 2
        with open(output_filepath, "w") as json_file:
            json.dump(json_response, json_file, indent=2)

    except Exception as e:
        print(f"Exception: {e}")


# Example usage
if __name__ == "__main__":
    input_filepath = "example.wav"
    output_filepath = "transcription.json"
    transcribe_audio_to_json(input_filepath, output_filepath)
