guard-%:
	@ if [ "${${*}}" = "" ]; then \
		echo "Variable $* is not set"; \
		exit 1; \
	fi

CHARGER_IMG_BASE_WIDTH = 120
CHARGER_IMG_BASE_HEIGHT = 186

CHARGER_IMG_WIDTH = $(shell echo $$(( $(CHARGER_IMG_BASE_WIDTH) * $(RES_FACTOR) )) )
CHARGER_IMG_HEIGHT = $(shell echo $$(( $(CHARGER_IMG_BASE_HEIGHT) * $(RES_FACTOR) )) )

CC = gcc
CFLAGS =

IN_DIR = res
OUT_DIR = out
SCALE_DIR = $(OUT_DIR)/scale
STRETCH_DIR = $(OUT_DIR)/stretch
COMBINE_DIR = $(OUT_DIR)/combine
FINAL_DIR = $(OUT_DIR)/final
IN_FILES = $(wildcard $(IN_DIR)/*.png)
SCALE_FILES = $(patsubst $(IN_DIR)/%.png,$(SCALE_DIR)/$(RES_DIR)/%.png,$(IN_FILES))
STRETCH_FILES = $(sort $(patsubst $(IN_DIR)/%.png,$(STRETCH_DIR)/$(RES_DIR)/%.png,$(IN_FILES)))

CHARGER_IMG_COUNT = $(words $(IN_FILES))
CHARGER_IMG_STRETCH_HEIGHT = $(shell echo $$(( $(CHARGER_IMG_COUNT) * $(CHARGER_IMG_HEIGHT) )) )

all: guard-RES_DIR guard-RES_FACTOR $(FINAL_DIR)/$(RES_DIR)/lineage_battery_scale.png

$(COMBINE_DIR)/$(RES_DIR)/combine.png: $(STRETCH_FILES)
	mkdir -p $(COMBINE_DIR)/$(RES_DIR)
	convert $^ -size $(CHARGER_IMG_WIDTH)x$(CHARGER_IMG_STRETCH_HEIGHT) -depth 8 -alpha deactivate -fx 'u[j%n]' $@

$(FINAL_DIR)/$(RES_DIR)/lineage_battery_scale.png: $(COMBINE_DIR)/$(RES_DIR)/combine.png
	mkdir -p $(FINAL_DIR)/$(RES_DIR)
	pngcrush -text b 'Frames' $(CHARGER_IMG_COUNT) $< $@

$(STRETCH_DIR)/$(RES_DIR)/%.png: $(SCALE_DIR)/$(RES_DIR)/%.png
	mkdir -p $(STRETCH_DIR)/$(RES_DIR)
	convert -scale $(CHARGER_IMG_WIDTH)x$(CHARGER_IMG_STRETCH_HEIGHT)! $< $@

$(SCALE_DIR)/$(RES_DIR)/%.png: $(IN_DIR)/%.png
	mkdir -p $(SCALE_DIR)/$(RES_DIR)
	convert -resize $(CHARGER_IMG_WIDTH)x$(CHARGER_IMG_HEIGHT)! $< $@

.PHONY: all
