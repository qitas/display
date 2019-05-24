from PIL import Image
import time
import Adafruit_ILI9341 as TFT
import Adafruit_GPIO as GPIO
import Adafruit_GPIO.SPI as SPI


# Raspberry Pi configuration.
DC = 18
RST = 23
SPI_PORT = 0
SPI_DEVICE = 0

# BeagleBone Black configuration.
#DC = 'P9_15'
#RST = 'P9_12'
#SPI_PORT = 1
#SPI_DEVICE = 0

# Create TFT LCD display class.
disp = TFT.ILI9341(DC, rst=RST, spi=SPI.SpiDev(SPI_PORT, SPI_DEVICE, max_speed_hz=64000000))

# Initialize display.
disp.begin()

# Load an image.
print('Loading image...')
image = Image.open('cat.jpg')

# Resize the image and rotate it so it's 240x320 pixels.
image = image.rotate(90).resize((240, 320))

print('Press Ctrl-C to exit')
while(True):
    # Draw the image on the display hardware.
    print('Drawing image')
    start_time = time.time()
    disp.display(image)
    end_time = time.time()
    print('Time to draw image: ' + str(end_time - start_time))
    disp.clear((0, 0, 0))
    disp.display()
