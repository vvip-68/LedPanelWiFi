// ***************** ДЛЯ РАЗРАБОТЧИКОВ ******************
// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

uint16_t speed = 20;        // speed is set dynamically once we've started up
uint16_t scale = 30;        // scale is set dynamically once we've started up
uint8_t  *noise = nullptr;

CRGBPalette16 currentPalette( PartyColors_p );
uint8_t colorLoop = 1;
uint8_t ihue = 0;

void madnessNoise() {
  
  if (loadingFlag) {
    // modeCode = MC_NOISE_MADNESS;
    loadingFlag = false;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
  }
  
  uint8_t effectBrightness = getBrightnessCalculated(deviceBrightness, getEffectContrastValue(thisMode));
  scale = map8(getEffectScaleParamValue(MC_NOISE_MADNESS),0,100);
  fillnoise8();
  for (uint8_t i = 0; i < pWIDTH; i++) {
    for (uint8_t j = 0; j < pHEIGHT; j++) {
      CRGB thisColor = CHSV(noise[j * maxDim + i], 255, map8(noise[i * maxDim + j], effectBrightness / 2, effectBrightness));
      drawPixelXY(i, j, thisColor);
    }
  }
  
  ihue += 1;
}

void rainbowNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_RAINBOW;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = RainbowColors_p;
    colorLoop = 1;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_RAINBOW),0,100); 
  fillNoiseLED();
}

void rainbowStripeNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_RAINBOW_STRIP;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = RainbowStripeColors_p;
    colorLoop = 1;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_RAINBOW_STRIP),0,100); 
  fillNoiseLED();
}

void zebraNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_ZEBRA;
    // 'black out' all 16 palette entries...
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    colorLoop = 1;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_ZEBRA),0,100); 
  fillNoiseLED();
}

void forestNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_FOREST;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = ForestColors_p;
    colorLoop = 0;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_FOREST),0,100); 
  fillNoiseLED();
}

void oceanNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_OCEAN;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = OceanColors_p;
    colorLoop = 0;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_OCEAN),0,100); 
  fillNoiseLED();
}

void plasmaNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_PLASMA;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = PartyColors_p;
    colorLoop = 1;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_PLASMA),0,100); 
  fillNoiseLED();
}

void cloudNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_CLOUD;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = CloudColors_p;
    colorLoop = 0;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_CLOUD),0,100); 
  fillNoiseLED();
}

void lavaNoise() {
  if (loadingFlag) {
    loadingFlag = false;
    // modeCode = MC_NOISE_LAVA;
    createNoise();
    if (noise == nullptr) {
      // Если недостаточно памяти под эффект - перейти к другому эффекту;
      setRandomMode();
      return;      
    }        
    currentPalette = LavaColors_p;
    colorLoop = 0;
  }
  scale = map8(getEffectScaleParamValue(MC_NOISE_LAVA),0,100); 
  fillNoiseLED();
}

// ******************* СЛУЖЕБНЫЕ *******************
void fillNoiseLED() {
  uint8_t dataSmoothing = 0;
  if ( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint8_t i = 0; i < maxDim; i++) {
    uint16_t ioffset = scale * i;
    for (uint8_t j = 0; j < maxDim; j++) {
      uint16_t joffset = scale * j;

      uint8_t data = inoise8(x + ioffset, y + joffset, z);

      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if ( dataSmoothing ) {
        uint8_t olddata = noise[i * maxDim + j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i * maxDim + j] = data;
    }
  }
  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  uint8_t effectBrightness = getBrightnessCalculated(deviceBrightness, getEffectContrastValue(thisMode));

  for (uint8_t i = 0; i < pWIDTH; i++) {
    for (uint8_t j = 0; j < pHEIGHT; j++) {
      uint8_t index = noise[j * maxDim + i];
      uint8_t bri =   noise[i * maxDim + j];
      // if this palette is a 'loop', add a slowly-changing base value
      if ( colorLoop) {
        index += ihue;
      }
      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if ( bri > map8(effectBrightness,0,127) ) { 
        bri = effectBrightness; // 255;
      } else {
        bri = dim8_raw( bri * 2);
      }
      CRGB color = ColorFromPalette( currentPalette, index, bri);      
      drawPixelXY(i, j, color);
    }
  }
  ihue += 1;
}

void fillnoise8() {
  for (uint8_t i = 0; i < maxDim; i++) {
    int16_t ioffset = scale * i;
    for (uint8_t j = 0; j < maxDim; j++) {
      int16_t joffset = scale * j;
      noise[i * maxDim + j] = inoise8(x + ioffset, y + joffset, z);
    }
  }
  z += speed;
}

void createNoise() {
  if (noise == nullptr) { 
    noise = new uint8_t[maxDim * maxDim]; 
    if (noise == nullptr) {
      DEBUGLN(F("createNoise() - недостаточно памяти для эффекта"));
      setRandomMode();
    }
  }    
}

void releaseNoise() {
  if (noise != nullptr) { 
    delete[] noise; 
    noise = nullptr; 
  }  
}
