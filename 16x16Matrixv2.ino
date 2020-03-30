#include <FastLED.h>
#include <EEPROM.h>
#include <SPI.h>
#define LED_PIN 6

#define WIDTH 16
#define HEIGHT 16
#define BRIGHTNESS 10

#define NUM_LEDS WIDTH *HEIGHT

uint16_t getPixelNumber(byte x, byte y)
{
    if (y % 2 == 0)
    {
        return (y * WIDTH + WIDTH - x - 1);
    }
    else
    {
        return (y * WIDTH + x);
    }
}

bool press1 = false;
bool press2 = false;
int prog = 1;
CRGB leds[NUM_LEDS];

void (*resetFunc)(void) = 0;

byte bytes[16][16];
byte b1, b2, b3;
bool f1, f2, f3;
uint16_t i1, i2;
byte BrightTimer;

void loop()
{
    //if (BrightTimer > 100 )
    //{
    //    FastLED.setBrightness((analogRead(4) * -1 + 255) / 2);
    //    BrightTimer = 0;
    //}
    BrightTimer++;
    if (digitalRead(3) == 1 && !press1)
    {
        prog += 1;
        EEPROM.write(0, prog);
        delay(200);
        resetFunc();
    }
    if (digitalRead(3) == 0 && press1)
        press1 = false;
    if (digitalRead(2) == 1 && !press2)
    {
        b1 += 1;
        if(b1 > 2)
        b1 = 0;
        delay(200);
    }
    if (digitalRead(3) == 0 && press2)
        press2 = false;
    if (prog > 2)
        prog = 0;
    switch (prog)
    {
    case 0:
        FastLED.clear();
        Equol();
        delay(1);
        break;
    case 1:
        FastLED.clear();
        Snake();
        delay(10);
        break;
    case 2:
        Tetris();
        delay(30);
        break;

    default:
        break;
    }
    FastLED.show();
}
void setup()
{
    for (byte i = 0; i < 16; i++)
    {
        for (byte ii = 0; ii < 16; ii++)
        {
            bytes[i][ii] = 0;
        }
    }
    b1 = 0;
    b2 = 0;
    b3 = 0;
    f1 = 0;
    f2 = 0;
    f3 = 0;
    i1 = 0;
    i2 = 0;
    Serial.begin(9600);

    pinMode(2, INPUT);
    pinMode(3, INPUT);

    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    //FastLED.setBrightness(analogRead(4) * -1 + 255);
    FastLED.setBrightness(25);
    FastLED.clear();
    randomSeed(analogRead(10) + analogRead(11));
    prog = EEPROM.read(0);
    if (prog == 2)
        addFigure();
}

#pragma region Equol
void Equol()
{
    for (byte i = 0; i <= 15; i++)
    {
        if (bytes[0][i] == bytes[1][i])
        {
            bytes[0][i] = random(15);
            //Serial.println(bytes[0][i]);
        }
        if (bytes[1][i] > bytes[0][i])
        {
            bytes[1][i] -= random(2);
            //Serial.println('-');
        }
        else if (bytes[1][i] < bytes[0][i])
        {
            bytes[1][i] += random(2);
            //Serial.println('+');
        }
        for (byte ii = 0; ii <= 15; ii++)
        {
            if (ii > bytes[1][i])
            switch (b1)
            {
            case 0:
                leds[getPixelNumber(i, ii)] = 0x882288;
                break;
            case 1:
                leds[getPixelNumber(i, ii)] = 0x880000;
                break;
            case 2:
                leds[getPixelNumber(i, ii)] = 0x000088;
                break;
            }
        }
    }
}
#pragma endregion

#pragma region Snake
#define SNAKECOLOR 0x888888
#define b1COLOR 0x0000FF
#define HEADECOLOR 0xFF0000
void Snake()
{
    if ((b1 & 240) >> 4 > (bytes[0][b3] & 240) >> 4)
    {
        b2 = 2;
    }
    else if ((b1 & 240) >> 4 < (bytes[0][b3] & 240) >> 4)
    {
        b2 = 3;
    }
    else
    {
        if (b1 & 0b00001111 < bytes[0][b3] & 0b00001111)
        {
            b2 = 1;
        }
        else
        {
            b2 = 0;
        }
    }
    if (bytes[0][b3] == b1)
    {
        switch (b2)
        {
        case 0:
            bytes[0][b3 + 1] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) + 0b00000001) & 0b00001111);
            break;
        case 1:
            bytes[0][b3 + 1] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) - 0b00000001) & 0b00001111);
            break;
        case 2:
            bytes[0][b3 + 1] = (((bytes[0][b3] & 0b11110000) + 0b00010000) & 0b11110000) | bytes[0][b3] & 0b00001111;
            break;
        case 3:
            bytes[0][b3 + 1] = (((bytes[0][b3] & 0b11110000) - 0b00010000) & 0b11110000) | bytes[0][b3] & 0b00001111;
            break;

        default:
            bytes[0][b3 + 1] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) + 1) & 0b00001111);
            break;
        }
        b3 += 1;
        drawSnakePx();
        b1 = random(256);
        return;
    }
    //перемещение
    for (byte i = 1; i <= b3; i++)
    {
        bytes[0][i - 1] = bytes[0][i];
    }
    switch (b2)
    {
    case 0:
        bytes[0][b3] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) + 1) & 0b00001111);
        break;
    case 1:
        bytes[0][b3] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) - 1) & 0b00001111);
        break;
    case 2:
        bytes[0][b3] = (((bytes[0][b3] & 0b11110000) + 0b00010000) & 0b11110000) | bytes[0][b3] & 0b00001111;
        break;
    case 3:
        bytes[0][b3] = (((bytes[0][b3] & 0b11110000) - 0b00010000) & 0b11110000) | bytes[0][b3] & 0b00001111;
        break;

    default:
        bytes[0][b3 + 1] = (bytes[0][b3] & 0b11110000) | (((bytes[0][b3] & 0b00001111) + 1) & 0b00001111);
        b3 += 1;
        break;
    }
    drawSnakePx();
    for (byte i = 0; i < b3; i++)
    {
        if (bytes[0][i] == bytes[0][b3])
        {
            resetFunc();
        }
    }
}
void drawSnakePx()
{
    leds[getPixelNumber((b1 & 0b11110000) >> 4, b1 & 0b00001111)] = b1COLOR;
    for (byte i = 0; i <= b3; i++)
    {
        byte x = (bytes[0][i] & 0b11110000) >> 4;
        byte y = bytes[0][i] & 0b00001111;
        leds[getPixelNumber(x, y)] = SNAKECOLOR;
        Serial.println(b2);
    }
    leds[getPixelNumber((bytes[0][b3] & 0b11110000) >> 4, bytes[0][b3] & 0b00001111)] = HEADECOLOR;
}
#pragma endregion

void Tetris()
{
    if (f1 == true)
    {
        addFigure();
        f1 = false;
        return;
    }
    for (byte i = 0; i < 16; i -= -1)
    {
        for (byte ii = 0; ii < 16; ii -= -1)
        {
            leds[getPixelNumber(i, ii)] = (bytes[i][ii] & 0b01111111) * 0x20408;
        }
    }
    for (byte i = 0; i < 16; i -= -1)
        if (bytes[i][0] != 0 && bytes[i][0] >> 7 == 0)
            resetFunc();
    for (byte i = 0; i < 16; i -= -1)
    {
        for (byte ii = 0; ii < 16; ii -= -1)
        {
            if (bytes[i][ii] >> 7 == 0b00000001)
                if (ii == 15 || (bytes[i][ii + 1] >> 7 == 0b00000000 && bytes[i][ii + 1] != 0))
                {
                    for (byte i = 0; i < 16; i -= -1)
                    {
                        for (byte ii = 0; ii < 16; ii -= -1)
                        {
                            bytes[i][ii] = bytes[i][ii] & 0b01111111;
                        }
                    }
                    f1 = true;
                    return;
                }
        }
    }
    for (byte i = 0; i < 16; i -= -1)
    {
        for (byte ii = 16; ii > 0; ii -= 1)
        {
            if (bytes[i][ii - 1] >> 7 == 1)
            {
                bytes[i][ii] = bytes[i][ii - 1];
                bytes[i][ii - 1] = 0;
            }
        }
    }
}
void addFigure()
{
    b2 = random(13);
    switch (random(6))
    {
    case 0:
        b1 = 0b11110000;
        break;
    case 1:
        b1 = 0b10001111;
        break;
    case 2:
        b1 = 0b11000011;
        break;
    case 3:
        b1 = 0b11101100;
        break;
    case 4:
        b1 = 0b11110100;
        break;
    case 5:
        b1 = 0b10100000;
        break;
    }
    switch (random(6))
    {
    case 0:
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 1][1] = b1;
        bytes[b2 + 1][2] = b1;
        bytes[b2][2] = b1;
        bytes[b2 + 2][2] = b1;
        break;
    case 1:
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 1][1] = b1;
        bytes[b2][0] = b1;
        bytes[b2][1] = b1;
        break;
    case 2:
        bytes[b2][0] = b1;
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 2][0] = b1;
        break;
    case 3:
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 1][1] = b1;
        bytes[b2 + 1][2] = b1;
        bytes[b2][2] = b1;
        break;
    case 4:
        bytes[b2][2] = b1;
        bytes[b2][0] = b1;
        bytes[b2][1] = b1;
        bytes[b2][3] = b1;
        break;
    case 5:
        bytes[b2][0] = b1;
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 1][1] = b1;
        bytes[b2 + 2][1] = b1;
        break;
    default:
        bytes[b2 + 1][0] = b1;
        bytes[b2 + 1][1] = b1;
        bytes[b2 + 1][2] = b1;
        bytes[b2][2] = b1;
        bytes[b2 + 2][2] = b1;
        break;
    }
}
