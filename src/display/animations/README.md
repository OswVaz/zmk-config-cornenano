# Biblioteca de animaciones de la OLED

Cada carpeta de aquí es una animación completa y guardada. Solo se compila una a
la vez y se elige con **una línea** en `config/corne.conf`:

```conf
CONFIG_CORNE_ANIMATION_NAVES=y
```

Después: `./build.sh` y flashear las dos mitades (`./flash.sh left`, `./flash.sh right`).

## Catálogo

| Opción en `corne.conf` | Carpeta | Qué hace | Notas |
|---|---|---|---|
| `CONFIG_CORNE_ANIMATION_MATRIX_RAIN=y` | `matrix_rain/` | Lluvia de katakana estilo Matrix. Cada tecla suelta una gota en el carril que queda bajo tu dedo. | Cada mitad es independiente. |
| `CONFIG_CORNE_ANIMATION_NAVES=y` | `naves/` | Nave contra aliens en una sola escena de 64×128 repartida entre las dos pantallas. La nave se pilotea sola y el juego sigue aunque no teclees; cada tecla lo acelera. Con `CONFIG_CORNE_ANIMATION_NAVES_AUTOPLAY=n` vuelve al modo viejo: solo avanza al teclear y en reposo los aliens se mecen. | Usa el nodo `animsync` del keymap para sincronizar las mitades por BLE. Hay que flashear **las dos** mitades. |

La alerta de batería (`../battery_alert.c`) se dibuja encima de cualquier animación.

## Agregar una animación nueva

1. Crea `animations/<nombre>/<nombre>.c` con una función:
   ```c
   #include "animation.h"
   void animation_init(lv_obj_t *screen) { /* objetos LVGL + lv_timer_create(...) */ }
   ```
   `screen` mide 128×32 en coordenadas de LVGL. Físicamente la OLED va en
   vertical: el borde de arriba es `x = 127`.
2. En `Kconfig` (raíz del repo) agrega una opción dentro de `choice CORNE_ANIMATION`,
   con los `select` de los widgets LVGL que use (`LV_USE_CANVAS`, etc.).
3. En `CMakeLists.txt` agrega su bloque `if(CONFIG_CORNE_ANIMATION_<NOMBRE>)`.
4. Agrégala a la tabla de arriba.

## Detalles de `naves/`

- `naves_core.c`: la simulación y el dibujo, en C puro. Es determinista: con el
  mismo seed y el mismo número de pasos, las dos mitades llegan al mismo estado.
  Se puede compilar en la Mac para probarlo sin flashear.
- `naves.c`: el loop de LVGL. La izquierda cuenta teclas y decide hasta qué paso
  se avanza; la derecha solo sigue.
- `naves_sync.c`: el canal entre mitades. Manda 3 números (seed de la oleada, paso
  en que arrancó y paso objetivo) cuando cambian, más un latido cada ~2 s.
- Si la escena sale de cabeza o en espejo, ajusta `NAVES_TOP_IS_LVGL_X_MAX` o
  `NAVES_MIRROR_X` en `naves_core.h`.
- Autoplay (default): cada mitad suma un paso por frame con su propio reloj y la
  central la corrige con el latido o cuando hay teclas. Apágalo con
  `CONFIG_CORNE_ANIMATION_NAVES_AUTOPLAY=n` en `corne.conf`.
- Ritmo y dificultad: `NAVES_STEPS_PER_KEY` en `naves.c`; `BOMB_CHANCE`,
  `FORMATION_DESCEND`, `FIRE_COOLDOWN`, etc. en `naves_core.c`.
