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
#define TURUNCU  0xFD20
#define MOR      0x780F

#define EKRAN_W   240
#define EKRAN_H   320

// ── Uygulama durumu ───────────────────────────────────
enum Ekran { SPLASH, MENU, MESAJ, OYUN };
Ekran aktifEkran = SPLASH;

// ── Menü butonu alanları ──────────────────────────────
#define GAME_BTN_X   20
#define GAME_BTN_Y   160
#define GAME_BTN_W   90
#define GAME_BTN_H   50

#define MSG_BTN_X    130
#define MSG_BTN_Y    160
#define MSG_BTN_W    90
#define MSG_BTN_H    50

bool klavyeGoruntu = false;

#define GELEN_Y_BUYUK   0
#define GELEN_H_BUYUK   220
#define METIN_Y_BUYUK   225
#define METIN_H_BUYUK   35
#define GELEN_Y_KUCUK   0
#define GELEN_H_KUCUK   35
#define METIN_Y_KUCUK   38
#define METIN_H_KUCUK   35

#define KBD_BTN_X  70
#define KBD_BTN_Y  270
#define KBD_BTN_W  100
#define KBD_BTN_H  40

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

// ════════════════════════════════════════════════════
//  AÇILIŞ ANİMASYONU
// ════════════════════════════════════════════════════
void splashEkrani() {
  tft.fillScreen(BLACK);

  // ── Logo metni: harf harf beliriyor ──────────────
  const char* logo = "MUCOL";
  int startX = 40;
  tft.setTextSize(3);
  for (int i = 0; i < (int)strlen(logo); i++) {
    tft.setTextColor(CYAN);
    tft.setCursor(startX + i * 22, 80);
    tft.print(logo[i]);
    delay(80);
  }

  // Alt başlık
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(55, 120);
  tft.print("Touch Interface v1.0");

  delay(400);

  // ── Progress bar ─────────────────────────────────
  int barX = 20, barY = 200, barW = 200, barH = 14;
  tft.drawRoundRect(barX, barY, barW, barH, 4, CYAN);

  for (int i = 0; i <= barW - 4; i += 4) {
    uint16_t renk;
    if      (i < (barW - 4) / 3)       renk = BLUE;
    else if (i < 2 * (barW - 4) / 3)   renk = CYAN;
    else                                 renk = GREEN;

    tft.fillRect(barX + 2, barY + 2, i, barH - 4, renk);
    delay(12);
  }

  // Yüzde göster
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(barX + barW + 6, barY + 3);
  tft.print("100%");

  delay(300);

  // ── "Hazır" mesajı ────────────────────────────────
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.setCursor(75, 230);
  tft.print("Hazir!");
  delay(500);

  // ── Beyazdan solma efekti ─────────────────────────
  for (int alpha = 0; alpha < 8; alpha++) {
    tft.fillScreen(WHITE);
    delay(30);
    tft.fillScreen(BLACK);
    delay(30);
  }
}

// ════════════════════════════════════════════════════
//  ANA MENÜ
// ════════════════════════════════════════════════════
void menuCiz() {
  tft.fillScreen(BLACK);

  // Başlık
  tft.setTextColor(CYAN);
  tft.setTextSize(2);
  tft.setCursor(40, 30);
  tft.print("ANA MENU");

  tft.drawFastHLine(0, 55, EKRAN_W, CYAN);

  // Dekoratif ikon alanı
  tft.setTextColor(GRAY);
  tft.setTextSize(1);
  tft.setCursor(20, 75);
  tft.print("Bir mod secin:");

  // ── GAME butonu ───────────────────────────────────
  tft.fillRoundRect(GAME_BTN_X, GAME_BTN_Y, GAME_BTN_W, GAME_BTN_H, 8, KOYU_GRI);
  tft.drawRoundRect(GAME_BTN_X, GAME_BTN_Y, GAME_BTN_W, GAME_BTN_H, 8, GREEN);
  // Küçük oyun ikonu (basit)
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(GAME_BTN_X + 8, GAME_BTN_Y + 6);
  tft.print("GAME");
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(GAME_BTN_X + 18, GAME_BTN_Y + 35);
  tft.print("Oyun");

  // ── MESSAGE butonu ────────────────────────────────
  tft.fillRoundRect(MSG_BTN_X, MSG_BTN_Y, MSG_BTN_W, MSG_BTN_H, 8, KOYU_GRI);
  tft.drawRoundRect(MSG_BTN_X, MSG_BTN_Y, MSG_BTN_W, MSG_BTN_H, 8, CYAN);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(MSG_BTN_X + 4, MSG_BTN_Y + 6);
  tft.print("MSG");
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(MSG_BTN_X + 12, MSG_BTN_Y + 35);
  tft.print("Mesaj");

  // Alt bilgi
  tft.setTextColor(KOYU_GRI);
  tft.setTextSize(1);
  tft.setCursor(30, 290);
  tft.print("designed by Yakup");
}

// ════════════════════════════════════════════════════
//  MESAJ EKRANI (mevcut kodun tamamı)
// ════════════════════════════════════════════════════
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
  tuslar[idx].y = satirY[3]; tuslar[idx].w = 2*(TUS_W+TUS_BOSLUK)-TUS_BOSLUK;
  tuslar[idx].h = TUS_H; tuslar[idx].renk = RED; idx++;

  strcpy(tuslar[idx].label, "BCK");
  tuslar[idx].x = TUS_BOSLUK; tuslar[idx].y = satirY[4];
  tuslar[idx].w = 1*(TUS_W+TUS_BOSLUK)-TUS_BOSLUK;
  tuslar[idx].h = TUS_H; tuslar[idx].renk = CYAN; idx++;

  strcpy(tuslar[idx].label, "SPC");
  tuslar[idx].x = 1*(TUS_W+TUS_BOSLUK)+TUS_BOSLUK; tuslar[idx].y = satirY[4];
  tuslar[idx].w = 4*(TUS_W+TUS_BOSLUK)-TUS_BOSLUK;
  tuslar[idx].h = TUS_H; tuslar[idx].renk = GRAY; idx++;

  strcpy(tuslar[idx].label, "ENT");
  tuslar[idx].x = 5*(TUS_W+TUS_BOSLUK)+TUS_BOSLUK; tuslar[idx].y = satirY[4];
  tuslar[idx].w = 2*(TUS_W+TUS_BOSLUK)-TUS_BOSLUK;
  tuslar[idx].h = TUS_H; tuslar[idx].renk = GREEN;
}

int cursorX() {
  String g = metin;
  const int maxKar = 17;
  if ((int)g.length() > maxKar) g = g.substring(g.length() - maxKar);
  return 5 + g.length() * 12;
}
void cursorCiz(bool gorunsun) {
  int kx = cursorX();
  if (kx >= EKRAN_W - 5) return;
  uint16_t renk = gorunsun ? YELLOW : KOYU_GRI;
  tft.drawFastVLine(kx,     metinY + 8, 18, renk);
  tft.drawFastVLine(kx + 1, metinY + 8, 18, renk);
}
void gelenKutuCiz() {
  tft.fillRect(0, gelenY, EKRAN_W, gelenH, KOYU_GRI);
  tft.drawRect(0, gelenY, EKRAN_W, gelenH, CYAN);
  tft.setTextColor(CYAN); tft.setTextSize(1);
  tft.setCursor(4, gelenY + 4); tft.print("GELEN MESAJ");
}
void metinKutuCiz() {
  tft.fillRect(0, metinY, EKRAN_W, metinH, KOYU_GRI);
  tft.drawRect(0, metinY, EKRAN_W, metinH, CYAN);
  tft.setTextColor(WHITE); tft.setTextSize(2);
  String g = metin;
  const int maxKar = 17;
  if ((int)g.length() > maxKar) g = g.substring(g.length() - maxKar);
  tft.setCursor(5, metinY + 10); tft.print(g);
  cursorCiz(cursorGorunum);
}
void klavyeButonuCiz() {
  tft.fillRoundRect(KBD_BTN_X, KBD_BTN_Y, KBD_BTN_W, KBD_BTN_H, 6, BLUE);
  tft.drawRoundRect(KBD_BTN_X, KBD_BTN_Y, KBD_BTN_W, KBD_BTN_H, 6, WHITE);
  tft.setTextColor(WHITE); tft.setTextSize(1);
  tft.setCursor(KBD_BTN_X + 20, KBD_BTN_Y + 16); tft.print("KLAVYE");
}
void tusCiz(int i, bool basili) {
  uint16_t arkaRenk = basili ? WHITE : tuslar[i].renk;
  uint16_t yaziRenk = basili ? BLACK : WHITE;
  tft.fillRoundRect(tuslar[i].x, tuslar[i].y, tuslar[i].w, tuslar[i].h, 4, arkaRenk);
  tft.drawRoundRect(tuslar[i].x, tuslar[i].y, tuslar[i].w, tuslar[i].h, 4, WHITE);
  tft.setTextColor(yaziRenk);
  String et = String(tuslar[i].label);
  if (et.length() == 1) {
    tft.setTextSize(2);
    tft.setCursor(tuslar[i].x + (tuslar[i].w - 12) / 2, tuslar[i].y + (tuslar[i].h - 14) / 2);
    tft.print(et);
  } else {
    String g = (et=="SPC")?"SPACE":(et=="ENT")?"ENTER":(et=="BCK")?"BACK":et;
    tft.setTextSize(1);
    tft.setCursor(tuslar[i].x + (tuslar[i].w - (int)g.length()*6)/2, tuslar[i].y + (tuslar[i].h - 8)/2);
    tft.print(g);
  }
}
void klavyeGizle() {
  klavyeGoruntu = false;
  gelenY = GELEN_Y_BUYUK; gelenH = GELEN_H_BUYUK;
  metinY = METIN_Y_BUYUK; metinH = METIN_H_BUYUK;
  tft.fillScreen(BLACK);
  gelenKutuCiz(); metinKutuCiz(); klavyeButonuCiz();
}
void klavyeGoster() {
  klavyeGoruntu = true;
  gelenY = GELEN_Y_KUCUK; gelenH = GELEN_H_KUCUK;
  metinY = METIN_Y_KUCUK; metinH = METIN_H_KUCUK;
  tft.fillScreen(BLACK);
  gelenKutuCiz(); metinKutuCiz();
  for (int i = 0; i < TUS_SAYISI; i++) tusCiz(i, false);
}

// ── Mesaj ekranı giriş noktası ────────────────────────
void mesajEkraniBaslat() {
  klavyeGoruntu = false;
  gelenY = GELEN_Y_BUYUK; gelenH = GELEN_H_BUYUK;
  metinY = METIN_Y_BUYUK; metinH = METIN_H_BUYUK;
  tft.fillScreen(BLACK);
  gelenKutuCiz(); metinKutuCiz(); klavyeButonuCiz();
}

// ════════════════════════════════════════════════════
//  OYUN (placeholder)
// ════════════════════════════════════════════════════
void oyunEkraniBaslat() {
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.setCursor(30, 100);
  tft.print("OYUN MODU");
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(25, 140);
  tft.print("Oyun kodunu buraya ekle");

  // Geri butonu
  tft.fillRoundRect(70, 260, 100, 40, 6, KOYU_GRI);
  tft.drawRoundRect(70, 260, 100, 40, 6, RED);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.setCursor(100, 278);
  tft.print("< MENU");
}

// ════════════════════════════════════════════════════
//  SETUP / LOOP
// ════════════════════════════════════════════════════
void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9486;
  tft.begin(ID);
  tft.setRotation(0);
  tuslariOlustur();

  // Açılış animasyonu → menü
  splashEkrani();
  aktifEkran = MENU;
  menuCiz();
}

void loop() {
  unsigned long simdi = millis();

  // Cursor sadece mesaj modunda yanıp söner
  if (aktifEkran == MESAJ && simdi - sonCursorZaman >= CURSOR_ARALIK) {
    sonCursorZaman = simdi;
    cursorGorunum  = !cursorGorunum;
    cursorCiz(cursorGorunum);
  }

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z < MINPRESSURE || p.z > MAXPRESSURE) return;

  int px = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  int py = map(p.y, TS_MINY, TS_MAXY, 0, 320);
  px = constrain(px, 0, 239);
  py = constrain(py, 0, 319);

  // ── MENÜ ─────────────────────────────────────────
  if (aktifEkran == MENU) {
    if (px >= GAME_BTN_X && px < GAME_BTN_X + GAME_BTN_W &&
        py >= GAME_BTN_Y && py < GAME_BTN_Y + GAME_BTN_H) {
      aktifEkran = OYUN;
      oyunEkraniBaslat();
    }
    else if (px >= MSG_BTN_X && px < MSG_BTN_X + MSG_BTN_W &&
             py >= MSG_BTN_Y && py < MSG_BTN_Y + MSG_BTN_H) {
      aktifEkran = MESAJ;
      mesajEkraniBaslat();
    }
    delay(150);
    return;
  }

  // ── OYUN ─────────────────────────────────────────
  if (aktifEkran == OYUN) {
    // Geri butonu
    if (px >= 70 && px < 170 && py >= 260 && py < 300) {
      aktifEkran = MENU;
      menuCiz();
    }
    delay(150);
    return;
  }

  // ── MESAJ: klavye kapalı ──────────────────────────
  if (aktifEkran == MESAJ && !klavyeGoruntu) {
    if (px >= KBD_BTN_X && px < KBD_BTN_X + KBD_BTN_W &&
        py >= KBD_BTN_Y && py < KBD_BTN_Y + KBD_BTN_H) {
      klavyeGoster();
    }
    delay(150);
    return;
  }

  // ── MESAJ: klavye açık ────────────────────────────
  if (aktifEkran == MESAJ && klavyeGoruntu) {
    for (int i = 0; i < TUS_SAYISI; i++) {
      if (px >= tuslar[i].x && px < tuslar[i].x + tuslar[i].w &&
          py >= tuslar[i].y && py < tuslar[i].y + tuslar[i].h) {

        tusCiz(i, true); delay(80); tusCiz(i, false);
        String et = String(tuslar[i].label);

        if (et == "DEL") {
          if (metin.length() > 0) metin.remove(metin.length() - 1);
        } else if (et == "BCK") {
          klavyeGizle();
        } else if (et == "SPC") {
          if ((int)metin.length() < METIN_MAX) metin += ' ';
        } else if (et == "ENT") {
          Serial.print("ENTER: "); Serial.println(metin);
          metin = "";
        } else {
          if ((int)metin.length() < METIN_MAX) metin += et;
        }

        cursorGorunum = true; sonCursorZaman = millis();
        if (et != "BCK") metinKutuCiz();
        break;
      }
    }
    delay(150);
  }
}

// designed by Yakup