This was tested on a ThinkPad P70 laptop with an Intel integrated graphics and an NVIDIA GPU:
```
lspci | egrep 'VGA|3D'
00:02.0 VGA compatible controller: Intel Corporation Device 191b (rev 06)
01:00.0 VGA compatible controller: NVIDIA Corporation GM204GLM [Quadro M3000M] (rev a1)
```

A reason to use the integrated graphics for display is if installing the NVIDIA drivers causes the display to stop working properly.
In my case, [Ubuntu would get stuck in a login loop after installing the NVIDIA drivers](http://askubuntu.com/questions/223501/ubuntu-gets-stuck-in-a-login-loop).
This happened regardless if I installed the drivers from the "Additional Drivers" tab in "System Settings" or the `ppa:graphics-drivers/ppa` in the command-line.

The solution is described in the answer from [this post](http://askubuntu.com/questions/779530/how-to-configure-igpu-for-xserver-and-nvidia-gpu-for-cuda).

## Install the NVIDIA drivers
Download the driver installation runfile from their website (e.g. [the 375.26 driver runfile](http://www.nvidia.com/Download/driverResults.aspx/112992/en-us)).
Run the script with the option `--no-opengl-files`:
```
sudo ./NVIDIA-Linux-x86_64-375.26.run --no-opengl-files
```
Reboot.

## Install CUDA
Download the CUDA installation runfile from [their website](https://developer.nvidia.com/cuda-downloads).
It's important to download the "runfile (local)" file so that we can explicitly prevent from overwriting the driver that was just installed.
Run the script:
```
sudo ./cuda_8.0.44_linux.run 
```
Respond "no" when asked "Install NVIDIA accelerated Graphics Driver for Linux-x86_65 375.26?".

## Configure xorg.conf
Modify or create the file `/etx/X11/xorg.conf` to specify that the NVIDIA GPU should be used for a secondary screen so that it has entries in `nvidia-settings`.
Mine looks like this:
```
Section "ServerLayout"
    Identifier "layout"
    Screen 0 "intel"
    Screen 1 "nvidia"
EndSection

Section "Device"
    Identifier "intel"
    Driver "intel"
    BusID "PCI:0@0:2:0"
    Option "AccelMethod" "SNA"
EndSection

Section "Screen"
    Identifier "intel"
    Device "intel"
EndSection

Section "Device"
    Identifier "nvidia"
    Driver "nvidia"
    BusID "PCI:1@0:0:0"
    Option "ConstrainCursor" "off"
EndSection

Section "Screen"
    Identifier "nvidia"
    Device "nvidia"
    Option "AllowEmptyInitialConfiguration" "on"
    Option "IgnoreDisplayDevices" "CRT"
EndSection
```
The `BusID` should match what the `lspci` command above returned.

Reboot.

## Miscellaneous tips
If installing the NVIDIA drivers messes up the OS (e.g. it gets stuck in a login loop or it shows a black screen before the login step), you can uninstall the drivers by openning a console with Ctrl-Alt-F1 and running:
```
sudo apt-get remove --purge nvidia-*
sudo apt-get autoremove
echo "nouveau" | sudo tee -a /etc/modules
```
Reboot.
