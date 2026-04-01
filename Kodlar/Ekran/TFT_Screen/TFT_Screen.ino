#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define TS_MINX 120
#define TS_MINY 70
#define TS_MAXX 900
#define TS_MAXY 920
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
#define CYAN     0x07FF
#define GRAY     0x8410
#define KOYU_GRI 0x2104

#define EKRAN_W   240
#define EKRAN_H   320

bool klavyeGoruntu = false;   // ← Başlangıçta KAPALI

// ── Klavye kapalıyken kutu boyutları ─────────────────
#define GELEN_Y_BUYUK   0
#define GELEN_H_BUYUK   220    // Büyük gelen kutusu
#define METIN_Y_BUYUK   225
#define METIN_H_BUYUK   35

// ── Klavye açıkken kutu boyutları ────────────────────
#define GELEN_Y_KUCUK   0
#define GELEN_H_KUCUK   35
#define METIN_Y_KUCUK   38
#define METIN_H_KUCUK   35

// KLAVYE butonu (klavye kapalıyken altta)
#define KBD_BTN_X  70
#define KBD_BTN_Y  270
#define KBD_BTN_W  100
#define KBD_BTN_H  40

// Klavye
#define KLAVYE_Y     80
#define TUS_W        32
#define TUS_H        44
#define TUS_BOSLUK   2
#define SATIR_BOSLUK 4

#define METIN_MAX  30
#define TUS_SAYISI 30
#define CURSOR_ARALIK 500

String metin = "";
unsigned long sonCursorZaman = 0;
bool cursorGorunum = true;

// Aktif kutu boyutları (başlangıçta büyük)
int gelenY  = GELEN_Y_BUYUK;
int gelenH  = GELEN_H_BUYUK;
int metinY  = METIN_Y_BUYUK;
int metinH  = METIN_H_BUYUK;

struct Tus {
  char     label[6];
  int      x, y, w, h;
  uint16_t renk;
};

Tus tuslar[TUS_SAYISI];

// ── Tuşları oluştur ───────────────────────────────────
void tuslariOlustur() {
  const int adim = TUS_H + SATIR_BOSLUK;

  int satirY[5];
  for (int i = 0; i < 5; i++)
    satirY[i] = KLAVYE_Y + i * adim;

  const char harfler[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int idx = 0;

  for (int s = 0; s < 7; s++, idx++) {
    tuslar[idx].label[0] = harfler[s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
    tuslar[idx].y = satirY[0];
    tuslar[idx].w = TUS_W;
    tuslar[idx].h = TUS_H;
    tuslar[idx].renk = BLUE;
  }
  for (int s = 0; s < 7; s++, idx++) {
    tuslar[idx].label[0] = harfler[7 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
    tuslar[idx].y = satirY[1];
    tuslar[idx].w = TUS_W;
    tuslar[idx].h = TUS_H;
    tuslar[idx].renk = BLUE;
  }
  for (int s = 0; s < 7; s++, idx++) {
    tuslar[idx].label[0] = harfler[14 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
    tuslar[idx].y = satirY[2];
    tuslar[idx].w = TUS_W;
    tuslar[idx].h = TUS_H;
    tuslar[idx].renk = BLUE;
  }
  for (int s = 0; s < 5; s++, idx++) {
    tuslar[idx].label[0] = harfler[21 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
    tuslar[idx].y = satirY[3];
    tuslar[idx].w = TUS_W;
    tuslar[idx].h = TUS_H;
    tuslar[idx].renk = BLUE;
  }

  strcpy(tuslar[idx].label, "DEL");
  tuslar[idx].x = 5 * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
  tuslar[idx].y = satirY[3];
  tuslar[idx].w = 2 * (TUS_W + TUS_BOSLUK) - TUS_BOSLUK;
  tuslar[idx].h = TUS_H;
  tuslar[idx].renk = RED;
  idx++;

  strcpy(tuslar[idx].label, "BCK");
  tuslar[idx].x = TUS_BOSLUK;
  tuslar[idx].y = satirY[4];
  tuslar[idx].w = 1 * (TUS_W + TUS_BOSLUK) - TUS_BOSLUK;
  tuslar[idx].h = TUS_H;
  tuslar[idx].renk = CYAN;
  idx++;

  strcpy(tuslar[idx].label, "SPC");
  tuslar[idx].x = 1 * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
  tuslar[idx].y = satirY[4];
  tuslar[idx].w = 4 * (TUS_W + TUS_BOSLUK) - TUS_BOSLUK;
  tuslar[idx].h = TUS_H;
  tuslar[idx].renk = GRAY;
  idx++;

  strcpy(tuslar[idx].label, "ENT");
  tuslar[idx].x = 5 * (TUS_W + TUS_BOSLUK) + TUS_BOSLUK;
  tuslar[idx].y = satirY[4];
  tuslar[idx].w = 2 * (TUS_W + TUS_BOSLUK) - TUS_BOSLUK;
  tuslar[idx].h = TUS_H;
  tuslar[idx].renk = GREEN;
}

// ── Cursor ────────────────────────────────────────────
int cursorX() {
  String g = metin;
  const int maxKar = 17;
  if ((int)g.length() > maxKar)
    g = g.substring(g.length() - maxKar);
  return 5 + g.length() * 12;
}

void cursorCiz(bool gorunsun) {
  int kx = cursorX();
  if (kx >= EKRAN_W - 5) return;
  uint16_t renk = gorunsun ? YELLOW : KOYU_GRI;
  tft.drawFastVLine(kx,     metinY + 8, 18, renk);
  tft.drawFastVLine(kx + 1, metinY + 8, 18, renk);
}

// ── Gelen kutu ────────────────────────────────────────
void gelenKutuCiz() {
  tft.fillRect(0, gelenY, EKRAN_W, gelenH, KOYU_GRI);
  tft.drawRect(0, gelenY, EKRAN_W, gelenH, CYAN);

  // Başlık etiketi
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.setCursor(4, gelenY + 4);
  tft.print("GELEN MESAJ");
}

// ── Yazma kutusu ─────────────────────────────────────
void metinKutuCiz() {
  tft.fillRect(0, metinY, EKRAN_W, metinH, KOYU_GRI);
  tft.drawRect(0, metinY, EKRAN_W, metinH, CYAN);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  String g = metin;
  const int maxKar = 17;
  if ((int)g.length() > maxKar)
    g = g.substring(g.length() - maxKar);

  tft.setCursor(5, metinY + 10);
  tft.print(g);

  cursorCiz(cursorGorunum);
}

// ── KLAVYE butonu çiz ─────────────────────────────────
void klavyeButonuCiz() {
  tft.fillRoundRect(KBD_BTN_X, KBD_BTN_Y, KBD_BTN_W, KBD_BTN_H, 6, BLUE);
  tft.drawRoundRect(KBD_BTN_X, KBD_BTN_Y, KBD_BTN_W, KBD_BTN_H, 6, WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(KBD_BTN_X + 20, KBD_BTN_Y + 16);
  tft.print("KLAVYE");
}

// ── Tuş çiz ───────────────────────────────────────────
void tusCiz(int i, bool basili) {
  uint16_t arkaRenk = basili ? WHITE : tuslar[i].renk;
  uint16_t yaziRenk = basili ? BLACK : WHITE;

  tft.fillRoundRect(tuslar[i].x, tuslar[i].y,
                    tuslar[i].w, tuslar[i].h, 4, arkaRenk);
  tft.drawRoundRect(tuslar[i].x, tuslar[i].y,
                    tuslar[i].w, tuslar[i].h, 4, WHITE);
  tft.setTextColor(yaziRenk);

  String et = String(tuslar[i].label);

  if (et.length() == 1) {
    tft.setTextSize(2);
    tft.setCursor(tuslar[i].x + (tuslar[i].w - 12) / 2,
                  tuslar[i].y + (tuslar[i].h - 14) / 2);
    tft.print(et);
  } else {
    String g = (et == "SPC") ? "SPACE" :
               (et == "ENT") ? "ENTER" :
               (et == "BCK") ? "BACK"  : et;
    tft.setTextSize(1);
    tft.setCursor(tuslar[i].x + (tuslar[i].w - (int)g.length() * 6) / 2,
                  tuslar[i].y + (tuslar[i].h - 8) / 2);
    tft.print(g);
  }
}

// ── Klavyeyi gizle → büyük mod ────────────────────────
void klavyeGizle() {
  klavyeGoruntu = false;
  gelenY = GELEN_Y_BUYUK;
  gelenH = GELEN_H_BUYUK;
  metinY = METIN_Y_BUYUK;
  metinH = METIN_H_BUYUK;

  tft.fillScreen(BLACK);
  gelenKutuCiz();
  metinKutuCiz();
  klavyeButonuCiz();
}

// ── Klavyeyi göster → küçük mod ───────────────────────
void klavyeGoster() {
  klavyeGoruntu = true;
  gelenY = GELEN_Y_KUCUK;
  gelenH = GELEN_H_KUCUK;
  metinY = METIN_Y_KUCUK;
  metinH = METIN_H_KUCUK;

  tft.fillScreen(BLACK);
  gelenKutuCiz();
  metinKutuCiz();
  for (int i = 0; i < TUS_SAYISI; i++) tusCiz(i, false);
}

// ── İlk ekran çizimi ─────────────────────────────────
void ekranCiz() {
  tft.fillScreen(BLACK);
  // Başlangıçta klavye kapalı → büyük mod
  gelenKutuCiz();
  metinKutuCiz();
  klavyeButonuCiz();
}

// ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(0);
  tuslariOlustur();
  ekranCiz();
}

void loop() {
  unsigned long simdi = millis();
  if (simdi - sonCursorZaman >= CURSOR_ARALIK) {
    sonCursorZaman = simdi;
    cursorGorunum  = !cursorGorunum;
    cursorCiz(cursorGorunum);
  }

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int px = map(p.x, TS_MINX, TS_MAXX, 240, 0);
    int py = map(p.y, TS_MINY, TS_MAXY, 0, 320);
    px = constrain(px, 0, 239);
    py = constrain(py, 0, 319);

    // ── Klavye KAPALI → sadece KLAVYE butonunu dinle ──
    if (!klavyeGoruntu) {
      if (px >= KBD_BTN_X && px < KBD_BTN_X + KBD_BTN_W &&
          py >= KBD_BTN_Y && py < KBD_BTN_Y + KBD_BTN_H) {
        klavyeGoster();
      }
      delay(150);
      return;
    }

    // ── Klavye AÇIK → tuşları işle ────────────────────
    for (int i = 0; i < TUS_SAYISI; i++) {
      if (px >= tuslar[i].x &&
          px <  tuslar[i].x + tuslar[i].w &&
          py >= tuslar[i].y &&
          py <  tuslar[i].y + tuslar[i].h) {

        tusCiz(i, true);
        delay(80);
        tusCiz(i, false);

        String et = String(tuslar[i].label);

        if (et == "DEL") {
          if (metin.length() > 0)
            metin.remove(metin.length() - 1);
        } else if (et == "BCK") {
          klavyeGizle();    // ← Klavyeyi kapat, büyük moda geç
        } else if (et == "SPC") {
          if ((int)metin.length() < METIN_MAX)
            metin += ' ';
        } else if (et == "ENT") {
          Serial.print("ENTER: ");
          Serial.println(metin);
          metin = "";
        } else {
          if ((int)metin.length() < METIN_MAX)
            metin += et;
        }

        cursorGorunum  = true;
        sonCursorZaman = millis();
        if (et != "BCK") metinKutuCiz();
        break;
      }
    }
    delay(150);
  }
}

// designed by Yakup