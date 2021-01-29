exit

# Install v4l2loopback [method #1]
sudo apt-get remove v4l2loopback-dkms
sudo apt-get install build-essential libelf-dev linux-headers-$(uname -r) unzip
wget https://github.com/umlaeute/v4l2loopback/archive/master.zip
unzip master.zip
cd v4l2loopback-master
make
sudo make install
sudo depmod -a
sudo modprobe v4l2loopback

# Install v4l2loopback [method #2]
sudo apt install v4l2loopback-dkms

# Insert module
sudo modprobe v4l2loopback

# List active devices
v4l2-ctl --list-devices

# List type, pixel format and name of each video device
v4l2-ctl --list-formats

# Stream image continuously
ffmpeg -re -loop 1 -i [filename].jpg -vf format=yuv420p -f v4l2 /dev/[loopback device]

# Stream video file
ffmpeg -re -i [filename].mp4 -map 0:v -f v4l2 /dev/[loopback device]

# Stream webcam
ffmpeg -f v4l2 -i /dev/[webcam file] -vf vflip -f v4l2 /dev/[loopback device]

## Preview with ffplay
ffplay /dev/video0