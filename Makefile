OUT_DIR = out

SUB_MAKE = make -f SingleResolution.mk

all: $(OUT_DIR)/ldpi/battery_scale.png \
	$(OUT_DIR)/mdpi/battery_scale.png \
	$(OUT_DIR)/hdpi/battery_scale.png \
	$(OUT_DIR)/xhdpi/battery_scale.png \
	$(OUT_DIR)/xxhdpi/battery_scale.png \
	$(OUT_DIR)/xxxhdpi/battery_scale.png

$(OUT_DIR)/ldpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=ldpi RES_FACTOR=3/4

$(OUT_DIR)/mdpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=mdpi RES_FACTOR=1

$(OUT_DIR)/hdpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=hdpi RES_FACTOR=3/2

$(OUT_DIR)/xhdpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=xhdpi RES_FACTOR=2

$(OUT_DIR)/xxhdpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=xxhdpi RES_FACTOR=3

$(OUT_DIR)/xxxhdpi/battery_scale.png:
	$(SUB_MAKE) RES_DIR=xxxhdpi RES_FACTOR=4


clean:
	rm -rf $(OUT_DIR) test

# Tool for testing the tExt chunks as read from android
# Other useful tools: pngcheck -cft; identify -verbose (imagemagick)
test: pngtest/test.c
	$(CC) $(CFLAGS) -lpng $< -o $@

.PHONY: all clean
