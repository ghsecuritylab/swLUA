## Moduł RTOS

`rtos.yield()` - Wraca do schedulera systemowego RTOS. Działą jak `taskYIELD()` w C.

`rtos.delay(ms)` - Wraca do schedulera systemowego RTOS na minimum `ms` milisekund. Działa jak `osDelay(ms)` w C.

`rtos.get_milis()` - Zwraca aktualny czas (od uruchomienia urządzenia) w milisekundach

`rtos.get_seconds()` - Zwraca aktualny czas (od uruchomienia urządzenia) w sekundach (float)

## Moduł TS (TouchScreen)

`ts.get_status()` - zwraca wartość ostatniego skanu TouchScreen. 
Struktura wyjściowa: `{ touches = [{x = 123, y = 23}, {...}, ...], gesture = GESTURE_ID }`
Gesture jest wynikiem wykrywania podstawowych gestów i jest jedną z poniższych wartości:
 - GESTURE_NONE - brak gestu
 - GESTURE_MOVE_UP - ... dokończyć
 - GESTURE_MOVE_RIGHT
 - GESTURE_MOVE_DOWN
 - GESTURE_MOVE_LEFT
 - GESTURE_ZOOM_IN
 - GESTURE_ZOOM_OUT

## Moduł LCD

`lcd.get_screen_size()` - zwraca wymiary ekranu jako parę: `{ x = x_size, y = y_size }`

`lcd.set_text_color(color)` - ustawia kolor rysowanych obiektów.

`lcd.get_text_color()` - zwraca aktualnie używany kolor.

`lcd.set_back_color(color)` - ustawia kolor wypełniania tła rysowanych obiektów.

`lcd.get_back_color()` - zwraca aktualnie używany kolor tła rysowanych obiektów.

`lcd.read_pixel(x, y)` - zwraca kolor pixela na danej pozycji. UWAGA! nie wychodzić poza zakres ekranu.

`lcd.draw_pixel(x, y, color)` - wypełnij pixel na dajen pozycji wybranym kolorem.

`lcd.clear(color)` - wyczyść bufor danym kolorem

`lcd.clear_string_line(line)` - wyczyść podaną linię (rysowanie tekstu)

`lcd.display_string(line, text)` - Wyświetl podany tekst na danej linii

`lcd.display_string(x, y, text, alignment)` - Wyświetl dany text na danej pozycji przy wygbranym układzie.

`lcd.display_char(x, y, c)` - Wyświetl znak na wybranej pozycji

`lcd.draw_horizontal_line(x, y, length)` - Narysuj poziomą linię o danej długości na wybranej pozycji.

`lcd.draw_vertical_line(x, y, length)` - Narysuj pionową linię o danej długości na wybranej pozycji.

`lcd.draw_line(x1, y1, x2, y2)` - Naryzuj linię pomiędzy wybranymi punktami.

`lcd.draw_rect(x1, y1, x2, y2)` - Naryzuj pusty prostokąt poniędzy danymi punktami.

`lcd.draw_circle(x, y, r)` - Narysuj okrąg o promieniu `r` na pozycji `x, y`

`lcd.draw_ellipse(x, y, rx, ry)` - Narysuj pustą elipsę na pozycji `x, y` o promieniach `rx, ry`

`lcd.fill_rect(x1, y1, x2, y2)` - Narysuj pełny prostokąt pomiędzy wybranymi punktami

`lcd.fill_circle(x, y, r)` - Narysuj pełne koło o promieniu `r` na pozycji `x, y`

`lcd.fill_ellipse(x, y, rx, ry)` - Narysuj pełną elipsę na pozycji `x, y` o promieniach `rx, ry`

`lcd.swap_buffers()` - PRzenieś obecnie rysowaną klatkę na bufor przedni i wyświetl ją, a poprzednią ustaw jako niewidoczną i gotową do rysowania.

`lcd.make_color(r, g, b, a)` - Stwórz kolor o parametrach `r g b a`. Parametry powinny byś z zakresu `[0.0; 1.0]`

### Stałe LCD

#### Kolory
- lcd.COLOR_BLUE
- lcd.COLOR_GREEN
- lcd.COLOR_RED
- lcd.COLOR_CYAN
- lcd.COLOR_MAGENTA
- lcd.COLOR_YELLOW
- lcd.COLOR_LIGHTBLUE
- lcd.COLOR_LIGHTGREEN
- lcd.COLOR_LIGHTRED
- lcd.COLOR_LIGHTCYAN
- lcd.COLOR_LIGHTMAGENTA
- lcd.COLOR_DARKBLUE
- lcd.COLOR_DARKGREEN
- lcd.COLOR_DARKRED
- lcd.COLOR_DARKCYAN
- lcd.COLOR_DARKMAGENTA
- lcd.COLOR_DARKYELLOW
- lcd.COLOR_WHITE
- lcd.COLOR_LIGHTGRAY
- lcd.COLOR_GRAY
- lcd.COLOR_DRKGRAY
- lcd.COLOR_BLACK
- lcd.COLOR_BROWN
- lcd.COLOR_ORANGE
- lcd.COLOR_TRANSPARENT

#### Alignment dla tekstu
- lcd.CENTER_MODE
- lcd.RIGHT_MODE
- lcd.LEFT_MODE

#### Ilość ramek uzywanych do rysowania (N-buffering np. double-buffering dla 2 ramek)
- lcd.LAYERS

# Działanie platformy uruchomieniowej
Najprostszym opisem użycia jest:
 1. Uruchomienie urządzenia
 2. Podpięcie nośnika USB MSC (Pendrive) z programem napisanym w języku skryptowym LUA.
    Plik ze skryptem powinien być na głównej partycji nośnika oraz nazywać się `script.lua`

# Działanie wewnętrzne:
Najpierw inicjalizowane są wszystkie potrzebne komponenty znajdujące się na urządzeniu. 
Następnie system rozpoczyna inicjalizację maszyny wirtualnej języka LUA oraz rejestruje wszelkie potrzebne oraz dostępne moduły (LCS, RTOS, TS). W tym momencie następuje oczekiwanie na dostęp do pliku 1:/script.lua. W przypadku znalezienia pliku (czyli też nośnika) zostaje on załadowany i wykonany przez dostępna maszynę wirtualną.
**UWAGA!** Ze względu na stan projektu proszę nie wyciągać nośnika ze skryptem podczas jego wykonania.
Doprowadza to do problemów z synchronizacją dostępu do systemu plików i zatrzymanie wykonania programu.

# Działanie implementacji API Newlib C w celu zapewniena dostępu do standardowego API C dla LUA
Dostęp do plików został rozwiązany w następujący sposób:
Informacje o strukturach są przechowywane wraz z deskryptorem pliku w statucznej globalnej synchronizowanej tablicy. Zapewnia to symulację działania realnego systemu operacyjnego z dostępem do standardowych funkcji STDLIB (strumienie STDIN, STDOUT oraz STDERR są odpowiednio traktowane).

# Przykłady 
W repozytorium dostępne są przykłady użycia dostępnej platformy.

# Przyszłość i kierunek rozwoju platformy
W zamyśle autorów jest stworzenie uniwersalnej biblioteki (BSP-like) pozwalającej na użycie maszyny wirtualnej LUA do prostego rozszerzania funkcjonalności programu bazowego. Może to znaleźć zastosowanie między innymi w personalizacji systemów zautomatyzowanych oraz możliwość tworzenia przez społezność serwisów do dzielenia się gotowymi funkcjonalnościami.

#  Planowany sposób działania
Proponowanym sposobem działania jest możliwość sterowania maszyną wirtualną za pomocą kolejki rozkazów systemu operacyjnego.
Rozważana jest też możliwość uruchomienia modułu jako osobnego zadania, które nie zawłaszcza zasobów i wykonuje się jako częściowo odseparowany wątek.