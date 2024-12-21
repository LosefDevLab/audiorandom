echo "[INFO]Audio Random (ver1.0.r1.b6) Install Shell"
echo "[Install]Apt-installing the lib and g++..."
sudo apt install portaudio19-dev
echo "[G++]Compile the AUR..."
g++ -o audio_random audio_random.cpp -lportaudio -lpthread
echo "[G++]Completed!"
echo "[Install]OK!./audio_random to use it!"