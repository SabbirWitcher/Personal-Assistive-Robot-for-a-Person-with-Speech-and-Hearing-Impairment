# Personal-Assistive-Robot-for-a-Person-with-Speech-and-Hearing-Impairment
This project designs and implements a personal assistive robot that can follow a person with speech and hearing impairment and can convert his sign language to audio and a normal person's speech to text so that the person with impairment can communicate properly in Bengali.

The project uses a custom dataset trained on 5 common words that a person may need assist with to train the model. You can use the ipynb file to generate your own custom dataset based on any words you like and can map that to Bengali words and then train your own model and deploy it inside the translator.py file to perform the inference and to perform real time sign language to speech conversion that can be heard in the speaker.

The trained model is a TFLite model to reduce the computation cost and to make it runnable on a Raspberry Pi Zero 2W. The model continuously monitors the feed from the camera connected to the Pi and performs sign to natural language conversion in Bengali.

The ESP32 code locks onto the person and then tracks and follows him so that he can be always monitored in times of need.
