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

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define CYAN    0x07FF
#define GRAY    0x8410

#define METIN_MAX  30
#define TUS_SAYISI 30

String metin = "";

// ── Cursor blink değişkenleri ──────────────────────────
unsigned long sonCursorZaman = 0;
bool          cursorGorunum  = true;
#define CURSOR_ARALIK 500   // ms — yarım saniyede bir yanıp söner
// ──────────────────────────────────────────────────────

struct Tus {
  char     label[6];
  int      x, y, w, h;
  uint16_t renk;
};

Tus tuslar[TUS_SAYISI];

void tuslariOlustur() {
  int sutunW = 34;
  int bosluk = 2;
  int satirY[5] = {38, 94, 150, 206, 262};
  int satirH    = 54;

  char harfler[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int idx = 0;

  for (int s = 0; s < 7; s++) {
    tuslar[idx].label[0] = harfler[s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * sutunW + bosluk;
    tuslar[idx].y = satirY[0];
    tuslar[idx].w = sutunW - bosluk;
    tuslar[idx].h = satirH - bosluk;
    tuslar[idx].renk = BLUE;
    idx++;
  }
  for (int s = 0; s < 7; s++) {
    tuslar[idx].label[0] = harfler[7 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * sutunW + bosluk;
    tuslar[idx].y = satirY[1];
    tuslar[idx].w = sutunW - bosluk;
    tuslar[idx].h = satirH - bosluk;
    tuslar[idx].renk = BLUE;
    idx++;
  }
  for (int s = 0; s < 7; s++) {
    tuslar[idx].label[0] = harfler[14 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * sutunW + bosluk;
    tuslar[idx].y = satirY[2];
    tuslar[idx].w = sutunW - bosluk;
    tuslar[idx].h = satirH - bosluk;
    tuslar[idx].renk = BLUE;
    idx++;
  }
  for (int s = 0; s < 5; s++) {
    tuslar[idx].label[0] = harfler[21 + s];
    tuslar[idx].label[1] = '\0';
    tuslar[idx].x = s * sutunW + bosluk;
    tuslar[idx].y = satirY[3];
    tuslar[idx].w = sutunW - bosluk;
    tuslar[idx].h = satirH - bosluk;
    tuslar[idx].renk = BLUE;
    idx++;
  }

  strcpy(tuslar[idx].label, "DEL");
  tuslar[idx].x = 5 * sutunW + bosluk;
  tuslar[idx].y = satirY[3];
  tuslar[idx].w = 2 * sutunW - bosluk;
  tuslar[idx].h = satirH - bosluk;
  tuslar[idx].renk = RED;
  idx++;

  strcpy(tuslar[idx].label, "SPC");
  tuslar[idx].x = bosluk;
  tuslar[idx].y = satirY[4];
  tuslar[idx].w = 5 * sutunW - bosluk;
  tuslar[idx].h = satirH - bosluk;
  tuslar[idx].renk = GRAY;
  idx++;

  strcpy(tuslar[idx].label, "ENT");
  tuslar[idx].x = 5 * sutunW + bosluk;
  tuslar[idx].y = satirY[4];
  tuslar[idx].w = 2 * sutunW - bosluk;
  tuslar[idx].h = satirH - bosluk;
  tuslar[idx].renk = GREEN;
  idx++;
}

// ── Cursor X pozisyonunu hesapla ──────────────────────
int cursorX() {
  String goster = metin;
  int maxKar = 14;
  if ((int)goster.length() > maxKar)
    goster = goster.substring(goster.length() - maxKar);
  return 5 + goster.length() * 12;
}
// ──────────────────────────────────────────────────────

void cursorCiz(bool gorunsun) {
  int kx = cursorX();
  if (kx >= 235) return;

  if (gorunsun) {
    // Cursor açık: sarı dikey çizgi
    tft.drawFastVLine(kx,     8, 18, YELLOW);
    tft.drawFastVLine(kx + 1, 8, 18, YELLOW);  // 2px kalınlık
  } else {
    // Cursor kapalı: arka plan rengiyle sil
    tft.drawFastVLine(kx,     8, 18, 0x2104);
    tft.drawFastVLine(kx + 1, 8, 18, 0x2104);
  }
}

void metinKutuCiz() {
  tft.fillRect(0, 0, 240, 35, 0x2104);
  tft.drawRect(0, 0, 240, 35, CYAN);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  String goster = metin;
  int maxKar = 14;
  if ((int)goster.length() > maxKar)
    goster = goster.substring(goster.length() - maxKar);

  tft.setCursor(5, 10);
  tft.print(goster);

  // Cursor durumuna göre hemen çiz
  cursorCiz(cursorGorunum);
}

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
    int tx = tuslar[i].x + (tuslar[i].w - 12) / 2;
    int ty = tuslar[i].y + (tuslar[i].h - 14) / 2;
    tft.setCursor(tx, ty);
    tft.print(et);
  } else {
    tft.setTextSize(1);
    String goster = et;
    if (et == "SPC") goster = "SPACE";
    else if (et == "ENT") goster = "ENTER";
    int tw = goster.length() * 6;
    int tx = tuslar[i].x + (tuslar[i].w - tw) / 2;
    int ty = tuslar[i].y + (tuslar[i].h - 8) / 2;
    tft.setCursor(tx, ty);
    tft.print(goster);
  }
}

void ekranCiz() {
  tft.fillScreen(BLACK);
  metinKutuCiz();
  for (int i = 0; i < TUS_SAYISI; i++) tusCiz(i, false);
}

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

  // ── Cursor blink — delay() yok, millis() ile ────────
  unsigned long simdi = millis();
  if (simdi - sonCursorZaman >= CURSOR_ARALIK) {
    sonCursorZaman = simdi;
    cursorGorunum  = !cursorGorunum;
    cursorCiz(cursorGorunum);   // sadece cursor pikselini güncelle
  }
  // ────────────────────────────────────────────────────

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    int px = map(p.x, TS_MINX, TS_MAXX, 240, 0);  // ters (aynalama düzeltmesi)
    int py = map(p.y, TS_MINY, TS_MAXY, 0,   320);

    px = constrain(px, 0, 239);
    py = constrain(py, 0, 319);

    for (int i = 0; i < TUS_SAYISI; i++) {
      if (px >= tuslar[i].x &&
          px <= tuslar[i].x + tuslar[i].w &&
          py >= tuslar[i].y &&
          py <= tuslar[i].y + tuslar[i].h) {

        tusCiz(i, true);
        delay(80);
        tusCiz(i, false);

        String et = String(tuslar[i].label);

        if (et == "DEL") {
          if (metin.length() > 0)
            metin = metin.substring(0, metin.length() - 1);
        } else if (et == "SPC") {
          if ((int)metin.length() < METIN_MAX)
            metin += " ";
        } else if (et == "ENT") {
          Serial.print("ENTER: ");
          Serial.println(metin);
          metin = "";
        } else {
          if ((int)metin.length() < METIN_MAX)
            metin += et;
        }

        // Harf basılınca cursor'u görünür yap ve timer'ı sıfırla
        cursorGorunum  = true;
        sonCursorZaman = millis();

        tft.fillRect(1, 1, 238, 33, 0x2104);
        metinKutuCiz();
        break;
      }
    }
    delay(150);
  }
}